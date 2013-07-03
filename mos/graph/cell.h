#ifndef __GRAPH_CELL_H_
#define __GRAPH_CELL_H_

struct lua_State;
extern "C"
{
#include "lua.h"
#include "lauxlib.h"
}

#include "graph/rect.h"

#define CELL_MAX_PARTS 6

class st_cell
{
public:
	int _not_use;

	const char* name;

	int x,y,z;
	int cx,cy; //重心

	float room;		//room改变offset，roomall改变render!

	int w,h;

	int color;
	int alpha;

	const char* image_file;
	int frame;

	const char* shader;
	float shader_param;

	//font
	int font;
	const char* text;
	int wrap,align,bold,underline;

	int is_box;		//for box
	
	int is_window;	//for window use
	//一个window上的图素可以一次绘制，合并其上的image和text，然后一次性绘制image和text。
	//尚未实现
	
	int is_map;		//for map
	const char* map_file;
	//一个特殊的实现，因为map的格式复杂，并且map可能是动态的，包括其上的遮罩，处理也复杂。

	unsigned long part0,part1,part2,part3,part4,part5;
	//int shader_light;	//发亮，可以用shader，但是part不行，因为part是分块的无法支持。

	st_cell();
	void init();
	//st_cell(const st_cell& r1,const st_cell& r2);
	void merge(const st_cell& r1,const st_cell& r2);

	void set_kv(const char* key,lua_Number value);
	void set_kv(const char* key,const char* value);

	void st_print(int level) const;
};

int lua_render(lua_State *L);
void init_cell();

#endif