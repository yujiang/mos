#include "texture_gl.h"
#include "device/window_render.h"
#include "mos.h"
#include "graph/image.h"
#include <assert.h>

texture_gl::texture_gl()
{
	m_textureId = 0;
}

texture_gl::~texture_gl()
{
}

void texture_gl::set_alpha(bool alpha) 
{
}

bool texture_gl::create_texture(image* img,const g_rect* rc) 
{
	return true;
}

bool texture_gl::create_texture_dynamic(int width,int height,int m_bits_component) 
{
	return true;
}

int texture_gl::draw_image(const st_cell& cell,const image* img,const g_rect* rc_img,const g_rect* rc_clip) 
{
	return 0;
}
