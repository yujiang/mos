#ifndef __GRAPH_IMAGE_H_
#define __GRAPH_IMAGE_H_

#include "rect.h"
class st_cell;
//image of file

class image;
typedef bool (*load_image_func)(image* ,void* ,int );

class image
{
public:
	image();
	~image();

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
	int add_ref(){
		return ++m_ref;
	}
	int release(){
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

	unsigned char* m_buffer;
	unsigned char* get_buffer() {
		if (is_compress())
			uncompress();
		return m_buffer;
	}
	//unsigned char* get_buffer_end() const{
	//	return m_buffer + get_buf_size();
	//}

	bool m_alpha;
	bool m_premul_alpha;
	int m_bits_component;

	int m_bits_pixel; //3 or 4

	int get_line_pitch() const{
		return m_width * m_bits_pixel;
	}
	int get_buf_size() const{
		return get_line_pitch() * get_height();
	}
	unsigned char* get_buf_offset(int x,int y) {
		return get_buffer() + (y * get_line_pitch() + x * m_bits_pixel);
	}

	unsigned int m_time_use;
	void mark_use_image(unsigned int time){
		m_time_use = time;
	}
	bool in_image(int x, int y) ;

public:
	void clear(unsigned long color);
	int draw_image(const st_cell& cell,const image* img,const g_rect* rc_img,const g_rect* rc_clip);
	int draw_box(const st_cell& cell,int w,int h);

	int copy_image(int offx,int offy,unsigned char* buf, int w, int h,int line_pitch);

protected:
	//bool create_image_png(void* data,int size);
	//bool create_image_jpg(void* data,int size);

	void render_image_1_3(int offx,int offy,unsigned char* buf, int w, int h,int line_pitch,int color, int alpha);
	void render_image_3_3(int offx,int offy,unsigned char* buf, int w, int h,int line_pitch,int color, int alpha);
	void render_image_4_3(int offx,int offy,unsigned char* buf, int w, int h,int line_pitch,int color, int alpha);

public:
	bool is_compress() const{
		return m_buffer_compress != 0;
	}
	void compress();
	void uncompress();
	size_t get_compress_size() const{
		return m_sz_compress;
	}
protected:
	unsigned char* m_buffer_compress;
	size_t m_sz_compress;
};


#endif