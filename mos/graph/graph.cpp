#include "graph.h"
#include "cell.h"
#include "texture.h"
#include "texture_font.h"
#include "image.h"
#include "device/window_render.h"
#include "device/file.h"
#include "core/utf8.h"
#include "font.h"
#include "image_db.h"
#include "mos.h"
#include "map/map.h"
#include "core/dir32.h"
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
graph::graph()
{
	m_compress_image = 10;
	m_clear_image  = 60;
	m_clear_texture = 10;
	m_clear_texturefont = 10;

}

void graph::regist_file_source(file_source* source)
{
	source_map[source->get_file_ext()] = source;
}

file_source* graph::find_file_source(const char* file) const
{
	auto it = source_map.find(get_file_ext(file));
	if (it != source_map.end())
		return it->second;
	//return source_map[f];
	return NULL;
}

void graph::maped_image(const char* file,image* img)
{
	img->image_add_ref();
	image_map[file] = img;
}

image* graph::find_image_raw(const char* file,int frame,const unsigned long* parts_pal_hsv)
{
	file_source* source = find_file_source(file);
	if (source)
		return source->find_image_file(file,frame,parts_pal_hsv);
	//if (is_zgp(file))
	//	return find_image_zgp(file,frame,parts_pal_hsv);

	image* i = image_map[file];
	if (!i)
	{
		i = image::create_image_file(file);
		if (i)
			image_map[file] = i;
	}
	if (i)
		i->mark_use_image(g_time_now);
	return i;
}

image* graph::find_image(const char* file,int frame,const unsigned long* parts_pal_hsv)
{
	const st_redirect* r = redirect_image_file(file,frame);
	if (r)
		file = r->file_image.c_str();
	return find_image_raw(file,frame,parts_pal_hsv);
}

bool graph::get_image_size(const char* file,int frame,g_size& sz)
{
	g_point cg;
	return get_image_sizecg(file,frame,sz,cg);
}

bool graph::get_image_sizecg(const char* file,int frame,g_size& sz,g_point& cg)
{
	const st_redirect* r = redirect_image_file(file,frame);
	if (r && !r->rc.is_empty())
	{
		sz = r->rc.get_size();
		cg = r->cg;
		return true;
	}
	if (r)
		file = r->file_image.c_str();

	image* i = find_image_raw(file,frame,NULL);
	if (i)
	{
		sz = i->get_size();
		cg = i->get_cg();
		return true;
	}
	return false;
}

void graph::maped_texture(const char* file,texture* p)
{
	p->mark_use_texture(g_time_now);
	texture_map[file] = p;
	p->m_name = file;
}

texture* graph::find_texturemap_index(int index)
{
	if (index < 0 || texture_map.size() <= index)
		return 0;
	auto it = texture_map.begin();
	for (int i=0; i<index; i++,++it);
	return it->second;
}

texture* graph::find_texturemul_index(int index)
{
	if (index < 0 || texture_muls.size() <= index)
		return 0;
	return texture_muls[index]->m_texture;
}


void graph::maped_texturesub(const char* file,texture_sub* p)
{
	p->mark_use_texture(g_time_now);
	texturesub_map[file] = p;
	p->m_name = file;
}

