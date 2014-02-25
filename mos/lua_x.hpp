#define LUA_BUILD_AS_DLL

#include "lua.hpp"

#ifdef WIN32
//always use lua as dll!
#if defined(LUA_CORE) || defined(LUA_LIB)
#else
#ifdef _DEBUG
#pragma comment(lib,"lua52_d.lib")
#pragma message("import lua52_d.lib")
#else
#pragma comment(lib,"lua52.lib")
//#pragma message("lua51.lib")
#endif
#endif
	
#endif