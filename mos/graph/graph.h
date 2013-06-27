#ifndef __GRAPH_GRAPH_H_
#define __GRAPH_GRAPH_H_

#include "rect.h"
#include <unordered_map>
#include <string>

//file可能不是实际的file，可以是一个key
//根据file和frame来定位texture(file and rect)
//button.jpg 1 --> ui.jpg ui (200,300,64,32) true ui为了drawbatch，所以true
//walk.ani 2 -->walk.jpg walk2 (200,300,64,32) false walk下一帧这一帧就没用了，所以false
class texture;
class image;
class st_cell;
struct stFont;
class texture_font;
struct text_char;
class image_zgp;

class graph
{
public:
	graph(){
		m_compress_image = 10;
		m_clear_image  = 60;
		m_clear_zgp = 100;
		m_clear_texture = 10;
		m_clear_texturefont = 10;
	}
	//所有zgp资源。
	std::unordered_map<std::string,image_zgp*> zgp_map;

	//所有资源。
	std::unordered_map<std::string,image*> image_map;
	image* find_image(const char* file,int frame);
	
	bool get_image_size(const char* file,int frame,g_size& sz);
	bool get_image_sizecg(const char* file,int frame,g_size& sz,g_point& cg);

	std::unordered_map<std::string,texture*> texture_map;
	texture* find_texture(const char* file,int frame);

	std::unordered_map<int,texture_font*> texture_font_map;

	//clear 
	int m_compress_image,m_clear_zgp,m_clear_image,m_clear_texture,m_clear_texturefont;
	void auto_clear_resource();
	void close_resource();
	void dump_resource(const std::string& type) const;

	//绘制
	int draw_image(const st_cell& cell,const char* file,int frame);
	int draw_box(const st_cell& cell,int w,int h);

	//text
	int draw_text(const st_cell& cell,const st_cell& text,const g_rect& rc_father);
	//传入sz_father是因为可能要wrap
	int get_text_line(const st_cell& text,const g_size& sz_father);
	g_size get_text_size(const st_cell& text,const g_size& sz_father);

	//loop
	void render_begin();
	void render_end();
	
	void draw_win_begin(int x,int y,int w,int h);
	void draw_win_end();

	//device
	void init_graph();
	void close_graph();
protected:
	int get_text_line(const st_cell& text,const g_size& sz_father, const stFont* font, const wchar_t* str);
	g_size get_text_size(const st_cell& text,const g_size& sz_father, const stFont* font, const wchar_t* str);

	image* find_image_raw(const char* file,int frame);
	image* find_image_zgp(const char* file,int frame);
	image* create_image_zgp(const char* file,int frame);

	texture_font* find_texture_font(int font,bool bold);
	bool find_texture_font_rc(const st_cell& text,int char_value,text_char& tc);
public:
	texture* find_texture(const char* file);
	void maped_texture(const char* file,texture* p);
};

graph* get_graph();

#endif