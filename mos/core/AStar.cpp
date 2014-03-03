
#include "AStar.h"
#include "dir32.h"
#include <string.h>
#ifdef WIN32
#include <conio.h>
#include <windows.h>
#endif
#include <stdlib.h>

using namespace std;

#define tile_num(x,y) ((y)*map_w+(x))  //将 x,y 坐标转换为地图上块的编号
#define tile_x(n) ((n)%map_w)          //由块编号得出 x,y 坐标
#define tile_y(n) ((n)/map_w)

#define MAPMAXSIZE 180  //地图面积最大为 180x180，如果没有64K内存限制可以更大
#define c_MAXINT 32767

#define ASSERT(a)

//#define Trace_AStarFindPath

void init_astar()
{
}

void clear_astar()
{
}


void clrscr()
{
#ifdef WIN32
	HANDLE hCon = GetStdHandle( STD_OUTPUT_HANDLE );
	CONSOLE_SCREEN_BUFFER_INFO  csbi; 
	DWORD                       dwConSize;
	DWORD                       cCharsWritten; 
	COORD coordScreen = { 0, 0 };

	GetConsoleScreenBufferInfo( hCon, &csbi );
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y; 
	FillConsoleOutputCharacter( hCon, TEXT(' '),
		dwConSize, 
		coordScreen,
		&cCharsWritten ); 
	GetConsoleScreenBufferInfo( hCon, &csbi );
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y; 
	coordScreen.X = 0;
	coordScreen.Y = 0;
	FillConsoleOutputAttribute( hCon,
		csbi.wAttributes,
		dwConSize,
		coordScreen,
		&cCharsWritten ); 
	SetConsoleCursorPosition( hCon, coordScreen ); 
#endif
}

void gotoxy(int x,int y)
{
#ifdef WIN32
	HANDLE hCon = GetStdHandle( STD_OUTPUT_HANDLE );
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(hCon,coord);
#endif
}

void drawxy(int x,int y,char ch)
{
#ifdef WIN32
	gotoxy(x,y);
	char c[2];
	c[0] = ch;
	c[1] = 0;
	cprintf(c);
#endif
}

cAStar::~cAStar()
{
	delete map;
	map = NULL;
	delete dis_map;
	dis_map = NULL;
}

bool cAStar::FindWay(const g_point& Start,const g_point& Des, PATH& listWay)
{
	start_x = Start.x - offx;
	start_y = Start.y - offy;
	end_x = Des.x - offx;
	end_y = Des.y - offy;
	//showmap();
	return findpath(listWay);
}

cAStar::cAStar()
{
	map = NULL;
	dis_map = NULL;
}

#define OBS 0xff

void cAStar::create_astar(g_size size,Obs_type* buffer,const g_rect& rc) 
{
	map_w = rc.width()+2;
	map_h = rc.height()+2;
	Obs_type* _map = new Obs_type[map_w*map_h];
	map = _map;
	offx = rc.l-1;
	offy = rc.t-1;
	Obs_type* src = buffer + rc.t * size.w + rc.l;
	_map += map_w + 1;
	for (int i=0; i<rc.height(); i++)
	{
		memcpy(_map,src,rc.width()*sizeof(Obs_type));
		_map += map_w;
		src += size.w;
	}
	//把地图的四周加上阻挡
	for (int x=0; x<map_w; x++)
	{
		map[tile_num(x,0)] = OBS;
		map[tile_num(x,map_h-1)] = OBS;
	}
	for (int y=0; y<map_h; y++)
	{
		map[tile_num(0,y)] = OBS;
		map[tile_num(map_w-1,y)] = OBS;
	}

	dis_map = new unsigned long[map_w*map_h];
}

void cAStar::showmap(void)
{
#ifdef WIN32
#ifndef Trace_AStarFindPath
	return;
#endif
	int i,j;
	clrscr();
	for (i=0;i<map_h;i++) {
		gotoxy(1,i+1);
		for (j=0;j<map_w;j++)
		{
			switch(map[tile_num(j,i)])
			{
			case 0:
				cprintf(" ");
				break;
			case OBS:
				cprintf("B");
				break;
			default: 
				cprintf("O");
			}
		}
	}
	draw_point(start_x,start_y,'s');
	draw_point(end_x,end_y,'e');
#endif
}

// 初始化队列
void cAStar::init_queue(void)
{
	sort_queue=new link_node;
	sort_queue->node=NULL;
	sort_queue->f=-1;
	sort_queue->next=new link_node;
	sort_queue->next->node=NULL;
	sort_queue->next->f=c_MAXINT;
	sort_queue->next->next=NULL;

	store_queue=new link_node;
	store_queue->node=NULL;
	store_queue->f=-1;
	store_queue->next=NULL;
}

// 待处理节点入队列, 依靠对目的地估价距离插入排序
void cAStar::enter_queue(TREE node,int f)
{
	LINK p=sort_queue,father,q;
	while(f>p->f) {
		father=p;
		p=p->next;
		ASSERT(p);
	}
	q=new link_node;
	ASSERT(sort_queue);
	q->f=f,q->node=node,q->next=p;
	father->next=q;
}

// 将离目的地估计最近的方案出队列
TREE cAStar::get_from_queue(void)
{
	LINK bestchoice=sort_queue->next;
	LINK next=sort_queue->next->next;
	sort_queue->next=next;

	bestchoice->next=store_queue->next;
	store_queue->next=bestchoice;
	return bestchoice->node;
}

// 释放栈顶节点
void cAStar::pop_stack(void)
{
	LINK s=store_queue->next;
	ASSERT(s);
	store_queue->next=store_queue->next->next;
	delete(s->node);
	delete(s);
}

