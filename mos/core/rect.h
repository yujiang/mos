#ifndef __GRAPH_RECT_H_
#define __GRAPH_RECT_H_

#include "point.h"
typedef point2 g_point;

#ifndef max
#define max(a,b) (a)>(b)?(a):(b)
#endif
#ifndef min
#define min(a,b) (a)>(b)?(b):(a)
#endif

typedef unsigned long g_long;
//////////////////////////////////////////////////////////////////////////
///by cao jin lin
struct g_point_vertex
{
	g_point_vertex():x(0), y(0), z(0), w(1.0f), color(0xffffffff){}
	g_point_vertex(float _x, float _y, float _z, float _w, g_long _color)
		:x(_x), y(_y), z(_z), w(_w), color(_color){}
	float x, y, z, w;
	g_long color;      
};

struct g_size{
	g_size():w(0),h(0){}
	g_size(int _w,int _h):w(_w),h(_h){}
	g_size(const g_size& sz):w(sz.w),h(sz.h){}
	int w,h;	
	bool operator == (const g_size& sz) const{return w == sz.w && h == sz.h;}
	bool operator != (const g_size& sz) const{return w != sz.w || h != sz.h;}
	g_long get_area() const{return w*h;}
};

struct g_rect{
	g_rect():l(0),t(0),r(0),b(0){}
	g_rect(int _l,int _t,int _r,int _b):l(_l),t(_t),r(_r),b(_b){}
	g_rect(const g_rect& rc):l(rc.l),t(rc.t),r(rc.r),b(rc.b){}
	g_rect(g_point left_top,g_size size){
		l = left_top.x;
		t = left_top.y;
		r = left_top.x + (int)size.w;
		b = left_top.y + (int)size.h;
	}

	g_rect(g_point left_top, g_point right_bottom)
	{
		l = left_top.x;
		t = left_top.y;
		r = right_bottom.x;
		b = right_bottom.y;
	}

	int	l,t,r,b;
	void set_rect(int _l,int _t,int _r,int _b){
		l = _l;t = _t;r = _r;b = _b;
	}
	void set_xywh(int x,int y,int w,int h){
		l = x;t = y;r = x+w;b = y+h;
	}
	g_size get_size() const{return g_size((g_long)width(),(g_long)height());}
	//Ãæ»ý
	int get_area() const{return (width()*height());}
	g_point get_off() const{return g_point(l,t);}
	g_point get_lefttop() const{return get_off();}
	g_point get_rightbottom() const{return g_point(r,b);}
	g_point get_center() const{return g_point((l+r)/2, (t+b)/2);}

	void stretch_rect(float scaleX, float scaleY){
		r = l + static_cast<int>((r-l)*scaleX); b = t + static_cast<int>((b-t) * scaleY);
	}
	int width() const{return r-l;}
	int height() const{return b-t;}

	bool is_empty() const{return l >= r || t >= b;}
	bool is_strict_empty() const{return l > r || t > b;}
	bool have_point(const point2& pt) const{
		return pt.x >= l && pt.x < r && pt.y >= t && pt.y < b;
	}

	void enlarge(const point2& pt)
	{
		if (l > pt.x) l = pt.x;
		if (r < pt.x) r = pt.x;
		if (t > pt.y) t = pt.y;
		if (b < pt.y) b = pt.y;
	}

	void clamp(const g_size &sz){
		if (l < 0)
			l = 0;
		if (r > (int)sz.w)
			r = (int)sz.w;
		if (t < 0)
			t = 0;
		if (b > (int)sz.h)
			b = (int)sz.h;
	}
	inline void offseted(const g_point &pt)
	{
		l += pt.x;
		r += pt.x;
		t += pt.y;
		b += pt.y;
	}
	inline void unoffseted(const g_point &pt)
	{
		l -= pt.x;
		r -= pt.x;
		t -= pt.y;
		b -= pt.y;
	}

	inline bool operator ==(const g_rect &rc) const
	{
		return(  (l == rc.l)
			&&(t == rc.t)
			&&(r == rc.r)
			&&(b == rc.b));
	}
};

bool clip_rect(g_rect& des,g_rect& src,const g_rect& rc_clip);

inline g_rect operator + (const g_rect &r1, const g_rect &r2){
	return g_rect(min(r1.l,r2.l),min(r1.t,r2.t),max(r1.r,r2.r),max(r1.b,r2.b));
}
inline g_rect operator - (const g_rect &r1, const g_rect &r2){
	return g_rect(max(r1.l,r2.l),max(r1.t,r2.t),min(r1.r,r2.r),min(r1.b,r2.b));
}

inline bool rect_intersection(const g_rect& r1,const g_rect& r2){
	g_rect r = r1 - r2;
	return !r.is_empty();
}

typedef void* graph_hwnd;
typedef void* graph_dc;


#endif