#include "graph.h"
#include "cell.h"
#include "texture.h"
#include "texture_font.h"
#include "image.h"
#include "device/window_render.h"
#include "core/utf8.h"
#include "font.h"
#include "image_db.h"
#include "mos.h"
#include <iostream>

class auto_free
{
public:
	wchar_t* ptr;
	auto_free(wchar_t* buf){
		ptr = buf;
	}
	~auto_free(){
		delete ptr;
	}

};

//////////////////////////////////////////////////////////////////////////
image* graph::find_image_raw(const char* file)
{
	image* i = image_map[file];
	if (i)
	{
		i->mark_use_image(g_time_now);
		return i;
	}
	i = image::create_image_file(file);
	if (i)
	{
		image_map[file] = i;
		i->mark_use_image(g_time_now);
		return i;
	}
	return NULL;
}

image* graph::find_image(const char* file,int frame)
{
	const st_redirect* r = redirect_image_file(file,frame);
	if (r)
		file = r->file_image.c_str();
	return find_image_raw(file);
}

bool graph::get_image_size(const char* file,int frame,g_size& sz)
{
	const st_redirect* r = redirect_image_file(file,frame);
	if (r && !r->rc.is_empty())
	{
		sz = r->rc.get_size();
		return true;
	}
	if (r)
		file = r->file_image.c_str();
	
	image* i = find_image_raw(file);
	if (i)
	{
		sz = i->get_size();
		return true;
	}
	return false;
}

texture* graph::find_texture(const char* file)
{
	texture* t = texture_map[file];
	if (t)
		t->mark_use_texture(g_time_now);
	return t;
}


texture_font* graph::find_texture_font(int font,bool bold)
{
	if (bold)
		font += 1000;
	texture_font* tf = texture_font_map[font];
	if (tf)
		return tf;

	const stFont* st = get_font(font);
	if (!st)
		return NULL;

	tf = new texture_font;
	tf->create_texture_font(512,512,st,bold);
	texture_font_map[font] = tf;
	return tf;
}

bool graph::find_texture_font_rc(const st_cell& text,int char_value,text_char& tc)
{
	texture_font* tf = find_texture_font(text.font,text.bold);
	if (tf)
	{
		texture_char* t = tf->find_char(char_value);
		if (t)
		{
			tf->mark_use_texturefont(g_time_now);
			t->mark_use_char(g_time_now);
			tc.rc_texture = &t->rc;
			tc.advance = t->advance;
			tc.texture = tf;
			return true;
		}
	}
	return false;
}


//////////////////////////////////////////////////////////////////////////
#define TIME_NOTUSE(A,B) ((int)(time - A->m_time_use) / 1000 > B)
#define TIME(A) ((time - A->m_time_use) / 1000)



void graph::auto_clear_resource()
{
	unsigned long time = get_time_now();

	for (auto it = image_map.begin(); it != image_map.end(); )
	{
		image* img = it->second;
		if (img && TIME_NOTUSE(img,m_clear_image) && img->m_ref == 1)
		{
#ifdef _DEBUG_RESOURCE
			std::cout << "clear image " << it->first << " time: " << TIME(img) << std::endl;
#endif
			delete img;
			it = image_map.erase(it);
		}
		else 
		{
			if (img && TIME_NOTUSE(img,m_compress_image) && !img->is_compress())			
			{
				img->compress();
#ifdef _DEBUG_RESOURCE
				std::cout << "compress image " << it->first << " time: " << TIME(img) 
					<< " size " << img->get_buf_size()/1024 << "->" << img->get_compress_size()/1024 << std::endl;
#endif
			}			
			++it;
		}
	}	
	for (auto it = texture_map.begin(); it != texture_map.end();)
	{
		const texture* tex = it->second;
		if (tex && TIME_NOTUSE(tex,m_clear_texture))
		{
#ifdef _DEBUG_RESOURCE
			std::cout << "clear texture " << it->first << " time: " << TIME(tex) << std::endl;
#endif
			delete tex;
			it = texture_map.erase(it);
		}
		else 
			++it;
	}	
	for (auto it = texture_font_map.begin(); it != texture_font_map.end(); ++it)
	{
		texture_font* tf = it->second;
		for (auto it2 = tf->m_map_char.begin(); it2 != tf->m_map_char.end(); )
		{
			const texture_char* tc = it2->second;
			if (TIME_NOTUSE(tc,m_clear_texturefont))
			{
#ifdef _DEBUG_RESOURCE
				std::cout << "clear font " << it->first << " char " << core::UnicodeCharToANSI(it2->first) << " time: " << TIME(tc) << std::endl;
#endif
				delete tc;
				it2 = tf->m_map_char.erase(it2);
			}
			else 
				++it2;
		}
	}
}

