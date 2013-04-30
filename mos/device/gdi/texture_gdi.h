#ifndef __GRAPH_TEXTURE_GDI_H_
#define __GRAPH_TEXTURE_GDI_H_

//wrap opengl texture not directx.
class image;
#include "graph/texture.h"

class texture_gdi : public texture
{
public:
	texture_gdi(){m_image = 0;m_image_src = 0;m_rc_image = 0;}
	~texture_gdi();

	image* m_image;
	bool create_texture_dynamic(int width,int height,CCTexture2DPixelFormat format) ;
	int draw_image_ontexture(int x,int y,const image* img) ;

	bool create_texture(image* img,const g_rect* rc,CCTexture2DPixelFormat format) ;
	image* m_image_src;
	const g_rect* m_rc_image;
};

#endif