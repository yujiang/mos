#include "map_tl.h"

#include "../map/map.h"
#include "../map/mapobs.h"
#include "../graph/image.h"
#include "../graph/graph.h"
#include "../graph/cell.h"
#include "../device/opengl/window_render_gl.h"
#include "../device/opengl/texture_gl.h"
#include "../device/opengl/director.h"
#include "../device/window.h"
#include "../device/file.h"
#include "../mos.h"
#include "mapdata.h"
//#include "mapobs.h"
#include <unordered_set>

class map_tl : public map_source
{
public:
	void mask_drawing_image(const st_cell& cell);
	void draw_map_image(const st_cell& cell, const char* map_file,int frame);
	bool load_map(const char* file,int frame,g_size& sz);
	bool load_obs(const char* file);
	void draw_map_begin() ;
	void draw_map_end() ;
	void destroy()
	{
		delete this;
	}
	mapdata m_data;
	//mapobs m_obs;
	std::vector<map_block*> m_current_blocks;
protected:
	void get_current_block(const st_cell& cell);
	void draw_block_image(const st_cell& cell,map_block* block);
	
	void draw_block_mask(const st_cell& cell,map_block* block);
	void draw_block_masks();
};

void map_tl::draw_map_begin() 
{
}

void map_tl::mask_drawing_image(const st_cell& cell)
{
	//我2了，哪有这么便宜的事情。
	//必须在此处立即绘制mask，而不能在draw_map_end()一次绘制。
	//当然贴图的创建为带通道的没有问题，还是省事。

	//int room = cell.room;
	const st_cell& cell_map = *get_map()->m_in_map;

	st_cell cell_block;
	memcpy(&cell_block,&cell_map,sizeof(cell_block));

	int blockw = BLOCKW * cell_map.room;
	int blockh = BLOCKH * cell_map.room;

	float room = cell.room / cell_map.room;
	g_rect rc_draw;
	int offx = (cell.x - cell_map.x)/cell_map.room;
	int offy = (cell.y - cell_map.y)/cell_map.room;
	rc_draw.set_xywh(
		offx - cell.cx * room,
		offy - cell.cy * room, 
		cell.w*room, 
		cell.h*room);

	for(auto it=m_current_blocks.begin(); it != m_current_blocks.end(); ++it)
	{
		map_block* bl = *it;

		g_rect rc_block;
		rc_block.set_xywh(bl->x*BLOCKW, bl->y*BLOCKH, BLOCKW, BLOCKH);
		if (!rect_intersection(rc_draw,rc_block))
			continue;

		int m_MaskNum = m_data.get_mask_num(bl->num);
		int len = 0;
		const int *maskindex = m_data.get_block_maskindex(bl->num, len);
		for (int i=0; i<m_MaskNum; i++)
		{
			const WMask& mask = bl->m_masks[i];
			//just
			//m.Create((int)mask->rect.x-BLOCKW*x, (int)mask->rect.y-BLOCKH*y,mask->rect.w,mask->rect.h, 0);
			g_rect rc;
			rc.set_xywh(mask.kx + bl->x*BLOCKW,mask.ky + bl->y*BLOCKH,mask.w,mask.h);
			//if (rc.have_point(point2(offx,offy)))
			g_rect r = rc - rc_draw;
			
			if (!r.is_empty())
			{
				if (mask.IsCover(offx-bl->x*BLOCKW,offy-bl->y*BLOCKH))
				//DrawMask with clip
				{
					r.offseted(point2(-bl->x*BLOCKW,-bl->y*BLOCKH));
					//r.set_xywh(mask.kx ,mask.ky ,mask.w,mask.h);
					cell_block.x = r.l * cell_map.room + bl->x * blockw + cell_map.x;
					cell_block.y = r.t * cell_map.room + bl->y * blockh + cell_map.y;
					get_render()->draw_image_cell(cell_block,bl->m_Image,bl->m_image_name.c_str(),&r);
				}
			}
		}
	}
}

void map_tl::draw_map_end() 
{
	//因为3d可以一次绘制所有的mask，就不必像2d那样绘制了。
	//draw_block_masks();
}

