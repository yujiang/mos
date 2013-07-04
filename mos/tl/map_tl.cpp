#include "map_tl.h"

#include "../graph/map.h"
#include "../graph/image.h"
#include "../graph/graph.h"
#include "../graph/cell.h"
#include "../device/window_render.h"
#include "../device/file.h"
#include "../mos.h"
#include "mapdata.h"

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
	mapdata m_data;
protected:
	void draw_block(const st_cell& cell,map_block* block);
};

void map_tl::mask_drawing_image(const st_cell* cell)
{

}

void map_tl::draw_map_image(const st_cell& cell, const char* map_file,int frame)
{
	//should draw 9 map.
	//first locate cell.pos to x,y
	int blockw = BLOCKW * cell.room;
	int blockh = BLOCKH * cell.room;

	int w = -cell.x / blockw;
	int h = -cell.y / blockh;

	st_cell c;
	memcpy(&c,&cell,sizeof(c));

	//const int range = 3; 
	const int range = 4; //有缩放，改为4
	for (int x = w; x < w+range; x++)
	for (int y = h; y < h+range; y++)
	{
		map_block* bl = m_data.get_block(x,y);
		if (bl)
		{
			c.x = x * blockw + cell.x;
			c.y = y * blockh + cell.y;
			//bl->draw_block(offx,offy,cell);
			//get_graph()->draw_image(c,map_file,m_data.pos_2_num(x,y));
			draw_block(c,bl);
		}
	}
}

void map_tl::draw_block(const st_cell& cell,map_block* block)
{
	if (!block->m_Image)
	{
		block->load_whole(&m_data);
	}
	block->m_Image->mark_use_image(get_time_now());
	char buf[128];
	sprintf(buf,"%s_%02d%02d",m_data.m_map.c_str(),block->y,block->x);
	get_render()->draw_image_cell(cell,block->m_Image,buf,NULL);
}


bool map_tl::load_map(const char* file,int frame,g_size& sz)
{
	if (!m_data.load_map(get_resourcefile(file)))
		return false;
	sz.w = m_data.get_width();
	sz.h = m_data.get_height();
	return true;
}

map_source* get_map_source_tl()
{
	return new map_tl;
}

