#include "map.h"
#include "graph.h"
#include "../device/file.h"

graph_map* get_map()
{
	static graph_map s_map;
	return &s_map;
}

void graph_map::draw_map_begin(int x,int y,int w,int h,const st_cell& map)
{
	m_rc_map.set_xywh(x,y,w,h);
	m_in_map = &map;
	m_source->draw_map_begin();
}

void graph_map::draw_map_end()
{
	m_source->draw_map_end();
	m_in_map = NULL;
}

graph_map::graph_map()
{
	m_source = NULL;
}

graph_map::~graph_map()
{
	destroy_map();
}

void graph_map::destroy_map()
{
	if (m_source)
	{
		m_source->destory();
		m_source = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
void graph_map::draw_map_image(const st_cell& cell, const char* map_file,int frame)
{
	m_source->draw_map_image(cell,map_file,frame);
}

bool graph_map::load_map(const char* file,int frame,g_size& sz)
{
	if (m_source)
		m_source->destory();
	m_source = s_mapLoad[get_file_ext(file)]();
	return m_source->load_map(file,frame,sz);
}

void graph_map::register_map_source(const char* fileext,map_source_func func)
{
	s_mapLoad[fileext] = func;
}


//////////////////////////////////////////////////////////////////////////
class map_jpg : public map_source
{
public:
	void draw_map_image(const st_cell& cell, const char* map_file,int frame){
		get_graph()->draw_image(cell,map_file,frame);
	}
	bool load_map(const char* file,int frame,g_size& sz){
		return get_graph()->get_image_size(file,frame,sz);
	}
	void destory(){
		delete this;
	}
	void draw_map_begin() {};
	void draw_map_end() {};
};

map_source* create_map_jpg()
{
	return new map_jpg;
}

void graph_map::init_map()
{
	register_map_source("jpg",&create_map_jpg);
	register_map_source("png",&create_map_jpg);
}