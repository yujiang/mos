#ifndef __GRAPH_IMAGE_H_
#define __GRAPH_IMAGE_H_

#include "rect.h"
class st_cell;
//image of file

class image;
typedef bool (*load_image_func)(image* ,void* ,int );

typedef unsigned char colorbyte;
struct color_palette
{
	colorbyte red;
	colorbyte green;
	colorbyte blue;
};

class image
{
public:
	image();
	~image();
	static int s_image_id ;
	static int s_image_num ;
	int m_id;

	enum enum_createtype{
		image_create_null,
		image_create_file,
		image_create_dynamic,
		image_create_image,
	};

	enum_createtype m_create_type;
	
	static image* create_image_file(const char* file);
	static void register_image_file(const char* fileext,load_image_func func);

	bool create_image_dynamic(int width,int height,int m_bits_component);
	bool create_image_image(const image* i,const g_rect* rc);

	int m_ref;
	int image_add_ref(){
		return ++m_ref;
	}
	int image_release(){
		return --m_ref;
	}

	int m_width,m_height;
	int get_width() const{
		return m_width;
	}
	int get_height() const{
		return m_height;
	}
	g_size get_size() const{
		return g_size(m_width,m_height);
	}
	g_rect get_rect() const{
		return g_rect(0,0,m_width,m_height);
	}

	colorbyte* m_buffer;
	colorbyte* get_buffer() const{
//		if (is_compress())
//			const_cast<image*>(this)->uncompress();
		return m_buffer;
	}

	//bool m_alpha;
	//bool m_premul_alpha;
	//colorbyte* m_buffer;

	int m_pal_alpha_num;
	colorbyte* m_pal_alpha;
	int m_pal_color_num;
	color_palette* m_pal_color;
	bool has_alpha() const {
		return m_bits_pixel == 4 || m_bits_pixel == 1 && m_pal_alpha;
	}
	void set_palette_color(const color_palette* colors,int num_palette);
	void set_palette_alpha(const colorbyte* alphas,int num_palette);

	colorbyte* render_256_argb() const;

	int m_bits_component;
	int m_bits_pixel; //3 or 4 or 1(256 color)

	int get_line_pitch() const{
		return m_width * m_bits_pixel;
	}
	int get_buf_size() const{
		return get_line_pitch() * get_height();
	}
	colorbyte* get_buf_offset(int x,int y) const{
		return get_buffer() + (y * get_line_pitch() + x * m_bits_pixel);
	}

	unsigned int m_time_use;
	void mark_use_image(unsigned int time){
		m_time_use = time;
	}
	bool in_image(int x, int y) ;

public:
	void rgb2bgr();
	void clear(unsigned long color);
	int draw_image_cell(const st_cell& cell,const image* img,const g_rect* rc_img,const g_rect* rc_clip);
	int draw_box_cell(const st_cell& cell,int w,int h);
	int draw_image(int x,int y,int color,int alpha,const image* img,const g_rect* rc_img,const g_rect* rc_clip);
	int draw_box(int x,int y,int color,int alpha,int w,int h);

	int copy_image(int offx,int offy,colorbyte* buf, int w, int h,int line_pitch);

protected:
	void render_image_1_3(int offx,int offy,colorbyte* buf, int w, int h,int line_pitch,int color, int alpha);
	void render_image_3_3(int offx,int offy,colorbyte* buf, int w, int h,int line_pitch,int color, int alpha);
	void render_image_4_3(int offx,int offy,colorbyte* buf, int w, int h,int line_pitch,int color, int alpha);

//public:
//	bool is_compress() const{
//		return m_buffer_compress != 0;
//	}
//	void compress();
//	void uncompress() ;
//	size_t get_compress_size() const{
//		return m_sz_compress;
//	}
//protected:
//	colorbyte* m_buffer_compress;
//	size_t m_sz_compress;
};

bool get_cliped_rect(g_rect& rect,const g_rect& rc,int& offx,int& offy,const g_rect* rc_clip);
bool get_cliped_box(int& offx,int& offy,int& w,int& h, int width,int height);


struct image_ref
{
	image* image;
	g_rect* rc;
};

#endif