
#include "driver.h"
#include "lua_x.hpp"
#include "graph/cell.h"
#include "graph/graph.h"
#include "graph/image.h"
#include "graph/image_db.h"
#include "tl/image_zgp.h"
#include "graph/font.h"
#include "core/dir.h"
#include "graph/texture.h"
#include "device/window.h"
#include "device/window_render.h"
#include "map/map.h"
#include "map/mapobs.h"
#include "mos.h"

#include <string>
using namespace std;

static int lua_in_image(lua_State *L) {
	const char* file = luaL_checkstring(L,1);
	int frame = luaL_checkinteger(L,2);
	int x = luaL_checkinteger(L,3);
	int y = luaL_checkinteger(L,4);
	
	const st_redirect* r = redirect_image_file(file,frame);
	if (r)
		file = r->file_image.c_str();
	image* img = get_graph()->find_image_raw(file,frame,NULL);
	if (img)
	{
		if (r)
			lua_pushboolean(L,img->in_image(x+r->rc.l,y+r->rc.t));
		else
			lua_pushboolean(L,img->in_image(x,y));
		return 1;
	}
	return 0;
}

static int lua_get_text_line (lua_State *L) {
	st_cell text;
	text.text = luaL_checkstring(L,1);
	text.font = luaL_checkinteger(L,2);
	text.wrap = luaL_checkinteger(L,3);

	g_size sz;
	if (lua_gettop(L) == 5)
	{
		sz.w = luaL_checkinteger(L,4);
		sz.h = luaL_checkinteger(L,5);
	}

	int line = get_graph()->get_text_line(text,sz);
	lua_pushinteger(L,line);
	return 1;
}

static int lua_get_text_size(lua_State *L) {
	st_cell text;
	text.text = luaL_checkstring(L,1);
	text.font = luaL_checkinteger(L,2);
	text.wrap = luaL_checkinteger(L,3);

	g_size sz;
	if (lua_gettop(L) == 5)
	{
		sz.w = luaL_checkinteger(L,4);
		sz.h = luaL_checkinteger(L,5);
	}

	g_size size = get_graph()->get_text_size(text,sz);
	lua_pushinteger(L,size.w);
	lua_pushinteger(L,size.h);
	return 2;
}

static int lua_get_image_size (lua_State *L) {
	const char* file = luaL_checkstring(L,1);
	int frame = -1;
	if (lua_gettop(L) == 2)
		frame = luaL_checkinteger(L,2);
	g_size sz;
	if (get_graph()->get_image_size(file,frame,sz))
	{
		lua_pushinteger(L,sz.w);
		lua_pushinteger(L,sz.h);
		return 2;
	}
	printf("error! get_image_size %s %d\n",file,frame);
	lua_pushinteger(L,0);
	lua_pushinteger(L,0);
	return 2;
}

static int lua_load_map(lua_State *L) {
	const char* file = luaL_checkstring(L,1);
	int frame = -1;
	if (lua_gettop(L) == 2)
		frame = luaL_checkinteger(L,2);
	g_size sz;
	if (get_map()->load_map(file,frame,sz))
	{
		lua_pushinteger(L,sz.w);
		lua_pushinteger(L,sz.h);
		return 2;
	}
	printf("error! load_map %s %d\n",file,frame);
	lua_pushinteger(L,0);
	lua_pushinteger(L,0);
	return 2;
}

static int lua_get_image_sizecg (lua_State *L) {
	const char* file = luaL_checkstring(L,1);
	int frame = -1;
	if (lua_gettop(L) == 2)
		frame = luaL_checkinteger(L,2);
	g_size sz;
	g_point cg;
	if (get_graph()->get_image_sizecg(file,frame,sz,cg))
	{
		lua_pushinteger(L,sz.w);
		lua_pushinteger(L,sz.h);
		lua_pushinteger(L,cg.x);
		lua_pushinteger(L,cg.y);
		return 4;
	}
	printf("error! get_image_sizecg %s %d\n",file,frame);
	lua_pushinteger(L,0);
	lua_pushinteger(L,0);
	lua_pushinteger(L,0);
	lua_pushinteger(L,0);
	return 4;
}

static int  lua_create_window (lua_State *L) {
	if (lua_gettop(L) != 8)
	{
		printf("error! lua_create_window  param %d != 8 ",lua_gettop(L));
		return 0;
	}
	const char* name = luaL_checkstring(L,1);
	const char* title = luaL_checkstring(L,2);
	st_window_param st;
	st.l = luaL_checkinteger(L,3);
	st.t = luaL_checkinteger(L,4);
	st.r = luaL_checkinteger(L,5) + st.l;
	st.b = luaL_checkinteger(L,6) + st.t;
	st.style = luaL_checkinteger(L,7);
	st.render_type = luaL_checkstring(L,8);
	init_window(name,title,st);
	return 0;
}

static int  lua_regist_font (lua_State *L) {
	int id = luaL_checkinteger(L,1);
	const char* font = luaL_checkstring(L,2);
	int w = luaL_checkinteger(L,3);
	int h = luaL_checkinteger(L,4);
	int xspace = luaL_checkinteger(L,5);
	int yspace = luaL_checkinteger(L,6);
	bool b = regist_font(id,font,w,h,xspace,yspace);
	lua_pushboolean(L,b);
	return 1;
}

