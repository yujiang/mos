
#include "dir32.h"
#include "core/cc.h"
#include <stdlib.h>
//#include "time.h"

#define ASSERT(a)

#define point2_fAlpha c_point2_fAlpha
#define point2_PI c_point2_PI

#define math_GetDir math_get_dir
#define math_GetDir16 math_get_dir16
#define math_GetDir8 math_get_dir8
#define GetDir8 get_dir8

#define Distance pos_distance


//32 方向已经足够精确了，不行的话还可以变为64,128方向
float c_tg64[16] = {0};
g_point c_aPoint32[32][max_point32];
g_point c_aPointDir8[c_nNumDir8];

void init_tg64()
{
	float a = point2_fAlpha/2.f;
	float mul = -15.f;
	for (int i=0; i<16; i++)
	{
		c_tg64[i] = (float)tan(a*mul);
		mul += 2.f;
	}
}

void init_point32()
{
	//each 
	float a = point2_fAlpha;
	for (int i=0;i<9;i++)
	{
		g_point* aPoint = c_aPoint32[i];
		if (i == 0 || i == 16)
		{
			int x,y;
			x = y = 0;
			for (int j=0; j<max_point32; j++)
			{
				aPoint[j] = g_point(x,y);
				if (i == 0)
					y++;
				else
					y--;
			}
		}
		else
		{
			float k = tan(point2_PI/2-a*i);
			int num = 0;
			int x = 0;
			float fy = 0;
			int py = 0;

			int dx = 1;
			if (i >= 16)
				dx = -1;
			do
			{
				fy += dx*k;
				int y;
				if (dx*k > 0)
				{
					for (y = py; y < fy; y++)
					{
						aPoint[num++] = g_point(x,y);
						if (num == max_point32)
							break;
					}
				}
				else
				{
					for (y = py; y > fy; y--)
					{
						aPoint[num++] = g_point(x,y);
						if (num == max_point32)
							break;
					}
				}

				if (num == max_point32)
					break;

				if (py == y)
					aPoint[num++] = g_point(x,py);
				else
					py = y;

				x += dx;
			}
			while(num < max_point32);
		}
	}

	for (int i=9; i<17; i++)
	{
		g_point* aPoint = c_aPoint32[i];
		g_point* aPointSrc = c_aPoint32[16-i];
		for (int j=0; j<max_point32; j++)
		{
			aPoint[j] = aPointSrc[j];
			aPoint[j].y *= -1;
		}
	}

	for (int i=17; i<32; i++)
	{
		g_point* aPoint = c_aPoint32[i];
		g_point* aPointSrc = c_aPoint32[32-i];
		for (int j=0; j<max_point32; j++)
		{
			aPoint[j] = aPointSrc[j];
			aPoint[j].x *= -1;
		}
	}

	for (int i=0; i<32; i++)
	{
		g_point* aPoint = c_aPoint32[i];
		for (int j=0; j<max_point32; j++)
		{
			aPoint[j].y *= -1;
		}
	}
}


int math_GetDir32(int x, int y)
{
	return math_GetDir(x,y,32,c_tg64);
}



//pos 在 dir的扇形中么
//dir 是32方向的dir
//angle = pi/32 的倍数
bool IsFan(const g_point& pos, int dir32, int angle)
{
	ASSERT(angle <= 8);
	int posdir = math_GetDir32(pos.x,pos.y);
	int a1 = abs(dir32-posdir);
	int a2 = dir32+32-posdir;
	int a3 = abs(dir32-32-posdir);
	int a = QIN_MIN(QIN_MIN(a1,a2),a3);
	return a <= angle;
	return false;
}

void init_dir32()
{
	for (int i=0; i<c_nNumDir8; i++)
	{
		c_aPointDir8[i].x = c_point_dir8_x[i];
		c_aPointDir8[i].y = c_point_dir8_y[i];
	}
	init_tg64();
	init_point32();
	//testdir32();
}
