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

#include "image/image_zgp.h"
#include "image/map_tl.h"
//////////////////////////////////////////////////////////////////////////
//custom your game source == call it from script.
void custom_game_source()
{
	get_graph()->regist_file_source(get_file_source_zgp());
	get_map()->register_map_source("map",&get_map_source_tl);
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

