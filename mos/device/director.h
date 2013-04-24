#ifndef __DEVICE_DIRECTOR_H_
#define __DEVICE_DIRECTOR_H_

#include <windows.h>
class window_render_gl;

class CCDirector
{
public:
	//HDC  m_hDC;
	//HGLRC m_hRC;
	window_render_gl* m_wgl;
	void create_director();
	CCDirector(window_render_gl* w);

	void setAlphaBlending(bool bOn);
	void setDepthTest(bool bOn);
	void setGLDefaultValues(void);
	void setProjection();
};

#endif