void map_tl::draw_block_masks() 
{
	const st_cell& cell = *get_map()->m_in_map;

	st_cell cell_block;
	memcpy(&cell_block,&cell,sizeof(cell_block));

	int blockw = BLOCKW * cell.room;
	int blockh = BLOCKH * cell.room;

	for(auto it=m_current_blocks.begin(); it != m_current_blocks.end(); ++it)
	{
		map_block* bl = *it;
		cell_block.x = bl->x * blockw + cell.x;
		cell_block.y = bl->y * blockh + cell.y;
		draw_block_mask(cell_block,bl);
	}
}

void map_tl::get_current_block(const st_cell& cell)
{
	m_current_blocks.clear();

	int blockw = BLOCKW * cell.room;
	int blockh = BLOCKH * cell.room;

	int w = -cell.x / blockw;
	int h = -cell.y / blockh;

	//const int range = 3; 
	int wrange = get_window()->get_width() / (BLOCKW * cell.room) + 2;
	int hrange = get_window()->get_height() / (BLOCKH * cell.room) + 2;
	for (int x = w; x < w+wrange; x++)
	for (int y = h; y < h+hrange; y++)
	{
		map_block* bl = m_data.get_block(x,y);
		if (bl)
			m_current_blocks.push_back(bl);
	}
}

void map_tl::draw_map_image(const st_cell& cell, const char* map_file,int frame)
{
	get_current_block(cell);

	st_cell m_cell_map;
	memcpy(&m_cell_map,&cell,sizeof(m_cell_map));

	//关闭alphablend
	window_render_gl* gl = get_render_gl();
	gl->m_director->set_alpha_blending(false);

	int blockw = BLOCKW * cell.room;
	int blockh = BLOCKH * cell.room;

	//const int range = 3; 
	for(auto it=m_current_blocks.begin(); it != m_current_blocks.end(); ++it)
	{
		map_block* bl = *it;

		m_cell_map.x = bl->x * blockw + cell.x;
		m_cell_map.y = bl->y * blockh + cell.y;
		//bl->draw_block(offx,offy,cell);
		//get_graph()->draw_image(c,map_file,m_data.pos_2_num(x,y));
		draw_block_image(m_cell_map,bl);
	}
	m_cell_map.x = cell.x;
	m_cell_map.y = cell.y;

	//打开alphablend
	gl->m_director->set_alpha_blending(true);
}

void map_tl::draw_block_image(const st_cell& cell,map_block* block)
{
	if (!block->m_Image)
		block->load_whole(&m_data);
	if (!block->m_Image)
		return;

	//return;
	block->m_Image->mark_use_image(get_time_now());
	get_render()->draw_image_cell(cell,block->m_Image,block->m_image_name.c_str(),NULL);
}

void map_tl::draw_block_mask(const st_cell& cell,map_block* block)
{
	if (!block->m_Image)
		return;
	if (m_data.get_mask_num(block->num) <= 0) //表示贴图是888的，不需要再次绘制。
		return;
	texture_gl* gl = (texture_gl*)get_graph()->find_texture(block->m_image_name.c_str());
	get_render()->draw_image_cell(cell,block->m_Image,block->m_image_name.c_str(),NULL);
}


bool map_tl::load_map(const char* file,int frame,g_size& sz)
{
	if (!m_data.load_map(file))
		return false;
	sz.w = m_data.get_width();
	sz.h = m_data.get_height();
	load_obs(replace_file_ext(file,"blk"));
	return true;
}

struct CHeader{
	int ver;
	//		int scale;		//原始阻挡与目前阻挡信息的比例
	int width;
	int height;
};

#define CELLSIZE 20

bool map_tl::load_obs(const char* file)
{
	size_t size;
	char* buf = read_resourcefile(file,size);
	if (!buf)
		return false;

	char* p = buf;
	CHeader cellheader;
	mread(&cellheader, p, sizeof(CHeader));
	if (cellheader.ver!='B1.0')
		return 1;

	int m_width = cellheader.width;
	int m_height = cellheader.height;

	unsigned char* m_buf = new unsigned char[m_width*m_height];

	int StackSize=cellheader.width*cellheader.height/2;

	unsigned char* des = m_buf;

	char tag[2] = {0x10,0x01};

	for(int i = 0; i< StackSize; ++i)
	{
		char cell;
		mread(&cell, p, 1);
		for (int k = 0; k < 2; k++,++des)
		{
			if (cell & tag[k])
				*des = 1;
			else
				*des = 0;
		}
	}

	delete buf;

	get_map()->m_obs->create_obs(m_width,m_height,m_buf,CELLSIZE);

	return true;
}


map_source* get_map_source_tl()
{
	return new map_tl;
}

