#include "driver.h"
#include <string.h>
#include "lua.hpp"
#include "script.h"

#define FUNCTION_INIT "on_init"

lua_State* g_L = NULL;
lua_State* get_lua()
{
	return g_L;
}

#define error(L,sig,...)  {printf(sig"\n", __VA_ARGS__);return -1;}

int docall (lua_State *L, int narg, int nresults);
void on_enter_lua(lua_State *L, const char* where);
void on_exit_lua(lua_State *L, const char* where);

static int on_error(lua_State* L,const char* func,int rt)
{
	printf("error running function `%s': %s\n",func, lua_tostring(L, -1));
	lua_pop(L,1);
	on_exit_lua(L, func);
	return rt;
}

static int call_function(lua_State* L,const char* func)
{
	on_enter_lua(L, func);
	lua_getglobal(L,func);
	int rt = docall(L,0,0);
	if (rt != 0)
		return on_error(L,func,rt);
	on_exit_lua(L,func);
	return rt;
	//int err = lua_pcall(L,0,0,0);
	//if (err)
	//	error(L,"call_function %s rt:%d %s",func,err,lua_tostring(L,-1));
	//return err;
}

int lua_call_function(const char* func)
{
	return call_function(g_L,func);
}

int lua_dostring(const char* str)
{
	int err = luaL_dostring(g_L,str);
	if (err)
		printf("lua_dostring %s rt:%d %s\n",str,err,lua_tostring(g_L,-1));
	return err;
}

lua_State* init_lua()
{
	g_L = lua_open();
	lua_State* L = g_L;
	tolua_driver_open(L);
	luaL_openlibs(L);
	int err = luaL_dofile(L,"main.lua");
	if (err)
	{
		printf("main.lua %d %s\n",err,lua_tostring(L,-1));
	}
	else
	{
		err = lua_call_function(FUNCTION_INIT);
		if (err)
		{
			printf("main.lua %s() %d %s\n",FUNCTION_INIT,err,lua_tostring(L,-1));
		}
	}

	if (err)
	{
		//lua_close(L);
		//g_L = NULL;
		//return NULL;
	}	
	return L;
}

