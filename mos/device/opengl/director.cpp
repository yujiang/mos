#include "director.h"
#include "window_render_gl.h"
#include "GL/glew.h"
#include "../window.h"
#include "gl_macro.h"
#include <assert.h>

director::director(window_render_gl* w)
{
	m_wgl = w;
	m_alpha_blending = false;
	m_depth_test = false;
	m_hDC = 0;
	m_hRC = 0;
}

void director::destroy_dc()
{
	if (m_hDC != NULL && m_hRC != NULL)
	{
		// deselect rendering context and delete it
		wglMakeCurrent(m_hDC, NULL);
		wglDeleteContext(m_hRC);
		m_hDC = NULL;
		m_hRC = NULL;
	}
}

static void SetupPixelFormat(HDC hDC)
{
	int pixelFormat;

	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),  // size
		1,                          // version
		PFD_SUPPORT_OPENGL |        // OpenGL window
		PFD_DRAW_TO_WINDOW |        // render to window
		PFD_DOUBLEBUFFER,           // support double-buffering
		PFD_TYPE_RGBA,              // color type
		32,                         // preferred color depth
		0, 0, 0, 0, 0, 0,           // color bits (ignored)
		0,                          // no alpha buffer
		0,                          // alpha bits (ignored)
		0,                          // no accumulation buffer
		0, 0, 0, 0,                 // accum bits (ignored)
		24,                         // depth buffer
		8,                          // no stencil buffer
		0,                          // no auxiliary buffers
		PFD_MAIN_PLANE,             // main layer
		0,                          // reserved
		0, 0, 0,                    // no layer, visible, damage masks
	};

	pixelFormat = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, pixelFormat, &pfd);
}

void director::create_dc()
{
	m_hDC = GetDC((HWND)m_wgl->m_window->m_hWnd);
	SetupPixelFormat(m_hDC);
	m_hRC = wglCreateContext(m_hDC);
	wglMakeCurrent(m_hDC, m_hRC);
}

void director::flip()
{
	if (m_hDC != NULL)
	{
		::SwapBuffers(m_hDC);
	}
}

void director::create_director()
{
	glEnable(GL_TEXTURE_2D);
	set_alpha_blending(true);
	set_projection();

	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void director::set_alpha_blending(bool bOn)
{
	if (m_alpha_blending == bOn)
		return;
	m_alpha_blending = bOn;
	if (m_wgl->m_render_start)
	{
		m_wgl->batch_set_alpha_blending(bOn);
	}
	else
	{
		_set_alpha_blending(bOn);
	}
}

void director::_set_alpha_blending(bool bOn)
{
	if (bOn)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
	{
		glDisable(GL_BLEND);
	}
	CHECK_GL_ERROR_DEBUG();
}
/*
void director::set_depth_test(bool bOn)
{
	if (m_depth_test == bOn)
		return;
	m_wgl->flush_draws();
	m_depth_test = bOn;
	if (bOn)
	{
		glClearDepth(1.0f);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}
	CHECK_GL_ERROR_DEBUG();
}
*/
void director::set_projection()
{
	int w = m_wgl->m_window->get_width();
	int h = m_wgl->m_window->get_height();
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,w,h,0,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	CHECK_GL_ERROR_DEBUG();
}

