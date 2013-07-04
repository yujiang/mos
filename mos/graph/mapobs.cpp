#include "mapobs.h"
#include "../device/file.h"


int mapobs::is_block_safe(int x,int y)
{
	if (x < 0 || x >= m_width)
		return true;
	if (y < 0 || y >= m_height)
		return true;
	return is_block(x,y);
}


mapobs::~mapobs()
{
	destory_obs();
}

void mapobs::destory_obs()
{
	delete m_buf;
	m_buf = NULL;
}


bool mapobs::find_path(const point2& from,const point2& to,std::vector<point2>& path)
{
	if (is_block_safe(from.x,from.y) || is_block_safe(to.x,to.y))
		return false;
	path.push_back(to);
	return true;
}

bool mapobs::find_path_pixel(const point2& from,const point2& to,std::vector<point2>& path)
{
	point2 p1 = from;
	p1.x /= m_scale;
	p1.y /= m_scale;
	point2 p2 = to;
	p2.x /= m_scale;
	p2.y /= m_scale;

	bool rt = find_path(p1,p2,path);
	if (!rt)
		return rt;
	for (auto it=path.begin(); it != path.end(); ++it)
	{
		(*it).x *= m_scale;
		(*it).y *= m_scale;
	}
	return rt;
}