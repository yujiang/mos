#ifndef __DEVICE_WINDOW_RENDER_H_
#define __DEVICE_WINDOW_RENDER_H_

#include "graph/rect.h"
#include <string>

class texture;
class st_cell;
class image;
class window;

class window_render
{
public:
	window_render(window* w);
	virtual ~window_render();
	window* m_window;

	virtual texture* create_texture() = 0;
	virtual bool create_render(int width,int height) = 0;
	virtual void on_destroy() = 0;

	virtual void render_start() = 0;
	virtual void render_end() = 0;

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

	virtual int draw_texture_cell(const st_cell& cell,texture* tex,const g_rect* rc) = 0;
	virtual int draw_box_cell(const st_cell& cell,int w,int h) = 0;
	//int draw_text(const st_cell& cell,const st_cell& text);
	//image* m_image;
};


#endif