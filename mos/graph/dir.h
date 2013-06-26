//------------------------------------------------------------
//拥有人	俞江 版本0.3 日期2010.1.12
//描述		2d求dir
//------------------------------------------------------------

#ifndef QIN_MOVE_DIR_H_
#define QIN_MOVE_DIR_H_


#include "point.h"
#include <math.h>

const int c_dir_num = 8;		//方向数目为8

inline int valid_dir8(int dir)
{
	if (dir < 0)
		return dir + c_dir_num;
	else if (dir >= c_dir_num)
		return dir - c_dir_num;
	return dir;
}

inline int get_back_dir8(int dir)
{
	return valid_dir8(dir + 4);
}

const int c_point_dir8_x[c_dir_num] =
{
	0,1,1,1,0,-1,-1,-1,		
};
const int c_point_dir8_y[c_dir_num] =
{
	-1,-1,0,1,1,1,0,-1,		
};

inline point2 get_target_pos8(point2 pt,int dir, int speed)
{
	pt.x += c_point_dir8_x[dir]* speed;
	pt.y += c_point_dir8_y[dir]* speed;
	return pt;
}

//这个是用来寻路使用的，不精确，并且优先斜者运动。
inline int get_dir8(int x,int y)
{
	if (x == 0 && y == 0)
		return -1;
	y = -y;
	//	ASSERT(x || y);
	int dir = 1;
	if (!x)
		dir = 0;
	else if (!y)
		dir = 2;

	if (x < 0)
		dir = c_dir_num - dir;
	if (y < 0)
	{
		dir = 4 - dir;
		if (dir < 0)
			dir += c_dir_num;
	}
	return dir;
}

inline int get_dir8(const point2& p)
{
	return get_dir8(p.x,p.y);
}

inline int get_dir8(const point2& ptTo,const point2& ptFrom)
{
	point2 p = ptTo-ptFrom;
	return get_dir8(p);
}

//////////////////////////////////////////////////////////////////////////
//a most good one..
const float c_point2_PI = 3.14159265f;
const float c_point2_fAlpha = 2.f*c_point2_PI/32.f;
const float c_point2_tg1 = (float)tan(c_point2_fAlpha*1);
const float c_point2_tg2 = (float)tan(c_point2_fAlpha*2);
const float c_point2_tg3 = (float)tan(c_point2_fAlpha*3);
const float c_point2_tg4 = (float)tan(c_point2_fAlpha*4);
const float c_point2_tg5 = (float)tan(c_point2_fAlpha*5);
const float c_point2_tg6 = (float)tan(c_point2_fAlpha*6);
const float c_point2_tg7 = (float)tan(c_point2_fAlpha*7);
const float c_point2_tgs[] = {-c_point2_tg7,-c_point2_tg5,-c_point2_tg3,-c_point2_tg1,c_point2_tg1,c_point2_tg3,c_point2_tg5,c_point2_tg7};


inline int math_get_dir(int x, int y, int c_nDir, const float af[])
{
	y = -y;
	int dir = 1;
	if (x == 0)
	{
		if (y == 0)
			return -1;
		if (y < 0)
			return c_nDir/2;
		else
			return 0;
	}

	int c_nDir2 = c_nDir/2;

	float c_point2_tga = (float)y / x;

	if (c_point2_tga < af[0])
	{
		dir = c_nDir2;
	}
	else if (c_point2_tga >= af[c_nDir2-1])
	{
		dir = 0;
	}
	else
	{
		for (int i=0; i<c_nDir2-1; i++)
		{
			if (c_point2_tga >= af[i] && c_point2_tga < af[i+1])
			{
				dir = c_nDir2-1-i;
				break;
			}
		}
	}
	if (x < 0)
		dir += c_nDir2;
	if (dir == c_nDir)
		dir = 0;
	return dir;
}

//求出ptFrom到ptTo方向上指向格子角落的方向
inline int math_get_dir_corner(const point2& ptTo, const point2& ptFrom)
{
	if (ptFrom.x == ptTo.x || ptFrom.y == ptTo.y)
	{
		return -1;
	}
	if (ptTo.x > ptFrom.x)
	{
		return (ptTo.y > ptFrom.y)? 3 : 1;
	}
	else
	{
		return (ptTo.y > ptFrom.y)? 5 : 7;
	}
}

inline int math_get_dir8(int x, int y)
{
	static float af[] = {-c_point2_tg6,-c_point2_tg2,c_point2_tg2,c_point2_tg6};
	return math_get_dir(x,y,8,af);
}

inline int math_get_dir8(const point2& ptTo, const point2& ptFrom)
{
	point2 pt = ptTo - ptFrom;
	return math_get_dir8(pt.x,pt.y);
}

inline int math_get_dir16(int x, int y)
{
	return math_get_dir(x,y,16,c_point2_tgs);
}

#endif


