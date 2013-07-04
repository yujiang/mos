#ifndef MAPOBS_H
#define MAPOBS_H

#include "../graph/point.h"
#include <vector>

class mapobs
{
public:
	int m_width,m_height;
	char* m_buf;

	int is_block(int x,int y){
		return m_buf[y*m_width+x];
	}
	int is_block_safe(int x,int y);

	int m_scale;
	void create_obs(int w,int h,char* buf,int scale)
	{
		destory_obs();
		m_width = w;
		m_height = h;
		m_buf = buf;
		m_scale = scale;
	}
	mapobs(){
		m_buf = NULL;
	}
	~mapobs();
	void destory_obs();

	//Ѱ·
	bool find_path(const point2& from,const point2& to,std::vector<point2>& path);
	bool find_path_pixel(const point2& from,const point2& to,std::vector<point2>& path);
};

#endif