void graph::close_resource()
{
	for (auto it = image_map.begin(); it != image_map.end(); ++it)
		delete it->second;
	image_map.clear();
	for (auto it = texture_map.begin(); it != texture_map.end(); ++it)
		delete it->second;
	texture_map.clear();
	for (auto it = texture_font_map.begin(); it != texture_font_map.end(); ++it)
		delete it->second;
	texture_font_map.clear();
}

//why not image:dump 因为也只有这里用，写到类里没意思。
void graph::dump_resource(const std::string& type) const
{
	std::cout << "\n"__FUNCTION__" " << type << std::endl;
	unsigned long time = get_time_now();

	if (type == "all")
	{
		dump_resource("image");
		dump_resource("texture");
		dump_resource("texture_font");	
		dump_resource("font");
	}
	else if (type == "image")
	{
		for (auto it = image_map.begin(); it != image_map.end(); ++it)
		{
			const image* img = it->second;
			std::cout << it->first << " ref: " << img->m_ref << " time: " << TIME(img) << std::endl;
		}	
	}
	else if (type == "texture")
	{
		for (auto it = texture_map.begin(); it != texture_map.end(); ++it)
		{
			const texture* tex = it->second;
			std::cout << it->first << " time: " << TIME(tex) << std::endl;
		}	
	}
	else if (type == "texture_font")
	{
		for (auto it = texture_font_map.begin(); it != texture_font_map.end(); ++it)
		{
			const texture_font* tf = it->second;
			std::cout << it->first << " time: " << TIME(tf) << std::endl;
			//std::unordered_map<int,texture_char*> m_map_char;
			for (auto it2 = tf->m_map_char.begin(); it2 != tf->m_map_char.end(); ++it2)
			{
				const texture_char* tc = it2->second;
				std::cout << "\t" << core::UnicodeCharToANSI(it2->first) << " time: " << TIME(tc) << std::endl;
			}
		}
	}
	else if (type == "font")
	{
		dump_font();
	}
}

//////////////////////////////////////////////////////////////////////////
int graph::draw_image(const st_cell& cell,const char* file0,int frame)
{
	const char* file = file0;
	const st_redirect* r = redirect_image_file(file,frame);
	if (r)
		file = r->file_texture.c_str();

	texture* t = find_texture(file);
	if (!t)
	{
		image* img = find_image(file0,frame);
		if (!img)
			return -1;

		const g_rect* rc = NULL;
		if (r && !r->rc.is_empty())
			rc = &r->rc;

		t = get_render()->create_texture();
		if (!t->create_texture(img,rc))
		{
			delete t;
			return -1;
		}

		t->mark_use_texture(g_time_now);
		texture_map[file] = t;
	}
	return t->draw_cell(cell,0);
}

int graph::draw_box(const st_cell& cell,int w,int h)
{
	return get_render()->draw_box_cell(cell,w,h);
}

//////////////////////////////////////////////////////////////////////////
//text
static bool is_english(short c)
{
	return c <= 255;
}

void format_text(const st_cell& text,std::vector<text_char>& texts)
{
	//考虑到align和自动换行，和标点，这不是件容易的事情。
	//加上#x改变颜色，bold和下划线，更复杂。

	//text转为utf16
}

//no wrap now
g_size graph::get_text_size(const st_cell& text,const g_size& sz_father, const stFont* font, const wchar_t* str)
{
	text_char tc;

	int x = 0;
	int y = 0;

	int line = 1;
	int maxx = 0;

	int ch;
	while(ch = *str++)
	{
		//if tc.char_value == #... 如果是转义字符，那么变了。
		if (ch == '\n')
		{
			x = 0;
			y += font->height + font->y_space;
			line ++;
			continue;
		}

		if (!find_texture_font_rc(text,ch,tc))
			continue;
		x += tc.advance;
		if (maxx < x)
			maxx = x;
	}
	return g_size(maxx, line * (font->height + font->y_space) - font->y_space) ;
}