void close_lua()
{
	if (g_L)
	{
		lua_close(g_L);
		g_L = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
#include <stdarg.h>

int lua_call_va (const char *func, const char *sig, ...) {
	va_list vl;
	lua_State* L = get_lua();
	on_enter_lua(L, func);

	va_start(vl, sig);
	lua_getglobal(L, func);  /* get function */

	/* push arguments */
	int narg = 0;
	while (*sig) {  /* push arguments */
		switch (*sig++) {

		case 'd':  /* double argument */
			lua_pushnumber(L, va_arg(vl, double));
			break;

		case 'i':  /* int argument */
			lua_pushnumber(L, va_arg(vl, int));
			break;

		case 'b':  /* int argument */
			lua_pushboolean(L, va_arg(vl, int));
			break;

		case 's':  /* string argument */
			lua_pushstring(L, va_arg(vl, char *));
			break;

		case '>':
			goto endwhile;

		default:
			error(L, "invalid option (%c)", *(sig - 1));
		}
		narg++;
		luaL_checkstack(L, 1, "too many arguments");
	} endwhile:

	/* do the call */
	int res = strlen(sig);  /* number of expected results */
	int rt = docall(L, narg, res);
	if (rt)  /* do the call */
	{
		va_end(vl);
		return on_error(L,func,rt);
	}

	/* retrieve results */
	int nres = -res;  /* stack index of first result */
	while (*sig) {  /* get results */
		switch (*sig++) {

		case 'd':  /* double result */
			if (!lua_isnumber(L, nres))
				error(L, "wrong result type");
			*va_arg(vl, double *) = lua_tonumber(L, nres);
			break;

		case 'i':  /* int result */
			if (!lua_isnumber(L, nres))
				error(L, "wrong result type");
			*va_arg(vl, int *) = (int)lua_tonumber(L, nres);
			break;

		case 'b':  /* int result */
			if (!lua_isboolean(L, nres))
				error(L, "wrong result type");
			*va_arg(vl, int *) = (int)lua_toboolean(L, nres);
			break;

		case 's':  /* string result */
			if (!lua_isstring(L, nres))
				error(L, "wrong result type");
			*va_arg(vl, const char **) = lua_tostring(L, nres);
			break;

		default:
			error(L, "invalid option (%c)", *(sig - 1));
		}
		nres++;
	}
	va_end(vl);
	lua_pop(L,res);
	on_exit_lua(L, func);
	return 0;
}

//////////////////////////////////////////////////////////////////////////
//from tlqx server

static void InfiniteLoop(lua_State *L, lua_Debug *ar)
{
	lua_getglobal (L, "debug");
	lua_getfield(L, -1, "excepthook");
	if (!lua_isfunction(L, -1))
	{
		lua_pop(L, 2);
		return;
	}

	lua_pushstring(L, "infinite loop");
	lua_pcall(L, 1, 1, 0);  

	const char * traceback = luaL_checkstring(L, -1);
	printf("%s\n", traceback);

	//抛出异常中断掉。
	luaL_error(L,"infinite loop");
}


int ResetEvalCost(lua_State *L)
{
#define MAX_EVAL 200000000
	lua_sethook(L, InfiniteLoop, LUA_MASKCOUNT, MAX_EVAL);
	return 0;
}

int traceback (lua_State *L)
{
	//printf("top 11 %d\n",lua_gettop(L));
	lua_getglobal (L, "debug");
	//printf("top 12 %d\n",lua_gettop(L));
	lua_getfield(L, -1, "excepthook");
	//printf("top 14 %d\n",lua_gettop(L));
	if (!lua_isfunction(L, -1))
	{
		lua_pop(L, 2);
		//printf("top 15 %d\n",lua_gettop(L));
		return 1;
	}
	lua_pushvalue(L, 1);  //pass error message
	//printf("top 16 %d\n",lua_gettop(L));
	lua_pcall(L, 1, 1, 0);  //call debug.traceback
	//printf("top 17 %d\n",lua_gettop(L));
	//lua_pop(L, 2);
	//printf("top 18 %d\n",lua_gettop(L));
	return 1;
}

int docall (lua_State *L, int narg, int nresults)
{
	//return lua_pcall(L, narg, nresults, 0);
	//printf("top 1 %d\n",lua_gettop(L));
	int base = lua_gettop(L) - narg;  //function index
	lua_pushcfunction(L, traceback);  //push traceback function
	//printf("top 2 %d\n",lua_gettop(L));
	lua_insert(L, base);  //put it under chunk and args
	//printf("top 3 %d\n",lua_gettop(L));
	int status = lua_pcall(L, narg, nresults, base);
	//printf("top 4 %d\n",lua_gettop(L));
	lua_remove(L, base);  //remove traceback function
	//printf("top 5 %d\n",lua_gettop(L));
	return status;
}

static int old = 0;
//进入lua的入口函数。
void on_enter_lua(lua_State *L, const char* where)
{
	ResetEvalCost(L);	
	int n = lua_gettop(L);
	if (n != old)
	{
		//如果不是0，就有问题
		printf("on_enter_lua %s lua_gettop(L) = %d old %d\n",where,n,old);
		//lua_settop(L,0);	
		old = n;
	}
}

void stackdump_g(lua_State* l)
{
	int i;
	int top = lua_gettop(l);

	printf("total in stack %d\n",top);

	for (i = 1; i <= top; i++)
	{  /* repeat for each level */
		int t = lua_type(l, i);
		switch (t) {
		case LUA_TSTRING:  /* strings */
			printf("string: '%s'\n", lua_tostring(l, i));
			break;
		case LUA_TBOOLEAN:  /* booleans */
			printf("boolean %s\n",lua_toboolean(l, i) ? "true" : "false");
			break;
		case LUA_TNUMBER:  /* numbers */
			printf("number: %g\n", lua_tonumber(l, i));
			break;
		default:  /* other values */
			printf("%s\n", lua_typename(l, t));
			break;
		}
		printf("  ");  /* put a separator */
	}
	printf("\n");  /* end the listing */
}

void on_exit_lua(lua_State *L, const char* where)
{
	//因为存在没有正确的lua_pop的地方，导致luajit stackoverflow
	//详见: http://comments.gmane.org/gmane.comp.lang.lua.luajit/3173 

	int n = lua_gettop(L);
	if (n != old)
	{
		printf("on_exit_lua %s lua_gettop(L) = %d old %d\n",where,n,old);
		stackdump_g(L);
		old = n;
	}
}

