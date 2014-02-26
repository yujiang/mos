#include "window_render_gl.h"
#include "../window.h"
#include "../file.h"
#include "graph/texture.h"
#include "graph/image.h"
#include "graph/graph.h"
#include <windows.h>
#include "GL/glew.h"
#include "director.h"
#include "texture_gl.h"
#include "graph/cell.h"
#include "graph/color.h"
#include "gl_macro.h"
#include "glsl.h"
#include <assert.h>

//using namespace cwc;
window_render_gl::window_render_gl(window* w):window_render(w)
{
	m_director = new director(this);
	m_shader_manager = 0;
	m_render_start = false;
	m_in_flush = false;
	m_hRCThread = 0;
	m_hDC = 0;
	m_hRC = 0;
}

window_render_gl::~window_render_gl()
{
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
	if (is_thread)
	{
		m_hRCThread = wglCreateContext(m_hDC);
		BOOL rt = wglShareLists(m_hRC, m_hRCThread);
	}

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

	create_shaders();

	return true;
}

void window_render_gl::create_shaders() 
{
//-----------------------------------------------------------------------------
// Simple GLSL Vertex Shader:
	const char* palette_vertex_prog = "void main()\
									  {\
									  gl_TexCoord[0] = gl_MultiTexCoord0;\
									  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\
									  }";

	const char* palette_fragment_prog = "uniform sampler2D texture;\
										uniform sampler2D palette;\
										void main()\
										{\
										gl_FragColor = texture2D(palette, vec2(texture2D(texture, gl_TexCoord[0].st).a, 0));\
										}";

	m_shader_manager = new glShaderManager();
	m_shader_palette = m_shader_manager->loadfromMemoryName("shader_palette",palette_vertex_prog,palette_fragment_prog);
	m_shader_manager->SetPath(get_resourcefile("shaders"));
}

texture* window_render_gl::create_texture() 
{
	return new texture_gl;
}

void window_render_gl::on_destroy()
{
	delete m_director;
	m_director = 0;
	delete m_shader_manager;
	m_shader_manager = 0;

	if (m_hDC != NULL && m_hRC != NULL)
	{
		// deselect rendering context and delete it
		wglMakeCurrent(m_hDC, NULL);
		wglDeleteContext(m_hRC);
		wglDeleteContext(m_hRCThread);
		m_hDC = NULL;
		m_hRC = NULL;
		m_hRCThread = NULL;
	}
}

void window_render_gl::render_start()
{
	if (is_thread || is_batch)
		;
	else
		glClear(GL_COLOR_BUFFER_BIT);
	m_render_start = true;
}

void window_render_gl::flush_draws()
{
	m_in_flush = true;
	std::vector<st_draw*> batch;
	int max_area = m_window->m_height*m_window->m_width/4;
	//printf("flush_draws %d\n",m_draws.size());
	for (auto it = m_draws.begin(); it != m_draws.end(); ++it)
	{
		st_draw& st = *it;
		if (st.drawed)
			continue;
		if (st.op)
		{
			if (st.op == op_alpha_blend)
			{
				m_director->_set_alpha_blending(st.value);
			}
			continue;
		}

		if (!st._tex)
		{
			_draw_box(st.x,st.y,st.color,st.alpha,st.w,st.h); //box 不多，就不合并了。
			continue;
		}
		if (st.shader)
		{
			_draw_texture(st.x,st.y,st.room,st.color,st.alpha,st.shader,st.shader_param,st._tex,st.rc_tex ? &st.rc : 0, st.rect);
			continue;
		}

		batch.clear();
		batch.push_back(&st);
		//st_draw& p = *(batch.back());
		//assert(&p == &st);
		g_rect rc_dirty;

		auto it2 = it;
		++it2;
		for (; it2 != m_draws.end(); ++it2)
		{
			st_draw& st2 = *it2;
			if (st2.op)
				break;
			if (st2.drawed)
				continue;
			if (st2.shader || st2._tex != st._tex || rect_intersection(st2.rc_screen,rc_dirty))
			{
				if (!st2._tex) //box
					rc_dirty = rc_dirty + g_rect(st2.x,st2.y,st2.x+st2.w,st2.y+st2.h);
				else
					rc_dirty = rc_dirty + st2.rc_screen;
				if (rc_dirty.get_area() >= max_area)
					break;
			}
			else
			{
				batch.push_back(&st2);
			}
		}
		draw_batch(batch);
	}
	m_draws.clear();
	m_in_flush = false;
}

