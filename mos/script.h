#ifndef __SCRIPT_H_
#define __SCRIPT_H_

struct lua_State;
lua_State* get_lua();

int lua_call_function(const char* func);
//call_va("f", "dd>d", x, y, &z);
int lua_call_va (const char *func, const char *sig, ...);

lua_State* init_lua(const char* arg);
void close_lua();

#endif