#include "graph.h"
#include "texture.h"
#include "graph.h"
#include "image.h"
#include "device/window.h"
#include "mos.h"

texture::~texture()
{
	delete m_image;
	m_image = NULL;
}

//先来个gdi的方法，先绘制到缓冲，在flip一下
bool texture::create_texture(image* img,const g_rect* rc)
{
	m_image = new image;
	m_image->create_image_image(img,rc);

	return true;
}

int texture::draw_cell(const st_cell& cell,const g_rect* rc )
{
	return get_window()->draw_texture(cell,this,rc);
}