void window_render_gl::render_end()
{
	if (is_thread)
	{
		HGLRC rc = wglGetCurrentContext();
		if (rc == NULL)
		{
			wglMakeCurrent(m_hDC, m_hRCThread);
			m_director->create_director();
		}
	}

	if (is_thread || is_batch)
		glClear(GL_COLOR_BUFFER_BIT);	
	flush_draws();
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
struct s_color
{
	unsigned char r, g, b, a;
};

int window_render_gl::draw_texture_cell(const st_cell& cell,texture* _tex,const g_rect* rc)
{
	s_texture_render++;
	return _draw_texture_cell(cell,_tex,rc);
}

int window_render_gl::_draw_texture_cell(const st_cell& cell,texture* _tex,const g_rect* rc)
{
	return draw_texture(cell.x,cell.y,cell.room,cell.color,cell.alpha,cell.shader,cell.shader_param,_tex,rc);
}

bool get_cliped_rect2(g_rect& rect,const g_rect& clip,int& offx,int& offy)
{
	if (offx < clip.l)
	{
		rect.l -= offx - clip.l;
		if (rect.r <= rect.l)
			return false;
		offx = clip.l;
	}
	else if (offx >= clip.r)
		return false;
	if (offx + rect.width() > clip.r)
		rect.r -= offx + rect.width() - clip.r;

	if (offy < clip.t)
	{
		rect.t -= offy - clip.t;
		if (rect.b <= rect.t)
			return false;
		offy = clip.t;
	}
	else if (offy >= clip.b)
		return false;
	if (offy + rect.height() > clip.b)
		rect.b -= offy + rect.height() - clip.b;

	if (rect.is_empty())
		return false;

	return true;
}


int window_render_gl::draw_texture(int x,int y,float room,int color,int alpha,const char* shader,float shader_param, texture* _tex,const g_rect* rc_tex)
{
	assert(!m_in_flush);
	texture_gl* tex = (texture_gl*)_tex;
	g_rect rect0 = rc_tex ? *rc_tex : tex->get_rect();

	float u0 = (float)rect0.l/tex->m_tex_width;
	float v0 = (float)rect0.t/tex->m_tex_height;
	g_rect rect = rect0;
	rect.r += rect.width() * (room - 1);
	rect.b += rect.height() * (room - 1);


	//这个地方不对，跟window clip了接着room，错了。
	
	//因为shader的缘故，这里可以去了。
	g_rect rc_window = g_rect(0,0,m_window->m_width,m_window->m_height);
	//if (m_rc_clip && !get_cliped_rect2(rect,*m_rc_clip,x,y))
	if (!get_cliped_rect(rect,rc_window,x,y,m_rc_clip))
		return -1;

	st_draw st = {0};
	st.x = x;
	st.y = y;
	st.room = room;
	st.color = color;
	st.alpha = alpha;
	st.shader = shader;
	st.shader_param = shader_param;
	st._tex = _tex;
	st.rc_tex = rc_tex;
	if (rc_tex)
		st.rc = *rc_tex;
	st.rect = rect;
	st.rc_screen = g_rect(x,y,x+rect.width(),y+rect.height());
	if (is_batch)
		m_draws.push_back(st);
	else
		_draw_texture(st.x,st.y,st.room,st.color,st.alpha,st.shader,st.shader_param,st._tex,st.rc_tex,st.rect);
	return 0;
}

//room做成shader算了。
int window_render_gl::_draw_texture(int x,int y,float room,int color,int alpha,const char* shader,float shader_param, texture* _tex,const g_rect* rc_tex, const g_rect& rect)
{
	texture_gl* tex = (texture_gl*)_tex;
	g_rect rect0 = rc_tex ? *rc_tex : tex->get_rect();

	float u0 = (float)rect0.l/tex->m_tex_width;
	float v0 = (float)rect0.t/tex->m_tex_height;

	s_f2 f3[4];
	s_uv uv[4];

	f3[0].x = x;
	f3[0].y = y;
	uv[0].u = u0 + (float)(rect.l-rect0.l)/tex->m_tex_width/room;
	uv[0].v = v0 + (float)(rect.t-rect0.t)/tex->m_tex_height/room;
	f3[3].x = x + rect.width() ;
	f3[3].y = y + rect.height() ;
	uv[3].u = u0 + (float)(rect.r-rect0.l)/tex->m_tex_width/room;
	uv[3].v = v0 + (float)(rect.b-rect0.t)/tex->m_tex_height/room;

	f3[1].x = f3[0].x;
	f3[1].y = f3[3].y;
	uv[1].u = uv[0].u;
	uv[1].v = uv[3].v;
	f3[2].x = f3[3].x;
	f3[2].y = f3[0].y;
	uv[2].u = uv[3].u;
	uv[2].v = uv[0].v;


	glShader* cur_shader = NULL;
	if (shader)
		cur_shader = m_shader_manager->getShader(shader,tex->use_palette());
	if (cur_shader == NULL && tex->use_palette())
		cur_shader = m_shader_palette;

	if (cur_shader)
	{
		cur_shader->begin();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex->m_textureId);

		if (tex->use_palette())
		{
			int texid = cur_shader->GetUniformLocation("texture");
			glUniform1i(texid,0);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, tex->m_textureId_pal);
			int palette = cur_shader->GetUniformLocation("palette");
			glUniform1i(palette,1);
		}
		else
		{
			int param = cur_shader->GetUniformLocation("param");
			//shader_param
			glUniform1f(param,shader_param);
		}
	}
	else
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex->m_textureId);
	}

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

	if (cur_shader)
	{
		cur_shader->end();
	}

	//glFlush();
	s_triangle_render ++;

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
	assert(!m_in_flush);
	if (!get_cliped_box(x,y,w,h,m_window->m_width,m_window->m_height))
		return -1;

	st_draw st = {0};
	st.x = x;
	st.y = y;
	st.color = color;
	st.alpha = alpha;
	st.w = w;
	st.h = h;
	st._tex = 0;
	st.rc_tex = 0;
	if (is_batch)
		m_draws.push_back(st);
	else
		_draw_box(x,y,color,alpha,w,h);
	return 0;
}

