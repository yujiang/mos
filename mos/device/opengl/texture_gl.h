#ifndef __GRAPH_TEXTURE_GL_H_
#define __GRAPH_TEXTURE_GL_H_

//wrap opengl texture not directx.
class image;
#include "graph/texture.h"
#include "OGLES/GL/glew.h"
#include "graph/rect.h"


class texture_gl : public texture
{
public:
	texture_gl();
	~texture_gl();

	GLuint m_textureId;

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

	bool create_texture(image* img,const g_rect* rc,CCTexture2DPixelFormat format = kCCTexture2DPixelFormat_Default) ;
	bool create_texture_dynamic(int width,int height,CCTexture2DPixelFormat format) ;
	int draw_image_ontexture(int x,int y,const image* img) ;

	/** Initializes with a texture2d with data */
	bool init_data(const void* data, int rowLength,CCTexture2DPixelFormat pixelFormat, int pixelsWide, int pixelsHigh, int width, int height);
	bool update_data(const void* data, int rowLength,int offx,int offy,int width, int height);

};

#endif