#ifndef __GRAPH_FONT_H_
#define __GRAPH_FONT_H_

#include <string>

typedef struct FT_FaceRec_*  FT_Face;

struct stFont
{
	FT_Face face;
	char name[64];
	int width,height;
	int x_space,y_space;
	int id;
	int max_width() const;
	int max_height() const;
};

bool init_font();
void close_font();
void dump_font();
const stFont* get_font(int font_id);
bool regist_font(int font_id,const char* name,int width,int height,int xspace,int yspace);

class image;
image* create_font_image(const stFont* font,int char_value,int& advance);

#endif