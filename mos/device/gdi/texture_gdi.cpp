#include "texture_gdi.h"
#include "device/window_render.h"
#include "mos.h"
#include "graph/image.h"
#include <assert.h>

texture_gdi::~texture_gdi()
{
	delete m_image;
	m_image = NULL;

	if (m_image_src)
	{
		m_image_src->image_release();
		m_image_src= 0;
	}
}

bool texture_gdi::create_texture(image* img,const g_rect* rc,CCTexture2DPixelFormat format) 
{
	assert(m_image == 0);
	//m_image = new image;
	//m_image->create_image_image(img,rc);
	m_image_src = img;
	m_image_src->image_add_ref();
	m_rc_image = rc;

	return true;
}

bool texture_gdi::create_texture_dynamic(int width,int height,CCTexture2DPixelFormat format) 
{
	assert(m_image == 0);
	m_image = new image;
	int m_bits_component = bitsPerPixelForFormat(format)/8;
	bool rt = m_image->create_image_dynamic(width,height,m_bits_component);
	//m_image->m_alpha = hasAlpha(format);
	return true;
}

int texture_gdi::draw_image_ontexture(int x,int y,const image* img) 
{
	return m_image->draw_image(x,y,-1,255,img,NULL,NULL);
}

