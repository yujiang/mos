#include "graph.h"
#include "texture.h"
#include "graph.h"
#include "device/window_render.h"
#include "mos.h"

int texture::draw_cell(const st_cell& cell,const g_rect* rc )
{
	return get_render()->draw_texture_cell(cell,this,rc);
}

