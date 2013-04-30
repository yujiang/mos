#include "window_render_gdi.h"
#include "../window.h"
#include "texture_gdi.h"
#include "graph/image.h"
#include <windows.h>
#include <assert.h>

window_render_gdi::window_render_gdi(window* w) : window_render(w)
{
	m_image = 0;
}

window_render_gdi::~window_render_gdi()
{
	delete m_image;
	m_image = 0;
}

void window_render_gdi::on_destroy()
{
}

bool window_render_gdi::create_render(int width,int height)
{
	m_image = new image;
	m_image->create_image_dynamic(width,height,3);
	return true;
}

//先来个gdi的方法，先绘制到缓冲，在flip一下
texture* window_render_gdi::create_texture()
{
	texture_gdi* p = new texture_gdi;
	return p;
}

void window_render_gdi::render_start()
{
	//clear(0);
	m_image->clear(0);
}

void window_render_gdi::render_end()
{
	BITMAPINFO bmi;
	ZeroMemory(&bmi,sizeof(bmi));
	BITMAPINFOHEADER& h = bmi.bmiHeader;
	h.biSize = sizeof(BITMAPINFOHEADER);
	h.biWidth = m_image->m_width;
	h.biHeight = -m_image->m_height;
	h.biPlanes = 1;
	h.biBitCount = 24;
	h.biCompression = BI_RGB;
	h.biSizeImage = m_image->m_width * m_image->m_height;

	//flip();
	HDC dc = GetDC((HWND)m_window->m_hWnd);
	m_image->rgb2bgr();
	int err = SetDIBitsToDevice(dc,0,0,m_image->m_width,m_image->m_height,
		0,0,0,m_image->m_height,
		m_image->get_buffer() ,
		&bmi,
		DIB_RGB_COLORS);
	if (err <= 0)
	{
		//GDI_ERROR
		printf("error! SetDIBitsToDevice %d %d\n",err,GetLastError());
	}
	ReleaseDC((HWND)m_window->m_hWnd,dc);
}

int window_render_gdi::draw_image_cell(const st_cell& cell,image* img,const char* file,const g_rect* rc)
{
	s_image_render++;
	return m_image->draw_image_cell(cell,img,rc,m_rc_clip);
}

int window_render_gdi::draw_texture_cell(const st_cell& cell,texture* _tex, const g_rect* rc)
{
	s_texture_render++;
	return _draw_texture_cell(cell,_tex,rc);
}

int window_render_gdi::_draw_texture_cell(const st_cell& cell,texture* _tex, const g_rect* rc)
{
	texture_gdi* tex = (texture_gdi* )(_tex);
	return m_image->draw_image_cell(cell,tex->m_image,rc,m_rc_clip);
}

int window_render_gdi::draw_box_cell(const st_cell& cell,int w,int h)
{
	s_box_render++;
	return m_image->draw_box_cell(cell,w,h);
}

int window_render_gdi::draw_text_cell(const st_cell& cell,texture* tex,const g_rect* rc)
{
	s_text_render++;
	return _draw_texture_cell(cell,tex,rc);
}