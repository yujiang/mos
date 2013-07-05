#pragma once

//树结构, 比较特殊, 是从叶节点向根节点反向链接，方便从叶节点找到根节点
//#include "action/point2.h"
//#include "findpath/findpath_struct.h"
#include "rect.h"
#include <list>

typedef std::list<g_point> PATH;

typedef struct tree_node *TREE;

struct tree_node {
	int h;            //节点所在的高度，表示从起始点到该节点所有的步数
	int tile;        //该节点的位置
	int dir;
	TREE father;    //该节点的上一步
};

//链接结构，用于保存处理过的和没有处理过的结点
typedef struct link_node *LINK;

struct link_node {
	TREE node;
	int f;
	LINK next;
};

void drawxy(int x,int y,char c);
typedef unsigned char Obs_type;

class cAStar
{
protected:
	Obs_type* map;
	int offx,offy;
	unsigned long* dis_map;					//保存搜索路径时,中间目标地最优解
	int map_w,map_h;						//地图宽和高
	int start_x,start_y,end_x,end_y;		//地点,终点坐标
	LINK sort_queue;						// 保存没有处理的行走方法的节点
	LINK store_queue;			            // 保存已经处理过的节点 (搜索完后释放)

	void init_queue(void);
	void enter_queue(TREE node,int f);
	TREE get_from_queue(void);
	void pop_stack(void);
	void freetree(void);
	int judge(int x,int y);
	int trytile(int dir,int x,int y,TREE father);
	bool findpath(PATH& listWay);
	void showmap();
public:
	virtual ~cAStar();
	cAStar();
	void create_astar(g_size size,Obs_type* buffer,const g_rect& rc) ;

	bool FindWay(const g_point& Start,const g_point& Des, PATH& listWay);
	inline void draw_point(int x,int y,char c){
		drawxy(x+1+offx,y+1+offy,c);
	}};

void clrscr();
void gotoxy(int x,int y);
inline void goto_point(int x,int y){
	gotoxy(x+1,y+1);
}
inline void goto_point(const g_point& pt){
	goto_point(pt.x,pt.y);
}
inline void draw_point(int x,int y,char c){
	drawxy(x+1,y+1,c);
}
inline void draw_point(const g_point& pt,char c){
	draw_point(pt.x,pt.y,c);
}