// 释放申请过的所有节点
void cAStar::freetree(void)
{
	LINK p;
	while(store_queue){
		p=store_queue;
		delete(p->node);
		store_queue=store_queue->next;
		delete(p);
	}
	while (sort_queue) {
		p=sort_queue;
		delete(p->node);
		sort_queue=sort_queue->next;
		delete(p);
	}
}

// 估价函数,估价 x,y 到目的地的距离,估计值必须保证比实际值小
int cAStar::judge(int x,int y)
{
	//return abs(end_x-x)+abs(end_y-y);
	int dx,dy;
	dx=abs(x-end_x),dy=abs(y-end_y);
	if (dx>dy) 
		return 10*dx+6*dy; 
	else 
		return 10*dy+6*dx;
}

// 尝试下一步移动到 x,y 可行否
int cAStar::trytile(int dir,int x,int y,TREE father)
{
	TREE p=father;
	int h;
	if (map[tile_num(x,y)]!=0) return 1; // 如果 (x,y) 处是障碍,失败
	//这一步用来判断(x,y)点是否已经加入队列，多余可以删除，因为dis_map已经
	//保存该点是否已经保存
	//while (p) {
	//  if (x==tile_x(p->tile) && y==tile_y(p->tile)) return 1; //如果 (x,y) 曾经经过,失败
	//  p=p->father;
	//}
	h=father->h+1;
	if (h>=dis_map[tile_num(x,y)]) return 1; // 如果曾经有更好的方案移动到 (x,y) 失败
	dis_map[tile_num(x,y)]=h; // 记录这次到 (x,y) 的距离为历史最佳距离

	// 将这步方案记入待处理队列
	p=new tree_node;
	p->father=father;
	p->h=father->h+1;
	p->tile=tile_num(x,y);
	p->dir = dir;
	enter_queue(p,p->h+judge(x,y));
	return 0;
}

// 路径寻找主函数 //from 云风
bool cAStar::findpath(PATH& listWay)
{
	TREE root;
	int i;
	memset(dis_map,0xff,map_h*map_w*sizeof(*dis_map));    //填充dis_map为0XFF，表示各点未曾经过
	init_queue();
	root=new tree_node;
	root->tile=tile_num(start_x,start_y);
	root->h=0;
	root->dir = -1;
	root->father=NULL;
	enter_queue(root,judge(start_x,start_y));
	int dir = math_get_dir8(g_point(end_x,end_y),g_point(start_x,start_y));
	for (;;) {
		int x,y,child;
//		TREE p;
		root=get_from_queue();
		if (root==NULL) {
#ifdef Trace_AStarFindPath
			gotoxy(1,map_h+2);
			cout << "not find way" << endl;
#endif
			freetree();
			return false;
		}
		x=tile_x(root->tile);
		y=tile_y(root->tile);
#ifdef Trace_AStarFindPath
		//draw_point(x,y,'\'');
#endif
		if (x==end_x && y==end_y) break; // 达到目的地成功返回

		child=1;

		//尝试向四周移动时，考虑人物方向,这时人物都是左侧通行的
		for (int i=0;i<8; ++i)
		{
			int current_dir = valid_dir8(dir+i);
			child&=trytile(current_dir, x+c_point_dir8_x[current_dir],y+c_point_dir8_y[current_dir],root);  
		}
		//child&=trytile(0,x,y-1,root);  //尝试向上移动
		//child&=trytile(1,x+1,y-1,root);//尝试向右上移动
		//child&=trytile(2,x+1,y,root); //尝试向右移动
		//child&=trytile(3,x+1,y+1,root); //尝试向右下移动
		//child&=trytile(4,x,y+1,root); //尝试向下移动
		//child&=trytile(5,x-1,y+1,root); //尝试向左下移动
		//child&=trytile(6,x-1,y,root); //尝试向左移动
		//child&=trytile(7,x-1,y-1,root); //尝试向左上移动

		if (child!=0)
			pop_stack();  // 如果四个方向均不能移动,释放这个死节点
	}

	// 回溯树，将求出的最佳路径保存在 path[] 中
	// cout << "find way:  ";
	g_point pt = g_point(end_x,end_y);
	g_point prev_pt = pt;

	int prev_dir = -1;

	if (root->dir < 0)   //如果终点方向是负的，说明没找到路径
	{
		return false;
	}
	for (i=0;root->dir>=0;i++) {
		//cout << root->dir << ",";
		int backdir = root->dir + 4;
		if (backdir >= 8)
			backdir -= 8;
		pt += c_aPointDir8[backdir];
#ifdef Trace_AStarFindPath
		draw_point(pt.x,pt.y,'X');
#endif
		if (root->dir != prev_dir)
		{
			prev_dir = root->dir ;
		}
		else
		{
			//倒数第二个节点要保留
			if (root->father->dir>=0)
			{
				listWay.pop_front();
			}
		}
		if(root->father->dir < 0)
		{
			//最后一点不要插入，方便外部做优化
			//g_point pt_edge = g_point((pt.x + prev_pt.x)/2, (pt.y + prev_pt.y)/2);
			//listWay.push_front(pt_edge + g_point(offx, offy));
			prev_pt = pt;
			root = root->father;
			continue;
		}

		listWay.push_front(pt + g_point(offx,offy));
		prev_pt = pt;
		//listWay.push_front(root->dir);
		root=root->father;
	}
#ifdef Trace_AStarFindPath
	cout << endl;
#endif
	freetree();
	return true;
}