int window_render_gl::_draw_box(int x,int y,int color,int alpha,int w,int h)
{
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

	//glDisable(GL_TEXTURE_2D);

	glColor4ub(r,g,b,alpha);
	//glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
	//否则画不出box
	glBindTexture(GL_TEXTURE_2D, 0);

	glBegin(GL_TRIANGLE_STRIP);	
	for(int i=0;i<4;i++)
	{
		glVertex2i(f3[i].x,f3[i].y);	
	}
	glEnd();

	//glFlush();
	s_triangle_render ++;

	CHECK_GL_ERROR_DEBUG();

	return 0;
}

int window_render_gl::draw_text_cell(const st_cell& cell,texture* tex,const g_rect* rc)
{
	s_text_render++;
	return _draw_texture_cell(cell,tex,rc);
}

texture_mul* find_free_texture_mul(int w,int h)
{
	std::vector <texture_mul*>& v = get_graph()->texture_muls;
	for (auto it = v.begin(); it != v.end(); ++it)
	{
		if ((*it)->find_free(w,h))
			return *it;
	}
	return 0;
}

texture_sub* window_render_gl::create_texturesub(image* img,const g_rect* rc)
{
	g_rect r = rc ? *rc : img->get_rect();
	assert(r.width() <= 512 && r.height() <= 512);
	texture_mul* mul = find_free_texture_mul(r.width(),r.height());
	if (!mul)
	{
		mul = new texture_mul;
		mul->m_texture = new texture_gl;
		mul->m_texture->create_texture_dynamic(512,512,kCCTexture2DPixelFormat_RGBA8888);
		get_graph()->texture_muls.push_back(mul);
	}
	return mul->add_image_ontexture(img,r);
}

