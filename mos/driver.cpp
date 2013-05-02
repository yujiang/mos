
#include "driver.h"
#include "graph/cell.h"
#include "graph/graph.h"
#include "graph/image.h"
#include "graph/image_db.h"
#include "graph/font.h"
#include "graph/texture.h"
#include "device/window.h"
#include "device/window_render.h"
#include "mos.h"


static int lua_in_image(lua_State *L) {
	const char* file = luaL_checkstring(L,1);
	int frame = luaL_checkinteger(L,2);
	int x = luaL_checkinteger(L,3);
	int y = luaL_checkinteger(L,4);
	
	image* img = get_graph()->find_image(file,frame);
	if (img)
	{
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
	printf("error! get_image_size %s %d\n",file,frame);
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

static int  lua_regist_image (lua_State *L) {
	const char* file = luaL_checkstring(L,1);
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
	sprintf(buf,"img:%d,tex:%d,rd:%d %d %d %d tri:%d",
		image::s_image_num,
		texture::s_texture_num,
		window_render::s_texture_render,
		window_render::s_image_render,
		window_render::s_text_render,
		window_render::s_box_render,
		window_render::s_triangle_render);
	lua_pushstring(L,buf);
	return 1;
}

static const luaL_reg driver_lib[] = {
	{"render",				lua_render},
	{"create_window",		lua_create_window},
	{"get_image_size",		lua_get_image_size},
	{"get_image_sizecg",	lua_get_image_sizecg},
	{"get_text_line",		lua_get_text_line},
	{"get_text_size",		lua_get_text_size},
	{"in_image",			lua_in_image},
	{"regist_font",			lua_regist_font},
	{"regist_image",		lua_regist_image},
	{"regist_image_palette",lua_regist_image_palette},
	{"dump_resource",		lua_dump_resource},
	{"get_graph_trace",		lua_get_graph_trace},
	{NULL, NULL}
};


int tolua_driver_open(lua_State* L)
{
	luaL_register(L, "cdriver", driver_lib);
	return 0;
}