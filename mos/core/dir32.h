#ifndef DIR32_H
#define DIR32_H

#include "rect.h"
#include "dir.h"

const int max_point32 = 100;

const int c_nNumDir8 = 8;		//8个方向

extern g_point c_aPointDir8[c_nNumDir8] ;

extern g_point c_aPoint32[32][max_point32];

void init_dir32();

int math_GetDir32(int x, int y);
inline int math_GetDir32(const g_point& ptTo,const g_point& ptFrom)
{
	g_point pt = ptTo - ptFrom;
	return math_GetDir32(pt.x,pt.y);
}

//pos 在 dir的扇形中么
//dir 是32方向的dir
//angle = pi/32 的倍数 must <= 8 =8 mean 一半
bool IsFan(const g_point& pos, int dir32, int angle);

#endif
