#include "driver.h"
#include <string.h>
extern "C"
{
#include "lua/src/lua.h"
#include "lua/src/lauxlib.h"
#include "lua/src/lualib.h"
}
#include "script.h"

#define FUNCTION_INIT "on_init"

lua_State* g_L = NULL;
lua_State* get_lua()
{
	return g_L;
}

#define error(L,sig,...)  {printf(sig"\n", __VA_ARGS__);return -1;}

static int call_function(lua_State* L,const char* func)
{
	lua_getglobal(L,func);
	int err = lua_pcall(L,0,0,0);
	if (err)
		error(L,"call_function %s rt:%d %s",func,err,lua_tostring(L,-1));
	return err;
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
		lua_close(L);
		return NULL;
	}

	err = lua_call_function(FUNCTION_INIT);
	if (err)
	{
		lua_close(L);
		g_L = NULL;
		return NULL;
	}	
	return L;
}

void close_lua()
{
	lua_close(g_L);
	g_L = NULL;
}

//////////////////////////////////////////////////////////////////////////
#include <stdarg.h>

int lua_call_va (const char *func, const char *sig, ...) {
	va_list vl;
	int narg, nres;  /* number of arguments and results */
	lua_State* L = get_lua();

	va_start(vl, sig);
	lua_getglobal(L, func);  /* get function */

	/* push arguments */
	narg = 0;
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
	nres = strlen(sig);  /* number of expected results */
	if (lua_pcall(L, narg, nres, 0) != 0)  /* do the call */
		error(L, "error running function `%s': %s",
		func, lua_tostring(L, -1));

	/* retrieve results */
	nres = -nres;  /* stack index of first result */
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
	return 0;
}
