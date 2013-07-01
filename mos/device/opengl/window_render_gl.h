#ifndef __DEVICE_WINDOW_RENDER_GL_H_
#define __DEVICE_WINDOW_RENDER_GL_H_

#include "../window_render.h"
#include <windows.h>
class director;

class glShaderManager;
class glShader;

enum CShaderType 
{
	shader_null,
	shader_256,
};

class window_render_gl : public window_render
{
public:
	window_render_gl(window* w);
	~window_render_gl();

	bool create_render(int width,int height);
	void create_shaders();

	texture* create_texture() ;
	void on_destroy();

	void render_start();
	void render_end();

	int _draw_texture_cell(const st_cell& cell,texture* tex,const g_rect* rc);
	int draw_texture_cell(const st_cell& cell,texture* tex,const g_rect* rc);
	int draw_texture(int x,int y, float room,int color,int alpha,const char* shader,float shader_param,texture* _tex,const g_rect* rc);
	int draw_box_cell(const st_cell& cell,int w,int h);
	int draw_box(int x,int y,int color,int alpha,int w,int h);

	int draw_text_cell(const st_cell& cell,texture* tex,const g_rect* rc);
	int draw_image_cell(const st_cell& cell,image* img,const char* file,const g_rect* rc);

	HDC  m_hDC;
	HGLRC m_hRC;
	director* m_director;

	unsigned int m_program_palette;

	glShaderManager* m_shader_manager;

	//some default shader.
	glShader*	m_shader_palette;
};

#endif