texture_sub* graph::find_texturesub(const char* file)
{
	texture_sub* t = texturesub_map[file];
	if (t)
		t->mark_use_texture(g_time_now);
	return t;
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
	texture_font* tf = find_texture_font(text.font,text.bold != 0);
	if (tf)
	{
		tf->mark_use_texturefont(g_time_now);
		texture_char* t = tf->find_char(char_value);
		if (t)
		{
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
void graph::auto_clear_resource()
{
	for (auto it = source_map.begin(); it != source_map.end(); ++it)
	{
		file_source* p = it->second;
		p->auto_clear_resource();
	}

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
			++it;
//经过测试，压缩没啥用。
//		else 
//		{
//			if (img && TIME_NOTUSE(img,m_compress_image) && !img->is_compress())			
//			{
//				img->compress();
//#ifdef _DEBUG_RESOURCE
//				std::cout << "compress image " << it->first << " time: " << TIME(img) 
//					<< " size " << img->get_buf_size()/1024 << "->" << img->get_compress_size()/1024 << std::endl;
//#endif
//			}			
//			++it;
//		}
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
			texture_char* tc = it2->second;
			if (TIME_NOTUSE(tc,m_clear_texturefont))
			{
#ifdef _DEBUG_RESOURCE
				std::cout << "clear font " << it->first << " char " << core::UnicodeCharToANSI(it2->first) << " time: " << TIME(tc) << std::endl;
#endif
				tf->m_char_free.push_back(tc);
				it2 = tf->m_map_char.erase(it2);
			}
			else 
				++it2;
		}
	}

	for (auto it = texturesub_map.begin(); it != texturesub_map.end();)
	{
		texture_sub* tex = it->second;
		if (tex && TIME_NOTUSE(tex,m_clear_texture))
		{
#ifdef _DEBUG_RESOURCE
			std::cout << "clear texturesub " << it->first << " time: " << TIME(tex) << std::endl;
#endif
			tex->m_tex->released_sub(tex);
			it = texturesub_map.erase(it);
		}
		else 
			++it;
	}	
	for (auto it = texture_muls.begin(); it != texture_muls.end();)
	{
		texture_mul* tex = *it;
		if (tex && TIME_NOTUSE(tex->m_texture,m_clear_texture))
		{
#ifdef _DEBUG_RESOURCE
			std::cout << "clear texture_mul " << it->first << " time: " << TIME(tex) << std::endl;
#endif
			delete tex;
			it = texture_muls.erase(it);
		}
		else 
			++it;
	}	
}

void graph::close_resource()
{
	for (auto it = source_map.begin(); it != source_map.end(); ++it)
	{
		file_source* p = it->second;
		p->close_resource();
	}

	for (auto it = image_map.begin(); it != image_map.end(); ++it)
		delete it->second;
	image_map.clear();
	for (auto it = texture_map.begin(); it != texture_map.end(); ++it)
		delete it->second;
	texture_map.clear();
	for (auto it = texture_font_map.begin(); it != texture_font_map.end(); ++it)
		delete it->second;
	texture_font_map.clear();

	for (auto it = texture_muls.begin(); it != texture_muls.end();++it)
		delete *it;
	texture_muls.clear();
	texturesub_map.clear();
}

//why not image:dump 因为也只有这里用，写到类里没意思。
void graph::dump_resource(const std::string& type) const
{
	std::cout << "\n"__FUNCTION__" " << type << std::endl;
	unsigned long time = get_time_now();

	file_source* source = find_file_source(type.c_str());
	if (source)
		source->dump_resource();

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
int graph::draw_image(const st_cell& cell,const char* file,int frame)
{
	const st_redirect* r = redirect_image_file(file,frame);
	if (r)
		file = r->file_image.c_str();

	image* img = find_image_raw(file,frame,&cell.part0);
	if (!img)
		return -1;

	const g_rect* rc = NULL;
	if (r && !r->rc.is_empty())
		rc = &r->rc;

	file_source* source = find_file_source(file);
	if (source)
		file = source->get_texture_file(file,frame,&cell.part0);

	return get_render()->draw_image_cell(cell,img,file,rc);
}

int graph::draw_box(const st_cell& cell,int w,int h)
{
	return get_render()->draw_box_cell(cell,w,h);
}

int graph::draw_texture(const st_cell& cell,const char* file)
{
	texture* _tex = find_texture(file);
	if (!_tex)
		return 0;
	return get_render()->draw_texture_cell(cell,_tex,NULL);
}

int graph::draw_texturemap_index(const st_cell& cell,int index)
{
	texture* _tex = find_texturemap_index(index);
	if (!_tex)
		return 0;
	return get_render()->draw_texture_cell(cell,_tex,NULL);
}

int graph::draw_texturemul_index(const st_cell& cell,int index)
{
	texture* _tex = find_texturemul_index(index);
	if (!_tex)
		return 0;
	return get_render()->draw_texture_cell(cell,_tex,NULL);
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

	auto_free af(core::UTF8ToUnicode(text.text));
	if (af.ptr == 0)
		return g_size(0,0);
	g_size sz = get_text_size(text,sz_father,font,af.ptr);
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

	auto_free af(core::UTF8ToUnicode(text.text));
	if (af.ptr == 0)
		return 0;
	int line = get_text_line(text,sz_father,font,af.ptr);
	return line;
}

int graph::draw_text(const st_cell& cell,const st_cell& text,const g_rect& rc_father)
{
	const stFont* font = get_font(text.font);
	if (!font)
		return -1;

	get_render()->text_start(rc_father);
	st_cell c = cell;

	auto_free af(core::UTF8ToUnicode(text.text));
	if (af.ptr == 0)
		return -1;
	const wchar_t* str = af.ptr;

	text_char tc;

	int x = 0;
	int y = 0;
	float room = cell.room;

	int ch;
	while(ch = *str++)
	{
		//if tc.char_value == #... 如果是转义字符，那么变了。
		if (ch == '\n')
		{
			x = 0;
			y += font->height + font->y_space ;
			continue;
		}

		if (!find_texture_font_rc(text,ch,tc))
			continue;

		c.x = cell.x + x*room;
		c.y = cell.y + y*room;
		c.color = text.color;
		c.alpha = text.alpha;
		get_render()->draw_text_cell(c,tc.texture->m_texture,tc.rc_texture);

		x += tc.advance;
	}

	get_render()->text_end();
	return 0;
}

//////////////////////////////////////////////////////////////////////////
//loop

void graph::draw_win_begin(int x,int y,int w,int h,const st_cell& win)
{
	get_render()->window_start(x,y,w,h,win);
}

void graph::draw_win_end()
{
	get_render()->window_end();
}

//////////////////////////////////////////////////////////////////////////
//device
bool create_image_png(image_struct*,void* data,int size,const char* name);
bool create_image_jpg(image_struct*,void* data,int size,const char* name);

void graph::init_graph()
{
	init_dir32();

	init_cell();
	init_font();
	image::register_image_file("png",create_image_png);
	image::register_image_file("jpg",create_image_jpg);

	get_map()->init_map();
	//image::register_image_file("zgp",create_image_zgp);
	//简单的文件可以用register_image_file
	//复杂的多帧文件必须用regist_file_source
}

void graph::close_graph()
{
	get_map()->destroy_map();

	close_resource();
	close_font();
}

graph* get_graph()
{
	static graph s_graph;
	return &s_graph;
}

