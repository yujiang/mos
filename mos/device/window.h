#ifndef __DEVICE_WINDOWN_H_
#define __DEVICE_WINDOWN_H_

#include "core/rect.h"
#include <string>

struct st_window_param : public g_rect
{
	int style;
	std::string render_type;
};

class window_render;
class window
{
public:
	bool m_destroy;
	void on_destroy();

	std::string m_name;
	st_window_param m_param;

	int m_width,m_height;
	int get_width() const{
		return m_width;
	}
	int get_height() const{
		return m_height;
	}
	bool create_window(const char* name,const char* title,st_window_param& st);

	//////////////////////////////////////////////////////////////////////////
	window_render* m_render;

	graph_hwnd m_hWnd;
	g_point m_pt_mouse;
	void set_mouse_point(g_point pt){
		m_pt_mouse = pt;
	}
	g_point get_mouse_point() const{
		return m_pt_mouse;
	}

	window();
	~window();

	void message_loop();
};

unsigned long get_time();
//const char* get_line_timeout(unsigned int timeout);
const char* get_input_string();
bool is_key_down(int key);


#endif
