#ifndef MAP_H
#define MAP_H

#include "rect.h"

class st_cell;

class map
{
public:
	void mask_draw_image(const st_cell* cell);
	void draw_map_image(const st_cell& cell, const char* map_file,int frame);

	bool get_map_size(const char* file,int frame,g_size& sz);
};

map* get_map();

#endif