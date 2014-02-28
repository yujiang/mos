#ifndef __DEVICE_WINDOW_RENDER_GL_H_
#define __DEVICE_WINDOW_RENDER_GL_H_

#include "../window_render.h"
#include <windows.h>
#include <vector>

class director;
class texture_sub;

class glShaderManager;
class glShader;

enum CShaderType 
{
	shader_null,
	shader_256,
};

enum draw_op{
	op_alpha_blend = 1,
};

struct st_draw{
	int x;
	int y;
	float room;
	int color;
	int alpha;

	//texture
	const char* shader;
	float shader_param;
	texture* _tex;
	
	const g_rect* rc_tex;	
	g_rect rc;

	g_rect rect;
	g_rect rc_screen;

	//box
	int w;
	int h;

	//op
	int op;
	int value;

	bool drawed;
};

enum texture_op{
	op_create_texture_gl,
	op_create_texture_dynamic,
	op_draw_image_ontexture,
};

class texture_gl;
enum CCTexture2DPixelFormat;

struct st_texture
{
	texture_op op;
	texture_gl* tex;
	const image* img;
	int width,height;
	int x,y;
	CCTexture2DPixelFormat format;
	const g_rect* rc;
	g_rect rect;
};

class window_render_gl : public window_render
{
public:
	window_render_gl(window* w);
	~window_render_gl();

	int m_width,m_height;
	bool create_render(int width,int height);
	bool _create_render(int width,int height);
	void create_shaders();

	texture* create_texture() ;
	void on_destroy();
	void _on_destroy();

	void render_start();
	void render_end();
	void render_func();
	void flush_draws();


	int _draw_texture_cell(const st_cell& cell,texture* tex,const g_rect* rc);
	int draw_texture_cell(const st_cell& cell,texture* tex,const g_rect* rc);
	int _draw_texture(int x,int y, float room,int color,int alpha,const char* shader,float shader_param,texture* _tex,const g_rect* rc, const g_rect& rect);
	int draw_texture(int x,int y, float room,int color,int alpha,const char* shader,float shader_param,texture* _tex,const g_rect* rc);
	int draw_box_cell(const st_cell& cell,int w,int h);
	int _draw_box(int x,int y,int color,int alpha,int w,int h);
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

	bool m_in_flush;
	std::vector<st_draw> m_draws;
	int draw_batch(std::vector<st_draw*>& batch);
	void batch_set_alpha_blending(bool bOn);

	texture_sub* create_texturesub(image* img,const g_rect* rc);

	//all texture op
	std::vector<st_texture> m_textures;
	void update_textures();
};

#endif
