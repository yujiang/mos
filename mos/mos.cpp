// mos.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "driver.h"
#include "device/window.h"
#include "device/window_render.h"
#include "script.h"
#include "graph/graph.h"
#include "graph/map.h"
#include "mos.h"

window* g_window = NULL;
unsigned int g_time_now = 0;

window_render* get_render()
{
	return g_window->m_render;
}

window_render_gl* get_render_gl()
{
	return (window_render_gl*)g_window->m_render;
}

bool init_window(const char* name,const char* title,st_window_param& st)
{
	if (g_window)
		return false;
	window* w = new window();
	if (w->create_window(name,title,st))
	{
		g_window = w;
		return true;
	}
	delete w;
	return NULL;
}


bool window_run()
{
	if (!g_window || g_window->m_destroy)
		return false;

	std::string s = get_line_timeout(10);
	if (!s.empty())
	{
		if (s == "q" || s == "quit")
			return false;
		lua_call_va("on_input","s",s.c_str());
	}

	g_window->message_loop();
	if (g_window->m_destroy)
		return false;

	g_time_now = get_time();
	get_render()->render_start0();
	lua_call_function("on_every_frame");
	get_render()->render_end();

	return true;
}

//////////////////////////////////////////////////////////////////////////
//custom your game source == call it from script.
#include "tl/tl.h"
void custom_game_source()
{
	init_tl();
}

int _tmain(int argc, _TCHAR* argv[])
{
	get_graph()->init_graph();
	
	custom_game_source();

	init_lua();

	while(1)
	{
		if (!window_run())
			break;
	}

	delete g_window;
	g_window = NULL;

	return 0;
}

