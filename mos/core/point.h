#ifndef __GRAPH_POINT_H_
#define __GRAPH_POINT_H_

//wrap opengl texture not directx.
struct point2;

struct point2_short
{
	short x,y;
	void operator = (const point2& pos);
};

struct point2{
	point2() : x(0),y(0){}
	point2(long _x,long _y) : x(_x),y(_y){}
	point2(const point2_short& pos) : x(pos.x),y(pos.y){}
	long x,y;
	void operator -= (const point2& pt){
		x -= pt.x;
		y -= pt.y;
	}
	void operator += (const point2& pt){
		x += pt.x;
		y += pt.y;
	}
	bool operator == (const point2& pt) const{
		return (x == pt.x && y == pt.y);
	}
	bool operator !=  (const point2& pt) const{
		return (x != pt.x || y != pt.y);
	}

	void operator = (const point2_short& pos) {
		x = pos.x;
		y = pos.y;
	}
	bool operator < (const point2& pos) const {
		return x < pos.x || (x == pos.y && y < pos.y);
	}
	long size_t() const
	{
		return y * 10000 + x;
	}
};

inline void point2_short::operator = (const point2& pos) 
{
	x = (short)pos.x;
	y = (short)pos.y;
}

inline point2 operator - (const point2& p1,const point2& p2)
{
	return point2(p1.x-p2.x,p1.y-p2.y);
}

inline point2 operator + (point2 p1, point2 p2)
{
	return point2(p1.x + p2.x , p1.y + p2.y);
}


struct rect2
{
	point2 lu,rb;
	rect2(const point2& center,unsigned long range):lu(center.x-range,center.y-range),rb(center.x+range,center.y+range){}
	rect2(const point2& p1,const point2& p2):lu(p1),rb(p2){}
	bool point_in_rect(const point2& pos) const{
		return (pos.x >= lu.x && pos.y >= lu.y && pos.x <= rb.x && pos.y <= rb.y);
	}
};


#endif