#include "graph.h"
#include "texture.h"
#include "graph.h"
#include "device/window_render.h"
#include "mos.h"

int texture::s_texture_id = 0;
int texture::s_texture_num = 0;

texture::texture()
{
	m_id = s_texture_id++;
	s_texture_num++;
}

texture::~texture()
{
	s_texture_num--;
}

