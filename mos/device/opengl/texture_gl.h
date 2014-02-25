#ifndef __GRAPH_TEXTURE_GL_H_
#define __GRAPH_TEXTURE_GL_H_

//wrap opengl texture not directx.
class image;
#include "graph/texture.h"
#include "GL/glew.h"
#include "core/rect.h"
#include <vector>

enum CShaderType;

class texture_gl : public texture
{
public:
	texture_gl();
	~texture_gl();

	GLuint m_textureId;

	GLuint m_textureId_pal;

	//图的大小
	CCTexture2DPixelFormat m_format;
	int m_width,m_height;
	int get_width() const{
		return m_width;
	}
	int get_height() const{
		return m_height;
	}
	g_rect get_rect() const{
		return g_rect(0,0,m_width,m_height);
	}

	//系统贴图的大小，不必是2的幂
	int m_tex_width,m_tex_height;
	int get_tex_width() const{
		return m_tex_width;
	}
	int get_tex_height() const{
		return m_tex_height;
	}

	bool m_use_palette;
	bool use_palette() const{
		return m_use_palette;
	}

	bool create_texture_gl(image* img) ;

	bool create_texture_dynamic(int width,int height,CCTexture2DPixelFormat format) ;
	int draw_image_ontexture(int x,int y,const image* img,const g_rect* rc) ;

	/** Initializes with a texture2d with data */
	bool init_data(const void* data, int rowLength,CCTexture2DPixelFormat pixelFormat, int pixelsWide, int pixelsHigh, int width, int height);
	bool update_data(const void* data, int rowLength,int offx,int offy,int width, int height);

};

struct st_line{
	g_rect rc;
	std::vector<texture_sub*> subs;

};

class texture_mul : public texture_gl
{
public:
	counter<texture_mul> m_counter;

	texture_sub* create_line_sub(st_line& line,int w);
	texture_sub* create_sub(const image* img,const g_rect& rc) ;

	texture_sub* add_image_ontexture(const image* img,const g_rect& rc) ;
	//记录一系列的矩形。
	bool find_free(int w,int h) const;
	std::vector<st_line> m_lines;
	int get_last_height() const;
	int get_free_height() const{
		return get_tex_height() - get_last_height();
	}
};


#endif