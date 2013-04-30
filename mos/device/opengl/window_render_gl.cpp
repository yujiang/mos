#include "window_render_gl.h"
#include "../window.h"
#include "graph/texture.h"
#include "graph/image.h"
#include "graph/graph.h"
#include <windows.h>
#include "OGLES/GL/glew.h"
#include "director.h"
#include "texture_gl.h"
#include "graph/cell.h"
#include "graph/color.h"
#include "gl_macro.h"

window_render_gl::window_render_gl(window* w):window_render(w)
{
	m_director = new director(this);
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
	glClear(GL_COLOR_BUFFER_BIT);
}

void window_render_gl::render_end()
{
	swapBuffers();
}

void window_render_gl::swapBuffers()
{
	if (m_hDC != NULL)
	{
		::SwapBuffers(m_hDC);
	}
}

struct s_f2
{    
	GLint x;
	GLint y;
};
struct s_uv
{
	GLfloat u;
	GLfloat v;
};

int window_render_gl::draw_texture_cell(const st_cell& cell,texture* _tex,const g_rect* rc)
{
	s_texture_render++;
	return _draw_texture_cell(cell,_tex,rc);
}

int window_render_gl::_draw_texture_cell(const st_cell& cell,texture* _tex,const g_rect* rc)
{
	return draw_texture(cell.x,cell.y,cell.color,cell.alpha,_tex,rc);
}

int window_render_gl::draw_texture(int x,int y,int color,int alpha,texture* _tex,const g_rect* rc_tex)
{
	texture_gl* tex = (texture_gl*)_tex;
	g_rect rect = rc_tex ? *rc_tex : tex->get_rect();

	g_rect rc = g_rect(0,0,m_window->m_width,m_window->m_height);

	if (!get_cliped_rect(rect,rc,x,y,m_rc_clip))
		return -1;

	s_triangle_render += 2;

	s_f2 f3[4];
	s_uv uv[4];

	f3[0].x = x;
	f3[0].y = y;
	uv[0].u = (float)rect.l/tex->m_tex_width;
	uv[0].v = (float)rect.t/tex->m_tex_height;
	f3[3].x = x + rect.width();
	f3[3].y = y + rect.height();
	uv[3].u = (float)rect.r/tex->m_tex_width;
	uv[3].v = (float)rect.b/tex->m_tex_height;

	f3[1].x = f3[0].x;
	f3[1].y = f3[3].y;
	uv[1].u = uv[0].u;
	uv[1].v = uv[3].v;
	f3[2].x = f3[3].x;
	f3[2].y = f3[0].y;
	uv[2].u = uv[3].u;
	uv[2].v = uv[0].v;

	glBindTexture(GL_TEXTURE_2D, tex->m_textureId);

	unsigned char r,g,b;
	G_GET_RGB(color,r,g,b);
	glColor4ub(r,g,b,alpha);
	//glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
	glBegin(GL_TRIANGLE_STRIP);	
	for(int i=0;i<4;i++)
	{
		glTexCoord2f(uv[i].u,uv[i].v); 
		glVertex2i(f3[i].x,f3[i].y);	
	}
	glEnd();

	glFlush();

	CHECK_GL_ERROR_DEBUG();

	return 0;
}


int window_render_gl::draw_box_cell(const st_cell& cell,int w,int h)
{
	s_box_render ++;
	return draw_box(cell.x,cell.y,cell.color,cell.alpha,w,h);
}

int window_render_gl::draw_box(int x,int y,int color,int alpha,int w,int h)
{
	if (!get_cliped_box(x,y,w,h,m_window->m_width,m_window->m_height))
		return -1;

	s_triangle_render += 2;

	s_f2 f3[4];

	f3[0].x = x;
	f3[0].y = y;
	f3[3].x = x + w;
	f3[3].y = y + h;

	f3[1].x = f3[0].x;
	f3[1].y = f3[3].y;
	f3[2].x = f3[3].x;
	f3[2].y = f3[0].y;

	unsigned char r,g,b;
	G_GET_RGB(color,r,g,b);

	glColor4ub(r,g,b,alpha);
	//glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBegin(GL_TRIANGLE_STRIP);	
	for(int i=0;i<4;i++)
	{
		glVertex2i(f3[i].x,f3[i].y);	
	}
	glEnd();

	glFlush();

	CHECK_GL_ERROR_DEBUG();

	return 0;
}

int window_render_gl::draw_text_cell(const st_cell& cell,texture* tex,const g_rect* rc)
{
	s_text_render++;
	return _draw_texture_cell(cell,tex,rc);
}

int window_render_gl::draw_image_cell(const st_cell& cell,image* img,const char* file,const g_rect* rc)
{
	s_image_render++;
	texture* p = get_graph()->find_texture(file);
	if (p)
		return draw_text_cell(cell,p,rc);
	texture_gl* gl = new texture_gl;
	if (!gl->create_texture(img,0))
		return -1;
	get_graph()->maped_texture(file,gl);
	_draw_texture_cell(cell,gl,rc);
	return 0;
}