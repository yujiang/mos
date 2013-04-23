#ifndef __GRAPH_TEXTURE_GDI_H_
#define __GRAPH_TEXTURE_GDI_H_

//wrap opengl texture not directx.
class image;
#include "graph/texture.h"

class texture_gdi : public texture
{
public:
	texture_gdi(){m_image = 0;}
	~texture_gdi();
	image* m_image;

	bool create_texture(const image* img,const g_rect* rc,CCTexture2DPixelFormat format) ;
	bool create_texture_dynamic(int width,int height,CCTexture2DPixelFormat format) ;
	int draw_image_ontexture(int x,int y,const image* img) ;

};

#endif