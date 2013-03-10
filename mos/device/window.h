#ifndef __DEVICE_WINDOWN_H_
#define __DEVICE_WINDOWN_H_

#include "graph/rect.h"
#include <string>

struct st_window_param : public g_rect
{
	int style;
};

class texture;
class st_cell;
class image;

class window
{
public:
	bool m_destroy;
	void on_destroy(){
		m_destroy = true;
	}

	std::string m_name;
	st_window_param m_param;
	bool create_window(const char* name,const char* title,st_window_param& st);
	void render_start();
	void render_end();
	
	g_rect m_rc_window;
	bool m_in_window;
	void window_start(int x,int y,int w,int h){
		m_rc_window.set_xywh(x,y,w,h);
		m_in_window = true;
	}
	void window_end(){
		m_in_window = false;
	}

	const g_rect* m_rc_clip;

	g_rect m_rc_text;
	bool m_in_text;
	void text_start(const g_rect& rc){
		m_rc_text = rc;
		m_in_text = true;
		m_rc_clip = &m_rc_text;
	}
	void text_end(){
		m_in_text = false;
		m_rc_clip = NULL;
	}

	int draw_texture(const st_cell& cell,texture* tex,const g_rect* rc);
	int draw_box(const st_cell& cell,int w,int h);
	//int draw_text(const st_cell& cell,const st_cell& text);
	image* m_image;

	//////////////////////////////////////////////////////////////////////////
	graph_hwnd m_hWnd;
	g_point m_pt_mouse;
	void set_mouse_point(g_point pt){
		m_pt_mouse = pt;
	}
	g_point get_mouse_point() const{
		return m_pt_mouse;
	}

	window(){m_image = 0;m_rc_clip = NULL;  m_destroy = false;}
	~window();

	void message_loop();
};

unsigned long get_time();
const char* get_line_timeout(unsigned int timeout);
bool is_key_down(int key);


#endif
