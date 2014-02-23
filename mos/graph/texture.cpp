#include "graph.h"
#include "texture.h"
#include "graph.h"
#include "device/window_render.h"
#include "mos.h"


DECLARE_COUNTER(texture)

texture::texture()
{
	m_alloc_id = m_counter.s_alloc_num;
	m_name = 0;
}

texture::~texture()
{
}

