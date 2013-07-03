#include "map.h"
#include "graph.h"

map* get_map()
{
	static map s_map;
	return &s_map;
}

void map::mask_drawing_image(const st_cell* cell)
{

}

void map::draw_map_image(const st_cell& cell, const char* map_file,int frame)
{
	get_graph()->draw_image(cell,map_file,frame);
}

bool map::get_map_size(const char* file,int frame,g_size& sz)
{
	return get_graph()->get_image_size(file,frame,sz);
}