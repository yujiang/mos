#include "graph.h"
#include "texture_font.h"
#include "graph.h"
#include "cell.h"
#include "image.h"
#include "device/window.h"
#include "font.h"
#include "device/window_render.h"
#include "mos.h"
#include <assert.h>

DECLARE_COUNTER(texture_font)

texture_font::~texture_font()
{
	delete m_texture;
	m_texture = NULL;
	for (auto it = m_char_free.begin(); it != m_char_free.end(); ++it)
		delete *it;
	m_char_free.clear();
	for (auto it = m_map_char.begin(); it != m_map_char.end(); ++it)
		delete it->second;
	m_map_char.clear();
}

bool texture_font::create_texture_font(int width,int height,const stFont* st_font,bool bold)
{
	m_st_font = st_font;
	m_bold = bold;

	m_texture = get_render()->create_texture();
	m_texture->create_texture_dynamic(width,height,kCCTexture2DPixelFormat_A8);

	int maxw = st_font->max_width();
	int maxh = st_font->max_height();
	int col = width / maxw;
	int line = height / maxh;

	//printf(__FUNCTION__" w %d h %d fw %d fh %d\n",width,height,st_font->max_width(),st_font->max_height());

	for (int y = 0; y<line; y++)
	for (int x = 0; x<col; x++)
	{
		texture_char* p = new texture_char(x*maxw,y*maxh,maxw,maxh);
		m_char_free.push_back(p);
	}
	m_free_num = m_char_free.size();

	return true;
}

texture_char* texture_font::find_char(int char_value)
{
	auto it = m_map_char.find(char_value);
	if (it != m_map_char.end())
		return it->second;

	if (m_char_free.empty())
		return NULL;
	
	texture_char* p = m_char_free.back();
	
	if (!create_char(p,char_value))
		return NULL;

	m_char_free.pop_back();
	m_map_char[char_value] = p;

	assert(m_map_char.size() + m_char_free.size() == m_free_num);
	
	return p;
}

bool texture_font::create_char(texture_char* tc,int char_value)
{
	image* img = create_font_image(m_st_font,char_value,tc->advance);
	if (!img)
		return false;
	m_texture->draw_image_ontexture(tc->rc.l,tc->rc.t,img);

	delete img;
	return true;
}

