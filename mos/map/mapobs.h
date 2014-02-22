#ifndef MAPOBS_H
#define MAPOBS_H

#include "../core/point.h"
#include <list>

class cAStar;

class mapobs
{
public:
	int m_width,m_height;
	unsigned char* m_buf;
	void create_obs(int w,int h,unsigned char* buf,int scale);

	int is_block(int x,int y) const{
		return m_buf[y*m_width+x];
	}
	int is_block_safe(int x,int y) const;

	int m_scale;
	void pixel_2_cell(point2& pt) const{
		pt.x /= m_scale;
		pt.y /= m_scale;
	}
	void cell_2_pixel(point2& pt) const{
		pt.x *= m_scale;
		pt.y *= m_scale;
	}

	mapobs(){
		m_buf = NULL;
		m_astar = NULL;
	}
	~mapobs();
	void destroy_obs();

	//Ѱ·
	cAStar* m_astar;
	bool find_path(const point2& from,const point2& to,std::list<point2>& path);
	bool find_path_pixel(const point2& from,const point2& to,std::list<point2>& path);

	bool is_block_seam(const point2& from,const point2& to) const;
	bool is_block_seam_pixel(const point2& from,const point2& to) const;
	bool is_block_point(const point2& pos) const;
	bool is_block_point_pixel(const point2& pos) const;

	//return -1 (not find) 0 (find not change) 1 (find and change)
	int find_notobs_seam(const point2& pos, point2& click) const;

	void smooth_way(std::list<point2>& path);
};

#endif