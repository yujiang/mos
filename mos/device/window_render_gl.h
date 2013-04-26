#ifndef __DEVICE_WINDOW_RENDER_GL_H_
#define __DEVICE_WINDOW_RENDER_GL_H_

#include "window_render.h"
#include <windows.h>
class director;

class window_render_gl : public window_render
{
public:
	window_render_gl(window* w);
	~window_render_gl();

	bool create_render(int width,int height);
	texture* create_texture() ;
	void on_destroy();

	void render_start();
	void render_end();

	int draw_texture_cell(const st_cell& cell,texture* tex,const g_rect* rc);
	int draw_texture(int x,int y,int color,int alpha,texture* _tex,const g_rect* rc);
	int draw_box_cell(const st_cell& cell,int w,int h);
	int draw_box(int x,int y,int color,int alpha,int w,int h);

	HDC  m_hDC;
	void swapBuffers();

	HGLRC m_hRC;
	director* m_director;
};

#endif
