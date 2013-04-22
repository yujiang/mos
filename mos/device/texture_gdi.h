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

	void set_alpha(bool alpha) ;
	bool create_texture(image* img,const g_rect* rc) ;
	bool create_texture_dynamic(int width,int height,int m_bits_component) ;
	int draw_image(const st_cell& cell,const image* img,const g_rect* rc_img,const g_rect* rc_clip) ;

};

#endif