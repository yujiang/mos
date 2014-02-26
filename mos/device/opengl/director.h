#ifndef __DEVICE_DIRECTOR_H_
#define __DEVICE_DIRECTOR_H_

#include <windows.h>
class window_render_gl;

class director
{
public:
	window_render_gl* m_wgl;
	void create_director();
	director(window_render_gl* w);

	void set_alpha_blending(bool bOn);
	void _set_alpha_blending(bool bOn);
	//void set_depth_test(bool bOn);
	void set_projection();

	bool m_alpha_blending;
	bool m_depth_test;
};

#endif