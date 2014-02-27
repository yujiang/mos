#include "window_render.h"

int window_render::s_texture_render = 0;
int window_render::s_box_render = 0;
int window_render::s_text_render = 0;
int window_render::s_image_render = 0;
int window_render::s_triangle_render = 0;
int window_render::s_triangle_render_prev = 0;

window_render::window_render(window* w) : m_window(w)
{
	m_rc_clip = NULL;  
	m_in_text = false;
	m_in_window = NULL;
	//m_in_map = NULL;

	is_batch = true;
	is_mul = true;
	is_thread = false;
}

window_render::~window_render()
{
	m_window = 0;
}

void window_render::render_start0()
{
	s_texture_render = 0;
	s_box_render = 0;
	s_text_render = 0;
	s_image_render = 0;
	s_triangle_render_prev = s_triangle_render;
	s_triangle_render = 0;
	render_start();
}


void window_render::render_end()
{
	//其下可以用一个渲染线程来做。
	m_render_start = false;
}
