
#include "cell.h"
#include "graph.h"
#include <vector>
#include <unordered_map>
#include <assert.h>
#include <string>

std::unordered_map<std::string,int> g_hmOffsetInt;
std::unordered_map<std::string,int> g_hmOffsetStr;

#define offsetint(a) g_hmOffsetInt[#a] = offsetof(st_cell,a);
#define offsetstr(a) g_hmOffsetStr[#a] = offsetof(st_cell,a);

void init_cell()
{
	offsetint(is_window);
	offsetint(is_box);
	//offsetint(is_text);
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

	offsetint(font);
	offsetint(align);
	offsetint(wrap);
	offsetint(bold);
	offsetint(underline);

	offsetstr(image_file);
	offsetstr(shade);
	offsetstr(text);

	//st_cell cell;
	//cell.set_kv("y",12);
	//cell.set_kv("name","haha");
	//assert(cell.y == 12);
}

void st_cell::init()
{
	memset(this,0,sizeof(st_cell));
	color = -1;
	alpha = 255;
	room = 1.f;
}

st_cell::st_cell(const st_cell& r1,const st_cell r2)
{
	x = r1.x + r2.x;
	y = r1.y + r2.y;
	//z 不必变
	room = r1.room * r2.room;
	//color 不必变
	//alpha 也用个乘法
	alpha = r1.alpha * r2.alpha / 255;
}

void st_cell::set_kv(const char* key,lua_Number value)
{
	if (key[0] == 'r')
	{
		room = value;
		return;
	}
	int offset = g_hmOffsetInt[key];
	if (offset == 0)
		return;
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

void cell::draw(int level,const st_cell& st) const
{
	st_cell st2(st,*this);
	if (is_window)
		get_graph()->draw_win_begin(st2.x,st2.y,st.w,st.h);
	else if (image_file)
	{
		if (cx == 0 && cy == 0) 
			get_graph()->draw_image(st2,image_file,frame);
		else
		{
			st2.x -= cx;
			st2.y -= cy;
			get_graph()->draw_image(st2,image_file,frame);
			st2.x += cx;
			st2.y += cy;
		}
	}
	else if(text)
	{
		g_rect r;
		r.set_xywh(st.x,st.y,w,h);
		//use st2 x,y,color,alpha and use *this font...
		get_graph()->draw_text(st2,*this,r); //*this x,y 也是有用的。
	}
	else if(is_box)
	{
		st2.color = color;
		get_graph()->draw_box(st2,w,h);
	}
	for (auto it = childs.begin(); it != childs.end(); ++it)
		(*it)->draw(level+1,st2);
	if (is_window)
		get_graph()->draw_win_end();
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


int lua_render(lua_State *L) 
{
	get_graph()->render_begin();

	cell* root = g_cells.construct();
	lua_walk(L,root);
	//root->print(0);
	//printf("\n");
	st_cell st;
	root->draw(0,st);
	//delete root;
	g_cells.clear_all();

	get_graph()->render_end();

	return 1;
}


