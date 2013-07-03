#include "map_tl.h"

#include "../graph/map.h"

class map_tl : public map_source
{
public:
	void mask_drawing_image(const st_cell* cell);
	void draw_map_image(const st_cell& cell, const char* map_file,int frame);
	bool load_map(const char* file,int frame,g_size& sz);
	void draw_map_begin() {};
	void draw_map_end() {};
	void destory()
	{
		delete this;
	}
};

void map_tl::mask_drawing_image(const st_cell* cell)
{

}
void map_tl::draw_map_image(const st_cell& cell, const char* map_file,int frame)
{

}
bool map_tl::load_map(const char* file,int frame,g_size& sz)
{
	return false;
}

map_source* get_map_source_tl()
{
	return new map_tl;
}