static int  lua_regist_zgp(lua_State *L) {
	const char* file = luaL_checkstring(L,1);
	int dir,frame;
	lua_pushboolean(L,regist_zgp(file,dir,frame));
	lua_pushinteger(L,dir);
	lua_pushinteger(L,frame);
	return 3;
}

static int  lua_regist_image (lua_State *L) {
	const char* file = luaL_checkstring(L,1);
	if (lua_gettop(L) == 1) //regist ´óÍ¼
	{
		int dir,frame;
		lua_pushboolean(L,regist_image_ini(file,dir,frame));
		lua_pushinteger(L,dir);
		lua_pushinteger(L,frame);
		return 3;
	}

	int frame = luaL_checkinteger(L,2);
	st_redirect rd;
	rd.file_image = luaL_checkstring(L,3);
	if (lua_gettop(L) >= 7)
	{
		g_rect& rc = rd.rc;
		rc.l = luaL_checkinteger(L,4);
		rc.t = luaL_checkinteger(L,5);
		rc.r = luaL_checkinteger(L,6);
		rc.b = luaL_checkinteger(L,7);
		if (lua_gettop(L) == 9)
		{
			rd.cg.x = luaL_checkinteger(L,8);
			rd.cg.y = luaL_checkinteger(L,9);
		}
	}
	regist_image_file(file,frame,rd);
	return 0;
}

static int  lua_regist_image_palette (lua_State *L) {
	const char* file = luaL_checkstring(L,1);
	const char* pal = luaL_checkstring(L,2);
	regist_image_palette(file,pal);
	return 0;
}

static int  lua_dump_resource (lua_State *L) {
	const char* type = luaL_checkstring(L,1);
	get_graph()->dump_resource(type);
	return 0;
}

static int  lua_get_graph_trace (lua_State *L) {
	static char buf[1024];
	sprintf(buf,"img:%d tex:%d mul:%d ft:%d zgp:%d, rd:t%d i%d t%d b%d tri:%d",
		counter<image>::s_count_num,
		counter<texture>::s_count_num,
		counter<texture_mul>::s_count_num,
		counter<texture_font>::s_count_num,
		counter<image_zgp>::s_count_num,
		window_render::s_texture_render,
		window_render::s_image_render,
		window_render::s_text_render,
		window_render::s_box_render,
		window_render::s_triangle_render_prev);
	lua_pushstring(L,buf);
	return 1;
}

static int  lua_get_dir(lua_State *L) {
	int x = luaL_checkinteger(L,1);
	int y = luaL_checkinteger(L,2);
	lua_pushinteger(L,math_get_dir8(x,y));
	return 1;
}

static int  lua_find_path(lua_State *L) {
	int x = luaL_checkinteger(L,1);
	int y = luaL_checkinteger(L,2);
	int x1 = luaL_checkinteger(L,3);
	int y1 = luaL_checkinteger(L,4);
	std::list<point2> path;
	if (!get_map()->m_obs->find_path_pixel(point2(x,y),point2(x1,y1),path))
		return 0;
	//push the path.
	lua_newtable(L);
	int i=0;
	for (auto it = path.begin(); it!=path.end(); it++,i++)
	{
		const point2& p = *it;
		lua_pushinteger(L,p.x);
		lua_rawseti(L,-2,i*2+1);
		lua_pushinteger(L,p.y);
		lua_rawseti(L,-2,i*2+2);
	}
	return 1;
}

static int lua_get_input_string(lua_State *L) {
	const char* s = get_input_string();
	lua_pushstring(L,s);
	return 1;	
}

extern bool g_exit;

static int lua_exit(lua_State* L){
	g_exit = true;
	return 0;
}

//static int  lua_create_zgp_pal_hsv(lua_State *L) {
//	const char* zgp = luaL_checkstring(L,1);
//	int part = luaL_checkinteger(L,2);
//	int h = luaL_checkinteger(L,3);
//	int s = luaL_checkinteger(L,4);
//	int v = luaL_checkinteger(L,5);
//	//lua_pushinteger(L,math_get_dir8(x,y));
//
//	image_zgp* zgp = get_graph()->find_zgp(zgp);
//	int index = -1;
//	if (zgp)
//		index = zgp->create_pal_hsv(part,h,s,v);
//	lua_pushinteger(L,index);
//	return 1;
//}

static const luaL_reg driver_lib[] = {
	{"render",				lua_render},
	{"render_texture",		lua_render_texture},

	{"create_window",		lua_create_window},
	{"get_input_string",	lua_get_input_string},
	{"exit",				lua_exit},

	{"get_image_size",		lua_get_image_size},
	{"get_image_sizecg",	lua_get_image_sizecg},
	{"in_image",			lua_in_image},

	{"load_map",			lua_load_map},

	{"get_text_line",		lua_get_text_line},
	{"get_text_size",		lua_get_text_size},

	{"regist_font",			lua_regist_font},
	{"regist_image",		lua_regist_image},
	{"regist_zgp",			lua_regist_zgp},
	{"regist_image_palette",lua_regist_image_palette},

	{"dump_resource",		lua_dump_resource},
	{"get_graph_trace",		lua_get_graph_trace},

	{"get_dir",				lua_get_dir},
	{"find_path",			lua_find_path},

	//{"create_zgp_pal_hsv",	lua_create_zgp_pal_hsv},
	{NULL, NULL},
};


int tolua_driver_open(lua_State* L)
{
	luaL_register(L, "cdriver", driver_lib);
	return 0;
}