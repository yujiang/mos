#ifndef __GRAPH_TEXTURE_H_
#define __GRAPH_TEXTURE_H_

//wrap opengl texture not directx.
struct g_rect;
class st_cell;

class texture
{
public:
	virtual void set_alpha(bool alpha) = 0;
	virtual bool create_texture_dynamic(int width,int height,int m_bits_component) = 0;
	virtual int draw_image(const st_cell& cell,const image* img,const g_rect* rc_img,const g_rect* rc_clip) = 0;
	virtual bool create_texture(image* img,const g_rect* rc) = 0;

	int draw_cell(const st_cell& cell,const g_rect* rc = 0);

	unsigned int m_time_use;
	void mark_use_texture(unsigned int time){
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