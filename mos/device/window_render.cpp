#include "window_render.h"

window_render::window_render(window* w) : m_window(w)
{
	m_rc_clip = NULL;  
}

window_render::~window_render()
{
	m_window = 0;
}

