#include "director.h"
#include "window_render_gl.h"
#include "OGLES/GL/glew.h"
//#include "kazmath/kazmath.h"
#include "window.h"
#include "gl_macro.h"
//#include "cocos2dx/shaders/ccGLStateCache.h"
//#include "cocos2dx/include/ccMacros.h"

//USING_NS_CC;

CCDirector::CCDirector(window_render_gl* w)
{
	m_wgl = w;
}

void CCDirector::create_director()
{
	setGLDefaultValues();
}

void CCDirector::setAlphaBlending(bool bOn)
{
//#define CC_BLEND_SRC GL_ONE
//#define CC_BLEND_DST GL_ONE_MINUS_SRC_ALPHA
	if (bOn)
	{
		glEnable(GL_BLEND);
		//glBlendFunc(CC_BLEND_SRC, CC_BLEND_DST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
	{
		glDisable(GL_BLEND);
	}
	CHECK_GL_ERROR_DEBUG();
}

void CCDirector::setDepthTest(bool bOn)
{
	if (bOn)
	{
		glClearDepth(1.0f);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		//        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}
	CHECK_GL_ERROR_DEBUG();
}

void CCDirector::setGLDefaultValues(void)
{
	glEnable(GL_TEXTURE_2D);

	setAlphaBlending(true);
	// XXX: Fix me, should enable/disable depth test according the depth format as cocos2d-iphone did
	// [self setDepthTest: view_.depthFormat];
		
	setDepthTest(false);
	setProjection();

	// set other opengl default values
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

//GLU is not implemented in the embedded version of the OpenGL package, OpenGL ES.

void CCDirector::setProjection()
{
	int w = m_wgl->m_window->get_width();
	int h = m_wgl->m_window->get_height();
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,w,h,0,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//ccSetProjectionMatrixDirty();
}

