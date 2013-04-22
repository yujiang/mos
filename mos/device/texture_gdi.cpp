#include "texture_gdi.h"
#include "device/window_render.h"
#include "mos.h"
#include "graph/image.h"
#include <assert.h>

texture_gdi::~texture_gdi()
{
	delete m_image;
	m_image = NULL;
}

void texture_gdi::set_alpha(bool alpha) 
{
	m_image->m_alpha = alpha;
}

bool texture_gdi::create_texture(image* img,const g_rect* rc) 
{
	assert(m_image == 0);
	m_image = new image;
	m_image->create_image_image(img,rc);
	return true;
}

bool texture_gdi::create_texture_dynamic(int width,int height,int m_bits_component) 
{
	assert(m_image == 0);
	m_image = new image;
	return m_image->create_image_dynamic(width,height,m_bits_component);
}

int texture_gdi::draw_image(const st_cell& cell,const image* img,const g_rect* rc_img,const g_rect* rc_clip) 
{
	return m_image->draw_image(cell,img,rc_img,rc_clip);
}
