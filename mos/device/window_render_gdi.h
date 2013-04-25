#ifndef __DEVICE_WINDOW_RENDER_GDI_H_
#define __DEVICE_WINDOW_RENDER_GDI_H_

#include "window_render.h"
class window_render_gdi : public window_render
{
public:
	window_render_gdi(window* w);
	~window_render_gdi();

	bool create_render(int width,int height);
	texture* create_texture() ;
	void on_destroy();

	void render_start();
	void render_end();

	int draw_texture_cell(const st_cell& cell,texture* tex,const g_rect* rc);
	int draw_box_cell(const st_cell& cell,int w,int h);
	//int draw_text(const st_cell& cell,const st_cell& text);
	image* m_image;
};


#endif