int window_render_gl::draw_image_cell(const st_cell& cell,image* img,const char* file,const g_rect* rc)
{
	s_image_render++;

	if (is_mul)
	{
		enum_objtype obj = get_objtype_byfile(file);
		if (obj == obj_char) //
		{
			texture_sub* gl = (texture_sub*)get_graph()->find_texturesub(file);
			if (!gl)
			{
				gl = create_texturesub(img,rc);
				if (!gl)
					return -1;
			}
			get_graph()->maped_texturesub(file,gl);
			return _draw_texture_cell(cell,gl->m_tex->m_texture,&gl->m_rc_tex);
		}
	}

	texture_gl* gl = (texture_gl*)get_graph()->find_texture(file);
	if (!gl)
	{
		gl = new texture_gl;
		if (!gl->create_texture_gl(img))
		{
			delete gl;
			return -1;
		}
		get_graph()->maped_texture(file,gl);
	}
	return _draw_texture_cell(cell,gl,rc);
}

int window_render_gl::draw_batch(std::vector<st_draw*>& batch)
{
	if (batch.empty())
		return 0;

	st_draw& st = *(batch.front());
	texture_gl* tex = (texture_gl*)st._tex;

	int size = batch.size() * 6;
	std::vector <s_f2> af3(size);
	std::vector <s_uv> auv(size);
	std::vector <s_color> acolor(size);

	int i = 0;
	s_f2* f3 = &af3[0];
	s_uv* uv = &auv[0];
	s_color* color = &acolor[0];

	for (auto it = batch.begin(); it != batch.end(); ++ it,f3+=6,uv+=6)
	{
		st_draw& st = *(*it);
		st.drawed = true;

		g_rect rect0 = st.rc_tex ? st.rc : tex->get_rect();
		int x = st.x;
		int y = st.y;
		const g_rect& rect = st.rect;
		float room = st.room;

		float u0 = (float)rect0.l/tex->m_tex_width;
		float v0 = (float)rect0.t/tex->m_tex_height;

		//fill_texture(st,pf3,puv,pcolor);
		f3[0].x = x;
		f3[0].y = y;
		uv[0].u = u0 + (float)(rect.l-rect0.l)/tex->m_tex_width/room;
		uv[0].v = v0 + (float)(rect.t-rect0.t)/tex->m_tex_height/room;
		f3[5].x = x + rect.width() ;
		f3[5].y = y + rect.height() ;
		uv[5].u = u0 + (float)(rect.r-rect0.l)/tex->m_tex_width/room;
		uv[5].v = v0 + (float)(rect.b-rect0.t)/tex->m_tex_height/room;
		
		f3[1].x = f3[0].x;
		f3[1].y = f3[5].y;
		uv[1].u = uv[0].u;
		uv[1].v = uv[5].v;
		f3[2].x = f3[5].x;
		f3[2].y = f3[0].y;
		uv[2].u = uv[5].u;
		uv[2].v = uv[0].v;

		f3[3] = f3[1];
		f3[4] = f3[2];

		uv[3] = uv[1];
		uv[4] = uv[2];

		s_color c;
		G_GET_RGB(st.color,c.r,c.g,c.b);
		c.a = st.alpha;
		for (i=0; i<6; i++,color++)
		{
			*color = c;
		}
	}

	glShader* cur_shader = NULL;
	if (cur_shader == NULL && tex->use_palette())
		cur_shader = m_shader_palette;

	if (cur_shader)
	{
		cur_shader->begin();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex->m_textureId);

		if (tex->use_palette())
		{
			int texid = cur_shader->GetUniformLocation("texture");
			glUniform1i(texid,0);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, tex->m_textureId_pal);
			int palette = cur_shader->GetUniformLocation("palette");
			glUniform1i(palette,1);
		}

		//int param = cur_shader->GetUniformLocation("param");
		//shader_param
		//glUniform1f(param,shader_param);
	}
	else
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex->m_textureId);
	}

	glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );
    glVertexPointer( 2, GL_INT, sizeof(s_f2), &af3[0]);
    glTexCoordPointer( 2, GL_FLOAT, sizeof(s_uv), &auv[0]);
    glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof(s_color), &acolor[0]);
    glDrawArrays( GL_TRIANGLES, 0, size );

	if (cur_shader)
	{
		cur_shader->end();
	}

	//glFlush();
	s_triangle_render ++;

	CHECK_GL_ERROR_DEBUG();

	return 0;
}

void window_render_gl::batch_set_alpha_blending(bool bOn)
{
	assert(!m_in_flush);
	st_draw st = {0};
	st.op = op_alpha_blend;
	st.value = bOn;
	m_draws.push_back(st);
}