g_size graph::get_text_size(const st_cell& text,const g_size& sz_father)
{
	const stFont* font = get_font(text.font);
	if (!font)
		return g_size(0,0);

	//const wchar_t* str0 = core::UTF8ToUnicode(text.text);
	auto_free af(core::UTF8ToUnicode(text.text));
	if (af.ptr == 0)
		return g_size(0,0);
	//const wchar_t* str0 = core::ANSIToUnicode("a你b好");
	g_size sz = get_text_size(text,sz_father,font,af.ptr);
	//free((void*)str0);
	return sz;
}

//no wrap now
int graph::get_text_line(const st_cell& text,const g_size& sz_father, const stFont* font, const wchar_t* str)
{
	text_char tc;

	int x = 0;
	int y = 0;

	int line = 1;
	int ch;
	while(ch = *str++)
	{
		//if tc.char_value == #... 如果是转义字符，那么变了。
		if (ch == '\n')
		{
			x = 0;
			y += font->height + font->y_space;
			line ++;
			continue;
		}

		if (!find_texture_font_rc(text,ch,tc))
			continue;
		x += tc.advance;
	}
	return line;
}

int graph::get_text_line(const st_cell& text,const g_size& sz_father)
{
	const stFont* font = get_font(text.font);
	if (!font)
		return 0;

	//const wchar_t* str0 = core::UTF8ToUnicode(text.text);
	auto_free af(core::UTF8ToUnicode(text.text));
	if (af.ptr == 0)
		return 0;
	//const wchar_t* str0 = core::ANSIToUnicode("a你b好");
	int line = get_text_line(text,sz_father,font,af.ptr);
	//free((void*)str0);
	return line;
}

int graph::draw_text(const st_cell& cell,const st_cell& text,const g_rect& rc_father)
{
	const stFont* font = get_font(text.font);
	if (!font)
		return -1;

	get_render()->text_start(rc_father);
	st_cell c = cell;

	//const wchar_t* str0 = core::UTF8ToUnicode(text.text);
	auto_free af(core::UTF8ToUnicode(text.text));
	if (af.ptr == 0)
		return -1;
	//const wchar_t* str0 = core::ANSIToUnicode("a你b好");
	const wchar_t* str = af.ptr;

	text_char tc;

	int x = 0;
	int y = 0;

	int ch;
	while(ch = *str++)
	{
		//if tc.char_value == #... 如果是转义字符，那么变了。
		if (ch == '\n')
		{
			x = 0;
			y += font->height + font->y_space;
			continue;
		}

		if (!find_texture_font_rc(text,ch,tc))
			continue;

		c.x = cell.x + x;
		c.y = cell.y + y;
		c.color = text.color;
		c.alpha = text.alpha;
		tc.texture->m_texture->draw_cell(c,tc.rc_texture);

		x += tc.advance;
	}

	get_render()->text_end();
	//free((void*)str0);
	return 0;
}

//////////////////////////////////////////////////////////////////////////
//loop
void graph::render_begin()
{
}

void graph::render_end()
{
	auto_clear_resource();
}

void graph::draw_win_begin(int x,int y,int w,int h)
{
	get_render()->window_start(x,y,w,h);
}

void graph::draw_win_end()
{
	get_render()->window_end();
}


//////////////////////////////////////////////////////////////////////////
//device
//HDC graph::get_dc()
//{
//	return GetDC(m_hWnd);
//}

bool create_image_png(image*,void* data,int size);
bool create_image_jpg(image*,void* data,int size);

void graph::init_graph()
{
	init_cell();
	init_font();
	image::register_image_file("png",create_image_png);
	image::register_image_file("jpg",create_image_jpg);
}

void graph::close_graph()
{
	close_resource();
	close_font();
}

graph* get_graph()
{
	static graph s_graph;
	return &s_graph;
}

