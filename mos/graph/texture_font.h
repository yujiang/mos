#ifndef __GRAPH_TEXTURE_FONT_H_
#define __GRAPH_TEXTURE_FONT_H_

#include "texture.h"

struct texture_char
{
	texture_char(int x,int y,int w,int h):m_time_use(0){
		rc.set_xywh(x,y,w,h);
	}
	g_rect rc;
	int advance;
	//int x,y;
	unsigned int m_time_use;
	void mark_use_char(unsigned int time){
		m_time_use = time;
	}
};

class texture_font;
struct text_char
{
	//int x,y;
	//int w,h;
	//int color;
	//bool underline;
	//bool bold;

	const g_rect* rc_texture;
	texture_font* texture;
	//bool italic  ²»Ö§³Ö¡£
	int advance;
};

#include <unordered_map>
#include <vector>

struct stFont;

class texture_font 
{
public:
	counter<texture_font> m_counter;

	texture* m_texture;
	~texture_font();

	std::unordered_map<int,texture_char*> m_map_char;
	std::vector<texture_char*> m_char_free;

	texture_char* find_char(int char_value);

	bool create_texture_font(int width,int height,const stFont* st_font,bool bold);
	const stFont* m_st_font;
	bool m_bold;

	unsigned int m_time_use;
	void mark_use_texturefont(unsigned int time){
		m_time_use = time;
	}
protected:
	bool create_char(texture_char* tc,int char_value);
};

#endif