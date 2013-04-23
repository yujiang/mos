#include "image.h"
#include <string.h>
#include "device/file.h"
#include "color.h"
#include "cell.h"
#include "assert.h"
#include "zlib/zlib.h"
#include <map>
#include <string>

image::image()
{
	m_width = m_height = 0;
	m_buffer = 0;
	m_ref = 1;
	
	m_buffer_compress = 0;
	m_sz_compress = 0;

	m_create_type = image_create_null;
}	

image::~image()
{
	delete m_buffer;
	m_buffer = 0;

	delete m_buffer_compress;
	m_buffer_compress = 0;
}

bool image::in_image(int x, int y) 
{
	if (x >= 0 && x<m_width && y >= 0 && y<m_height)
	{
		if (!m_alpha)
			return true;
		unsigned char* des = get_buf_offset(x,y);
		unsigned char a = *(des + 3);
		return a > 127;
	}
	return false;
}

std::map<std::string,load_image_func> g_imageLoad;
void image::register_image_file(const char* fileext,load_image_func func)
{
	g_imageLoad[fileext] = func;
}

image* image::create_image_file(const char* file)
{
	//m_create_type = image_create_file;
	size_t sz;
	char* buf = read_imagefile(file,sz);
	if (!buf)
		return false;
	int len = strlen(file);
	if (len <= 3)
		return false;
	const char* ext = file+len-3;
	load_image_func func = g_imageLoad[ext];
	if (!func)
		return false;
	image* i = new image();
	if (func(i,buf,sz))
	{
		i->m_create_type = image_create_file;
		return i;
	}
	else 
	{
		delete i;
		return NULL;
	}
}

bool image::create_image_dynamic(int width,int height,int bits)
{
	m_create_type = image_create_dynamic;
	m_width = width;
	m_height = height;
	m_bits_pixel = bits;
	m_bits_component = 8;
	m_premul_alpha = false;
	m_alpha = false;
	m_buffer = new unsigned char[get_buf_size()];
	return true;
}

bool image::create_image_image(const image* i,const g_rect* rc)
{
	m_create_type = image_create_image;
	int w,h;
	if (!rc)
	{
		w = i->get_width();
		h = i->get_height();
	}
	else
	{
		w = rc->width();
		h = rc->height();
	}
	create_image_dynamic(w,h,i->m_bits_pixel);
	m_premul_alpha = i->m_premul_alpha;
	m_alpha = i->m_alpha;
	if (!rc)
	{
		int size = get_buf_size();
		memcpy(m_buffer,i->m_buffer,size);
	}
	else
	{
		//st_cell cell;
		draw_image(0,0,-1,255,i,rc,NULL);
	}
	return true;
}

void image::clear(unsigned long color)
{
	unsigned char a,r,g,b;
	G_GET_ARGB(color,a,r,g,b);
	if (r == g && g == b)
		memset(m_buffer,r,get_buf_size());
	else
	{
		if (m_bits_component == 3)
		{
			unsigned char* p = m_buffer;
			for (int i=0; i<m_width*m_height; ++i)
			{
				*p ++ = b;
				*p ++ = g;
				*p ++ = r;
			}
		}
	}
}

int image::copy_image(int offx,int offy,unsigned char* buf, int w,int h,int line_pitch)
{
	assert(offx >= 0 && offy >= 0 && offx + w <= m_width && offy + h <= m_height);
	unsigned char* src = buf;
	unsigned char* des = get_buf_offset(offx,offy);

	for (int y=0; y<h; y++)
	{
		memcpy(des,src,w*m_bits_pixel);
		src += line_pitch;
		des += get_line_pitch();
	}
	return 0;
}

