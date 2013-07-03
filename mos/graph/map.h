#ifndef MAP_H
#define MAP_H

#include "rect.h"

class st_cell;

class map
{
public:
	//准备绘制该image了，更新遮罩。
	void mask_drawing_image(const st_cell* cell);

	void draw_map_image(const st_cell& cell, const char* map_file,int frame);

	bool get_map_size(const char* file,int frame,g_size& sz);
};

map* get_map();

#endif