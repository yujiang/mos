#ifndef __GRAPH_TEXTURE_GL_H_
#define __GRAPH_TEXTURE_GL_H_

//wrap opengl texture not directx.
class image;
#include "graph/texture.h"
#include "OGLES/GL/glew.h"

class texture_gl : public texture
{
public:
	texture_gl();
	~texture_gl();

	GLuint m_textureId;

	void set_alpha(bool alpha) ;
	bool create_texture(image* img,const g_rect* rc) ;
	bool create_texture_dynamic(int width,int height,int m_bits_component) ;
	int draw_image(const st_cell& cell,const image* img,const g_rect* rc_img,const g_rect* rc_clip) ;
};

#endif