void image::render_image_1_3(int offx,int offy,unsigned char* buf, int w, int h,int line_pitch,int color,int alpha)
{
	assert(offx >= 0 && offy >= 0 && offx + w <= m_width && offy + h <= m_height);
	unsigned char* src = buf;
	unsigned char* des = get_buf_offset(offx,offy);

	unsigned char r,g,b,a;
	G_GET_ARGB(color,a,r,g,b);
	for (int y=0; y<h; y++)
	{
		unsigned char* srcy = src;
		unsigned char* desy = des;
		for (int x=0; x<w; x++)
		{
			a = *srcy ++;
			if (a == 0)
			{
				//srcy += 3;
				desy += 3;
			}
			else if(a == 255)
			{
				*desy ++ = b;
				*desy ++ = g;
				*desy ++ = r;
			}
			else
			{					
				*desy ++ = (b * a + *desy * (255-a)) / 255;
				*desy ++ = (g * a + *desy * (255-a)) / 255;;
				*desy ++ = (r * a + *desy * (255-a)) / 255;;
			}
		}
		src += line_pitch;
		des += get_line_pitch();
	}			
}

void image::render_image_3_3(int offx,int offy,unsigned char* buf, int w, int h,int line_pitch,int color, int alpha)
{
	assert(offx >= 0 && offy >= 0 && offx + w <= m_width && offy + h <= m_height);
	unsigned char* src = buf;
	unsigned char* des = get_buf_offset(offx,offy);

	unsigned char r,g,b,a;
	a = alpha;				
	for (int y=0; y<h; y++)
	{
		unsigned char* srcy = src;
		unsigned char* desy = des;
		for (int x=0; x<w; x++)
		{
			r = *srcy ++;
			g = *srcy ++;
			b = *srcy ++;
			//图片是rgb的顺序，而dc是bgr
			*desy ++ = (b * a + *desy * (255-a)) / 255;
			*desy ++ = (g * a + *desy * (255-a)) / 255;;
			*desy ++ = (r * a + *desy * (255-a)) / 255;;
		}
		src += line_pitch;
		des += get_line_pitch();
	}			
}

void image::render_image_4_3(int offx,int offy,unsigned char* buf, int w, int h,int line_pitch,int color, int alpha)
{
	assert(offx >= 0 && offy >= 0 && offx + w <= m_width && offy + h <= m_height);
	unsigned char* src = buf;
	unsigned char* des = get_buf_offset(offx,offy);

	unsigned char r,g,b,a;
	for (int y=0; y<h; y++)
	{
		unsigned char* srcy = src;
		unsigned char* desy = des;
		for (int x=0; x<w; x++)
		{
			r = *srcy ++;
			g = *srcy ++;
			b = *srcy ++;
			a = *srcy ++;
			a = a * alpha / 255;
			if (a == 0)
			{
				//srcy += 3;
				desy += 3;
			}
			else if(a == 255)
			{
				*desy ++ = b;
				*desy ++ = g;
				*desy ++ = r;
			}
			else
			{					
				*desy ++ = (b * a + *desy * (255-a)) / 255;
				*desy ++ = (g * a + *desy * (255-a)) / 255;;
				*desy ++ = (r * a + *desy * (255-a)) / 255;;
			}
		}
		src += line_pitch;
		des += get_line_pitch();
	}
}


int image::draw_image_cell(const st_cell& cell,const image* img,const g_rect* rc_img,const g_rect* rc_clip)
{
	return draw_image(cell.x,cell.y,cell.color,cell.alpha,img,rc_img,rc_clip);
}
int image::draw_box_cell(const st_cell& cell,int w,int h)
{
	return draw_box(cell.x,cell.y,cell.color,cell.alpha,w,h);
}

