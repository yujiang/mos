#include "font.h"
#include <unordered_map>
#include <assert.h>
#include "image.h"
#include "device/file.h"
#include <algorithm>
#include <iostream>

#include <freetype/ft2build.h>
#include FT_FREETYPE_H

int stFont::max_width() const
{
	return width + 4;
}

int stFont::max_height() const
{
	return height + 4;
}


FT_Library  library;
std::unordered_map<int,stFont> g_font_types;

const stFont* get_font(int font_id)
{
	if (font_id >= 1000)
		font_id -= 1000;
	auto it = g_font_types.find(font_id);
	if (it == g_font_types.end())
	{
		printf("font.cpp get_font not find font %d\n",font_id);
		return get_font(0);
	}
	return &it->second;
}

bool regist_font(int font_id,const char* name,int width,int height,int xspace,int yspace)
{
	if (g_font_types.find(font_id) != g_font_types.end())
		return false;

	FT_Face face;
	int error;
	error =  FT_New_Face( library,get_resourcefile(name),0,&face);
	if (error)
	{
		//find in windows font
		error =  FT_New_Face( library,get_windows_font(name),0,&face);
		if (error)
		{
			assert(0);
			return false;
		}
	}
	error = FT_Set_Pixel_Sizes(face,width,height);
	if (error)
	{
		assert(0);
		return false;
	}

	stFont& font = g_font_types[font_id];
	font.face = face;
	strcpy_s(font.name,name);
	font.width = width;
	font.height = height;
	font.id = font_id;
	font.x_space = xspace;
	font.y_space = yspace;
	
	return true;
}

//use freetype to draw face to a image.
image* create_font_image(const stFont* font,int char_value,int& advance)
{
	FT_Face face = font->face;
	int error = FT_Load_Char( face, char_value, FT_LOAD_RENDER | FT_LOAD_NO_BITMAP);
	if (error)
		return NULL;

	FT_GlyphSlot glyph = face->glyph;
	advance = glyph->advance.x >> 6;
	FT_Bitmap bitmap = glyph->bitmap;
	
	image* img = new image;
	img->create_image_dynamic(font->max_width(),font->max_height(),1);
	img->clear(0);

	if (bitmap.pixel_mode == FT_PIXEL_MODE_GRAY)
	{
		img->copy_image(
			glyph->bitmap_left,
			font->height - glyph->bitmap_top,
			bitmap.buffer,
			bitmap.width,bitmap.rows,
			bitmap.pitch);
	}
	else if (bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
	{
		assert(0);	
	}

	return img;
}

bool init_font()
{
	int error = FT_Init_FreeType( &library );
	if ( error )
		return false;	
	return true;
}

void close_font()
{
	//should free.
	std::for_each(g_font_types.begin(),g_font_types.end(),
		[](const std::pair<int,stFont>& it) {FT_Done_Face(it.second.face);});
	FT_Done_FreeType(library);
}

void dump_font()
{
	for (auto it = g_font_types.begin(); it != g_font_types.end(); ++it)	
	{
		const stFont& st = it->second;		
		std::cout << it->first << " " << st.name << std::endl;
	}
}
