#ifndef __GRAPH_TEXTURE_H_
#define __GRAPH_TEXTURE_H_

//wrap opengl texture not directx.
class image;
#include <string>

class texture
{
public:
	~texture();
	image* m_image;

	bool create_texture(image* img,const g_rect* rc);
	int draw_cell(const st_cell& cell,const g_rect* rc = NULL);

	unsigned int m_time_use;
	void mark_use(unsigned int time){
		m_time_use = time;
	}

	//const g_rect* m_image_rect;
};

//texture class: 
//texture_whole	美术把ui尽可能的拼到同一个图，还比如说icon
//texture_part	每个part是whole的一个rect
//texture,		比如动画这些不拼的
//texture_font	把所有字合并在同一个贴图，并根据使用的频繁做更新，
//texture_char	类似part，是font的一个rect


#endif