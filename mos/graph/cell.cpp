
#include "cell.h"
#include "graph.h"
#include "map/map.h"
#include "device/window_render.h"
#include "mos.h"
#include "lua_x.hpp"
#include <vector>
#include <unordered_map>
#include <assert.h>
#include <string>
#include "core/wait_notify.h"

#pragma warning(disable:4244)

std::unordered_map<std::string,int> g_hmOffsetInt;
std::unordered_map<std::string,int> g_hmOffsetStr;

#define offsetint(a) g_hmOffsetInt[#a] = offsetof(st_cell,a);
#define offsetstr(a) g_hmOffsetStr[#a] = offsetof(st_cell,a);

void init_cell()
{
	offsetint(is_window);
	offsetint(is_box);
	offsetint(is_map);

	offsetstr(name);

	offsetint(alpha);
	offsetint(color);
	offsetint(x);
	offsetint(y);
	offsetint(z);
	offsetint(cx);
	offsetint(cy);
	offsetint(w);
	offsetint(h);
	offsetint(frame);
	offsetint(need_mask);

	offsetint(font);
	offsetint(align);
	offsetint(wrap);
	offsetint(bold);
	offsetint(underline);

	offsetstr(image_file);
	offsetstr(map_file);
	offsetstr(shader);
	offsetstr(text);
	
	offsetint(part0);
	offsetint(part1);
	offsetint(part2);
	offsetint(part3);
	offsetint(part4);
	offsetint(part5);
	//st_cell cell;
	//cell.set_kv("y",12);
	//cell.set_kv("name","haha");
	//assert(cell.y == 12);
}

st_cell::st_cell()
{
	init();
};

void st_cell::init()
{
	memset(this,0,sizeof(st_cell));
	color = -1;
	alpha = 255;
	room = 1.f;
}

void st_cell::set_kv(const char* key,lua_Number value)
{
	//这个太trick了。
	int offset = g_hmOffsetInt[key];
	if (offset == 0)
	{
		switch(key[0])
		{
		case 'r':
			if (strcmp(key,"room") == 0)
				room = value;
			break;
		case 's':
			if (strcmp(key,"shader_param") == 0)
				shader_param = value;
			break;
		}
		return;
	}
	*((int*)(((char*)this)+offset)) = value;
}

void st_cell::set_kv(const char* key,const char* value)
{
	int offset = g_hmOffsetStr[key];
	if (offset == 0)
		return;
	*((const char**)(((char*)this)+offset)) = value;
}

void st_cell::st_print(int level) const
{
	for (int i=0;i<level;i++)
		printf("  ");
	printf("%s(%d,%d,%d,%.2f) ",name,x,y,z,room);
	if (color != -1 || alpha != 255)
		printf("color(%d,%d) ",color,alpha);
	if (image_file)
		printf("file(%s,%d) ",image_file,frame);
	else if(text)
		printf("text(%s) ",text);
	else if(is_box)
		printf("box(%d,%d) ",w,h);
	printf("\n");
}

class cell : public st_cell
{
public:
	std::vector<cell*> childs;

public:
	void reinit(){
		childs.clear();
		init();
	}
	void add_child(cell* r,int index);
	void print(int level) const;
	void draw(int level,const st_cell& st) const;
	void draw_texture(int level,const st_cell& st) const;
};

#include "core/pool.h"
core::temp_pool<cell> g_cells;

void cell::add_child(cell* r,int index)
{
	if (childs.size() < index)
		childs.resize(index);
	childs[index-1] = r;
}

void cell::print(int level) const
{
	st_print(level);
	for (auto it = childs.begin(); it != childs.end(); ++it)
		(*it)->print(level+1);
}

void st_cell::merge(const st_cell& father,const st_cell& me)
{
	const st_cell& r1 = father;
	const st_cell& r2 = me;
	//init();
	memcpy(&part0,&r2.part0,sizeof(part0)*CELL_MAX_PARTS);

	//后面覆盖前面的shader!
	if (r2.shader)
	{
		shader = r2.shader ;
		shader_param = r2.shader_param;
	}
	else
	{
		shader = r1.shader ;
		shader_param = r1.shader_param;
	}

	x = r1.x + r2.x * r1.room;
	y = r1.y + r2.y * r1.room;

	room = r1.room * r2.room;

	w = r2.w;
	h = r2.h;
	cx = r2.cx;
	cy = r2.cy;
	//need_mask = need_mask | r2.need_mask;

	//color 不必变
	//alpha 也用个乘法
	color = r2.color;
	alpha = r1.alpha * r2.alpha / 255;
}

