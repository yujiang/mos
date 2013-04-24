#include "window_render_gl.h"
#include "window.h"
#include "graph/texture.h"
#include "graph/image.h"
#include <windows.h>
#include "OGLES/GL/glew.h"
#include "director.h"
//#include "kazmath/kazmath.h"
#include "texture_gl.h"
#include "graph/cell.h"
#include "graph/color.h"
#include "cocos2dx/shaders/ccGLStateCache.h"
#include "cocos2dx/include/ccTypes.h"
//#include "cocoa/ccMacros.h"
//#include "cocoa/CCGLProgram.h"
#include "cocos2dx/shaders/CCGLProgram.h"

USING_NS_CC;

window_render_gl::window_render_gl(window* w):window_render(w)
{
	m_director = new CCDirector(this);
}

window_render_gl::~window_render_gl()
{
	delete m_director;
	m_director = 0;
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
	

bool window_render_gl::create_render(int width,int height)
{
	m_hDC = GetDC((HWND)m_window->m_hWnd);
	SetupPixelFormat(m_hDC);
	//SetupPalette();
	m_hRC = wglCreateContext(m_hDC);
	wglMakeCurrent(m_hDC, m_hRC);

	// check OpenGL version at first
	const GLubyte* glVersion = glGetString(GL_VERSION);
	printf("OpenGL version = %s\n", glVersion);

	if ( atof((const char*)glVersion) < 1.5 )
	{
		printf("OpenGL 1.5 or higher is required (your version is %s). Please upgrade the driver of your video card.\n",
			glVersion);
		return false;
	}

	GLenum GlewInitResult = glewInit();
	if (GLEW_OK != GlewInitResult)
	{
		printf("OpenGL error %s\n",(char *)glewGetErrorString(GlewInitResult));
		return false;
	}

	if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
	{
		printf("Ready for GLSL\n");
	}
	else
	{
		printf("Not totally ready :(\n");
	}

	if (glewIsSupported("GL_VERSION_2_0"))
	{
		printf("Ready for OpenGL 2.0\n");
	}
	else
	{
		printf("OpenGL 2.0 not supported\n");
	}

	// Enable point size by default on windows. 
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

	//set projection 
	m_director->create_director();

	return true;
}

texture* window_render_gl::create_texture() 
{
	return new texture_gl;
}

void window_render_gl::on_destroy()
{
	if (m_hDC != NULL && m_hRC != NULL)
	{
		// deselect rendering context and delete it
		wglMakeCurrent(m_hDC, NULL);
		wglDeleteContext(m_hRC);
	}
}

void window_render_gl::render_start()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void window_render_gl::render_end()
{
	//kmGLPopMatrix();
	swapBuffers();
}

void window_render_gl::swapBuffers()
{
	if (m_hDC != NULL)
	{
		::SwapBuffers(m_hDC);
	}
}

#define kQuadSize sizeof(ccV3F_C4B_T2F)

//from sprite.
int window_render_gl::draw_texture(const st_cell& cell,texture* _tex,const g_rect* rc)
{
	texture_gl* tex = (texture_gl*)_tex;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex->m_textureId);

	ccGLEnableVertexAttribs( kCCVertexAttribFlag_PosColorTex );

	g_rect rect;
	rect = rc ? *rc : tex->get_rect();

	ccV3F_C4B_T2F_Quad m_sQuad;
	m_sQuad.tl.vertices.x = cell.x;
	m_sQuad.tl.vertices.y = cell.y;
	m_sQuad.tl.texCoords.u = (float)rect.l/tex->m_tex_width;
	m_sQuad.tl.texCoords.v = (float)rect.t/tex->m_tex_height;
	m_sQuad.br.vertices.x = cell.x + tex->m_width;
	m_sQuad.br.vertices.y = cell.y + tex->m_height;
	m_sQuad.br.texCoords.u = (float)rect.r/tex->m_tex_width;
	m_sQuad.br.texCoords.v = (float)rect.b/tex->m_tex_height;

	m_sQuad.bl.vertices.x = m_sQuad.tl.vertices.x;
	m_sQuad.bl.vertices.y = m_sQuad.br.vertices.y;
	m_sQuad.bl.texCoords.u = m_sQuad.tl.texCoords.u;
	m_sQuad.bl.texCoords.v = m_sQuad.br.texCoords.v ;
	m_sQuad.tr.vertices.x = m_sQuad.br.vertices.x ;
	m_sQuad.tr.vertices.y = m_sQuad.tl.vertices.y;
	m_sQuad.tr.texCoords.u = m_sQuad.br.texCoords.u;
	m_sQuad.tr.texCoords.v = m_sQuad.tl.texCoords.v;

	unsigned char r,g,b;
	G_GET_RGB(cell.color,r,g,b);
	ccColor4B c = ccc4(r,g,b,cell.alpha);
	ccV3F_C4B_T2F* p = &m_sQuad.tl;
	for (int i=0;i<4;i++,p++)
	{
		p->colors = c;	
		p->vertices.z = 1.f;
	}

	//filled the m_sQuad

	long offset = (long)&m_sQuad;

	// vertex
	int diff = offsetof( ccV3F_C4B_T2F, vertices);
	glVertexAttribPointer(kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, kQuadSize, (void*) (offset + diff));

	// texCoods
	diff = offsetof( ccV3F_C4B_T2F, texCoords);
	glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize, (void*)(offset + diff));

	// color
	diff = offsetof( ccV3F_C4B_T2F, colors);
	glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (void*)(offset + diff));

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	CHECK_GL_ERROR_DEBUG();

	return 0;
}

int window_render_gl::draw_box(const st_cell& cell,int w,int h)
{
	return 0;
}