int image::draw_image(int offx,int offy,int color,int alpha,const image* img,const g_rect* rc_img,const g_rect* rc_clip)
{
	if (alpha <= 0)
		return -1;
	g_rect rect = rc_img ? *rc_img : img->get_rect();

	g_rect clip;
	if (rc_clip)
	{
		clip = *rc_clip - get_rect();
		if (clip.is_empty())
			return -1;
	}
	else
		clip = get_rect();

	if (offx < clip.l)
	{
		rect.l -= offx - clip.l;
		if (rect.r <= rect.l)
			return -1;
		offx = clip.l;
	}
	else if (offx >= clip.r)
		return -1;
	if (offx + rect.width() > clip.r)
		rect.r -= offx + rect.width() - clip.r;

	if (offy < clip.t)
	{
		rect.t -= offy - clip.t;
		if (rect.b <= rect.t)
			return -1;
		offy = clip.t;
	}
	else if (offy >= clip.b)
		return -1;
	if (offy + rect.height() > clip.b)
		rect.b -= offy + rect.height() - clip.b;

	if (rect.is_empty())
		return -1;

	unsigned char* src = (const_cast<image*>(img))->get_buf_offset(rect.l,rect.t);
	//unsigned char* des = get_buf_offset(offx,offy);
	int w = rect.width();
	int h = rect.height();
	
	//not use cell.alpha and cell.color
	if (!img->m_alpha && alpha == 255)
	{
		assert(img->m_bits_pixel == m_bits_pixel);
		if (img->m_bits_pixel == m_bits_pixel)
			copy_image(offx,offy,src,w,h,img->get_line_pitch());
	}
	else
	{
		if (m_bits_pixel == 3)
		{
			if (img->m_bits_pixel == 1)
				render_image_1_3(offx,offy,src,w,h,img->get_line_pitch(),color,alpha);
			else if (img->m_bits_pixel == 3)
				render_image_3_3(offx,offy,src,w,h,img->get_line_pitch(),color,alpha);
			else if (img->m_bits_pixel == 4)
				render_image_4_3(offx,offy,src,w,h,img->get_line_pitch(),color,alpha);
		}
		else if (m_bits_pixel == 4)
		{
			assert(img->m_bits_pixel == m_bits_pixel);
			if (alpha == 255)
				copy_image(offx,offy,src,w,h,img->get_line_pitch());
		}
	}
	return 0;
}

int image::draw_box(int offx,int offy,int color,int alpha,int w,int h)
{
	if (offx < 0)
	{
		w += offx;
		offx = 0;
	}
	else if (offx >= get_width())
		return -1;
	if (offx + w > get_width())
		w = get_width() - offx;

	if (offy < 0)
	{
		h += offy;
		offy = 0;
	}
	else if (offy >= get_height())
		return -1;
	if (offy + h > get_height())
		h = get_height() - offy;

	unsigned char* des = get_buf_offset(offx,offy);
	unsigned char a,r,g,b;
	G_GET_ARGB(color,a,r,g,b);
	a = alpha;
	if (a == 255)
	{
		for (int y=0; y<h; y++)
		{
			unsigned char* desy = des;
			for (int x=0; x<w; x++)
			{
				*desy ++ = b;
				*desy ++ = g;
				*desy ++ = r;
			}
			des += get_line_pitch();
		}
	}
	else
	{
		for (int y=0; y<h; y++)
		{
			unsigned char* desy = des;
			for (int x=0; x<w; x++)
			{
				*desy ++ = (b * a + *desy * (255-a)) / 255;
				*desy ++ = (g * a + *desy * (255-a)) / 255;;
				*desy ++ = (r * a + *desy * (255-a)) / 255;;
			}
			des += get_line_pitch();
		}
	}
	return 0;
}

void image::compress()
{
	assert(m_buffer && !m_buffer_compress);
	uLong size = max(compressBound(get_buf_size()),get_buf_size());
	unsigned char* des = new unsigned char[size];
	int rt = ::compress(des,&size,m_buffer,get_buf_size());
	assert(rt == Z_OK);

	m_sz_compress = size;
	m_buffer_compress = new unsigned char[size];
	memcpy(m_buffer_compress,des,size);

	delete m_buffer;
	m_buffer = NULL;
}

void image::uncompress() 
{
	assert(!m_buffer && m_buffer_compress);
	uLong size = get_buf_size();
	m_buffer = new unsigned char[size];

	int rt = ::uncompress(m_buffer,&size,m_buffer_compress,m_sz_compress);
	assert(rt == Z_OK);

	delete m_buffer_compress;
	m_buffer_compress = NULL;
}