void cell::draw_texture(int level,const st_cell& st) const
{
	//st_cell st2(st,*this);
	st_cell st2;
	st2.merge(st,*this);

	if (is_window)
	{
	}
	else if(is_box)
	{
		st2.color = color;
		get_graph()->draw_box(st2,w,h);
	}
	else 
	{
		if(text)
		{			
			g_rect r;
			r.set_xywh(st.x,st.y,w,h);
			get_graph()->draw_text(st2,*this,r); 
		}
		else if (image_file) 
		{
			if (image_file[0] == 'm')
				get_graph()->draw_texturemul_index(st2,frame);
			else
				get_graph()->draw_texturemap_index(st2,frame);
		}
	}

	for (auto it = childs.begin(); it != childs.end(); ++it)
		(*it)->draw_texture(level+1,st2);
}

void cell::draw(int level,const st_cell& st) const
{
	//st_cell st2(st,*this);
	st_cell st2;
	st2.merge(st,*this);

	if (is_window)
		get_graph()->draw_win_begin(st2.x,st2.y,st.w,st.h,*this);
	else if(is_map)
		get_map()->draw_map_begin(st2.x,st2.y,st.w,st.h,*this);
	else 
	{
		int x0 = st2.x;
		int y0 = st2.y;
	
		//image_file,text,is_box是互斥的，本来也可以并行，有一个默认排序即可
		//但是保持结构，还是互斥。
		if (image_file)
		{
			//必须修改这里才行，来修改offset。和destexture。
			st2.x -= cx * st2.room;
			st2.y -= cy * st2.room;
			if (get_graph()->draw_image(st2,image_file,frame) >= 0)
			{
				//因为3d可以一次绘制所有的mask，就不必像2d那样绘制了。
				//还是不行，太天真了。
				if (need_mask && get_map()->m_in_map)
				{
					st2.x = x0;
					st2.y = y0;
					get_map()->mask_drawing_image(st2);
				}
			}
		}
		else if(map_file) //地图文件
		{
			get_map()->draw_map_image(st2,map_file,frame);
		}
		else if(text)
		{			
			g_rect r;
			r.set_xywh(st.x,st.y,w* st2.room,h* st2.room);
			get_graph()->draw_text(st2,*this,r); //*this x,y 也是有用的。
		}
		else if(is_box)
		{
			st2.color = color;
			get_graph()->draw_box(st2,w*st2.room,h*st2.room);
		}

		st2.x = x0;
		st2.y = y0;
	}

	for (auto it = childs.begin(); it != childs.end(); ++it)
		(*it)->draw(level+1,st2);

	if (is_window)
		get_graph()->draw_win_end();
	else if(is_map)
		get_map()->draw_map_end();
}


static int lua_walk(lua_State *L,cell* f) 
{
	luaL_checktype(L, 1, LUA_TTABLE);
	for (lua_pushnil(L); lua_next(L, -2); lua_pop(L, 1))
	{
		if (lua_isnumber(L,-2) && lua_istable(L,-1))
		{
			//cell* c = new cell();
			cell* c = g_cells.construct();
			lua_walk(L,c);
			f->add_child(c,lua_tointeger(L,-2));
		}
		else if(lua_isstring(L,-2))
		{
			const char* key = lua_tostring(L,-2);
			if (lua_isboolean(L,-1))
			{
				int value = lua_toboolean(L,-1);
				f->set_kv(key,value);
			}
			else if (lua_isnumber(L,-1))
			{
				lua_Number value = lua_tonumber(L,-1);
				f->set_kv(key,value);
			}
			else if(lua_isstring(L,-1))
			{
				const char* value = lua_tostring(L,-1);
				f->set_kv(key,value);
			}
		}
	}
	return 1;
}


std::thread* g_thread_render;


wait_notify g_wn_renderstart;
wait_notify g_wn_renderfinish;
//wait_notify g_wm_threadend;

extern bool g_exit;

void thread_render_func()
{
	while(!g_exit)
	{
		g_wn_renderstart.wait();
		if (g_exit)
			break;
		get_render()->render_end();
		g_wn_renderfinish.notify();
	}
	//g_wm_threadend.notify();
}

int lua_render(lua_State *L) 
{
	cell* root = g_cells.construct();
	lua_walk(L,root);

	if (get_render()->is_thread())
	{
		if (!g_thread_render)
		{
			g_thread_render = new std::thread(&thread_render_func);
			//g_thread_render->detach();
		}
		else
			g_wn_renderfinish.wait();
	}

	get_render()->render_start0();
	st_cell st;
	root->draw(0,st);
	g_cells.clear_all();
	get_graph()->auto_clear_resource();

	if (get_render()->is_thread())
	{
		g_wn_renderstart.notify();
	}
	else
	{		
		//get_render()->render_end();
	}

	return 1;
}

void end_thread_cell()
{
	if (g_thread_render)
	{
		g_wn_renderstart.notify();

		g_thread_render->join();
		//g_wm_threadend.wait();
		delete g_thread_render;
		g_thread_render = NULL;
	}
}

int lua_render_texture(lua_State *L) 
{
	if (get_render()->is_thread())
		return 0;
	cell* root = g_cells.construct();
	lua_walk(L,root);
	st_cell st;
	root->draw_texture(0,st);
	g_cells.clear_all();

	return 1;
}