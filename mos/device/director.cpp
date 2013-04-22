#include "director.h"
#include "window_render_gl.h"
#include "OGLES/GL/glew.h"
#include "kazmath/kazmath.h"
#include "window.h"
#include "cocos2dx/shaders/ccGLStateCache.h"
#include "cocos2dx/include/ccMacros.h"

USING_NS_CC;

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
	if (bOn)
	{
		glEnable(GL_BLEND);
		glBlendFunc(CC_BLEND_SRC, CC_BLEND_DST);
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
	setAlphaBlending(true);
	// XXX: Fix me, should enable/disable depth test according the depth format as cocos2d-iphone did
	// [self setDepthTest: view_.depthFormat];

	setDepthTest(false);
	setProjection();

	// set other opengl default values
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}


void CCDirector::setViewPortInPoints(float x , float y , float w , float h)
{
	glViewport(x,y,w,h);
}

void CCDirector::setScissorInPoints(float x , float y , float w , float h)
{
	glScissor(x,y,w,h);
}


void CCDirector::setProjection()
{
	int w = m_wgl->m_window->get_width();
	int h = m_wgl->m_window->get_height();

	setViewPortInPoints(0, 0, w,h);

	kmGLMatrixMode(KM_GL_PROJECTION);
	kmGLLoadIdentity();
	kmMat4 orthoMatrix;
	kmMat4OrthographicProjection(&orthoMatrix, 0, w, 0, h, -1024, 1024 );
	kmGLMultMatrix(&orthoMatrix);
	kmGLMatrixMode(KM_GL_MODELVIEW);
	kmGLLoadIdentity();

	//ccSetProjectionMatrixDirty();
}

