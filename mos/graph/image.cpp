#include "image.h"
#include <string.h>
#include "device/file.h"
#include "color.h"
#include "cell.h"
#include "assert.h"
#include "zlib/zlib.h"
#include <map>
#include <string>

int image::s_image_id = 0;
int image::s_image_num = 0;

image::image()
{
	m_id = s_image_id++;
	s_image_num++;

	m_ref = 1;

	m_width = m_height = 0;

	m_buffer = 0;
	m_pal_color = 0;
	m_pal_alpha = 0;
	
//	m_buffer_compress = 0;
//	m_sz_compress = 0;

	m_create_type = image_create_null;
}	

image::~image()
{
	s_image_num--;

	delete m_buffer;
	m_buffer = 0;

	delete m_pal_color;
	m_pal_color = 0;
	delete m_pal_alpha;
	m_pal_alpha = 0;
//	delete m_buffer_compress;
//	m_buffer_compress = 0;
}


bool image::in_image(int x, int y) 
{
	if (x >= 0 && x<m_width && y >= 0 && y<m_height)
	{
		if (!has_alpha())
			return true;
		colorbyte* des = get_buf_offset(x,y);
		if (is_256())
		{
			colorbyte index = *des++;
			if (index < m_pal_alpha_num)
				return m_pal_alpha[index] > 127;
			else
				return true;
		}
		else
		{
			colorbyte a = *(des + 3);
			return a > 127;
		}
	}
	return false;
}

std::map<std::string,load_image_func> s_imageLoad;
void image::register_image_file(const char* fileext,load_image_func func)
{
	s_imageLoad[fileext] = func;
}

image* image::create_image_file(const char* file)
{
	size_t sz;
	char* buf = read_imagefile(file,sz);
	if (!buf)
		return false;
	int len = strlen(file);
	if (len <= 3)
		return false;
	const char* ext = file+len-3;
	load_image_func func = s_imageLoad[ext];
	if (!func)
		return false;
	image* i = new image();
	if (func(i,buf,sz))
	{
		i->m_create_type = image_create_file;
		i->m_file = file;
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
	//m_premul_alpha = false;
	//m_alpha = false;
	m_buffer = new colorbyte[get_buf_size()];
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
	//m_premul_alpha = i->m_premul_alpha;
	//m_alpha = i->m_alpha;
	if (!rc)
	{
		int size = get_buf_size();
		memcpy(m_buffer,i->m_buffer,size);
	}
	else
	{
		draw_image(0,0,-1,255,i,rc,NULL);
	}
	return true;
}

//图片是rgb的顺序，而dc是bgr，所以有此函数
void image::rgb2bgr()
{
	assert(m_bits_pixel == 3);
	colorbyte* p = m_buffer;
	colorbyte r;
	for (int i=0; i<m_width*m_height; ++i)
	{
		r = *p;
		*p = *(p+2);
		*(p+2) = r ;
		p += 3;
	}
}

void image::clear(unsigned long color)
{
	colorbyte a,r,g,b;
	G_GET_ARGB(color,a,r,g,b);
	if (r == g && g == b)
		memset(m_buffer,r,get_buf_size());
	else
	{
		assert(m_bits_pixel == 3);
		colorbyte* p = m_buffer;
		for (int i=0; i<m_width*m_height; ++i)
		{
			*p ++ = r;
			*p ++ = g;
			*p ++ = b;
		}
	}
}

int image::copy_image(int offx,int offy,colorbyte* buf, int w,int h,int line_pitch)
{
	assert(offx >= 0 && offy >= 0 && offx + w <= m_width && offy + h <= m_height);
	colorbyte* src = buf;
	colorbyte* des = get_buf_offset(offx,offy);

	for (int y=0; y<h; y++)
	{
		memcpy(des,src,w*m_bits_pixel);
		src += line_pitch;
		des += get_line_pitch();
	}
	return 0;
}

void image::render_image_1_3(int offx,int offy,colorbyte* buf, int w, int h,int line_pitch,int color,int alpha)
{
	assert(offx >= 0 && offy >= 0 && offx + w <= m_width && offy + h <= m_height);
	colorbyte* src = buf;
	colorbyte* des = get_buf_offset(offx,offy);

	colorbyte r,g,b,a;
	G_GET_ARGB(color,a,r,g,b);
	for (int y=0; y<h; y++)
	{
		colorbyte* srcy = src;
		colorbyte* desy = des;
		for (int x=0; x<w; x++)
		{
			a = *srcy ++;
			if (a == 0)
			{
				desy += 3;
			}
			else if(a == 255)
			{
				*desy ++ = r;
				*desy ++ = g;
				*desy ++ = b;
			}
			else
			{					
				*desy ++ = (r * a + *desy * (255-a)) / 255;
				*desy ++ = (g * a + *desy * (255-a)) / 255;
				*desy ++ = (b * a + *desy * (255-a)) / 255;
			}
		}
		src += line_pitch;
		des += get_line_pitch();
	}			
}

void image::render_image_3_3(int offx,int offy,colorbyte* buf, int w, int h,int line_pitch,int color, int alpha)
{
	assert(offx >= 0 && offy >= 0 && offx + w <= m_width && offy + h <= m_height);
	colorbyte* src = buf;
	colorbyte* des = get_buf_offset(offx,offy);

	colorbyte r,g,b,a;
	a = alpha;				
	for (int y=0; y<h; y++)
	{
		colorbyte* srcy = src;
		colorbyte* desy = des;
		for (int x=0; x<w; x++)
		{
			r = *srcy ++;
			g = *srcy ++;
			b = *srcy ++;
			*desy ++ = (r * a + *desy * (255-a)) / 255;
			*desy ++ = (g * a + *desy * (255-a)) / 255;
			*desy ++ = (b * a + *desy * (255-a)) / 255;
		}
		src += line_pitch;
		des += get_line_pitch();
	}			
}

void image::render_image_4_3(int offx,int offy,colorbyte* buf, int w, int h,int line_pitch,int color, int alpha)
{
	assert(offx >= 0 && offy >= 0 && offx + w <= m_width && offy + h <= m_height);
	colorbyte* src = buf;
	colorbyte* des = get_buf_offset(offx,offy);

	colorbyte r,g,b,a;
	for (int y=0; y<h; y++)
	{
		colorbyte* srcy = src;
		colorbyte* desy = des;
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
				*desy ++ = r;
				*desy ++ = g;
				*desy ++ = b;
			}
			else
			{					
				*desy ++ = (r * a + *desy * (255-a)) / 255;
				*desy ++ = (g * a + *desy * (255-a)) / 255;
				*desy ++ = (b * a + *desy * (255-a)) / 255;
			}
		}
		src += line_pitch;
		des += get_line_pitch();
	}
}

void image::render_image_256_3(const image* img,int offx,int offy,colorbyte* buf, int w, int h,int line_pitch,int color, int alpha)
{
	assert(img->is_256());
	assert(offx >= 0 && offy >= 0 && offx + w <= m_width && offy + h <= m_height);
	colorbyte* src = buf;
	colorbyte* des = get_buf_offset(offx,offy);
	colorbyte a;
	for (int y=0; y<h; y++)
	{
		colorbyte* srcy = src;
		colorbyte* desy = des;
		for (int x=0; x<w; x++)
		{
			colorbyte index = *srcy++;
			color_palette* pal = img->m_pal_color+index;
			if (index < img->m_pal_alpha_num)
				a = img->m_pal_alpha[index] * alpha / 255;
			else
				a = alpha;
			//a = 255;
			if (a == 0)
			{
				desy += 3;
			}
			else if (a == 255)
			{
				*desy ++ = pal->red;
				*desy ++ = pal->green;
				*desy ++ = pal->blue;
			}
			else
			{
				*desy ++ = (pal->red * a + *desy * (255-a)) / 255;
				*desy ++ = (pal->green * a + *desy * (255-a)) / 255;
				*desy ++ = (pal->blue * a + *desy * (255-a)) / 255;
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

bool get_cliped_rect(g_rect& rect,const g_rect& rc,int& offx,int& offy,const g_rect* rc_clip)
{
	g_rect clip;
	if (rc_clip)
	{
		clip = *rc_clip - rc;
		if (clip.is_empty())
			return false;
	}
	else
		clip = rc;

	if (offx < clip.l)
	{
		rect.l -= offx - clip.l;
		if (rect.r <= rect.l)
			return false;
		offx = clip.l;
	}
	else if (offx >= clip.r)
		return false;
	if (offx + rect.width() > clip.r)
		rect.r -= offx + rect.width() - clip.r;

	if (offy < clip.t)
	{
		rect.t -= offy - clip.t;
		if (rect.b <= rect.t)
			return false;
		offy = clip.t;
	}
	else if (offy >= clip.b)
		return false;
	if (offy + rect.height() > clip.b)
		rect.b -= offy + rect.height() - clip.b;

	if (rect.is_empty())
		return false;

	return true;
}

int image::draw_image(int offx,int offy,int color,int alpha,const image* img,const g_rect* rc_img,const g_rect* rc_clip)
{
	if (alpha <= 0)
		return -1;
	g_rect rect = rc_img ? *rc_img : img->get_rect();
	g_rect rc = get_rect();
	if (!get_cliped_rect(rect,rc,offx,offy,rc_clip))
		return -1;

	colorbyte* src = (const_cast<image*>(img))->get_buf_offset(rect.l,rect.t);
	int w = rect.width();
	int h = rect.height();
	
	if (!img->has_alpha() && alpha == 255)
	{
		assert(img->m_bits_pixel == m_bits_pixel);
		//if (img->m_bits_pixel == m_bits_pixel)
		copy_image(offx,offy,src,w,h,img->get_line_pitch());
	}
	else
	{
		if (m_bits_pixel == 3)
		{
			if (img->is_256())
				render_image_256_3(img,offx,offy,src,w,h,img->get_line_pitch(),color,alpha);
			else if (img->m_bits_pixel == 1)
				render_image_1_3(offx,offy,src,w,h,img->get_line_pitch(),color,alpha);
			else if (img->m_bits_pixel == 3)
				render_image_3_3(offx,offy,src,w,h,img->get_line_pitch(),color,alpha);
			else if (img->m_bits_pixel == 4)
				render_image_4_3(offx,offy,src,w,h,img->get_line_pitch(),color,alpha);
		}
		else //if (m_bits_pixel == 4)
		{
			assert(img->m_bits_pixel == m_bits_pixel);
			assert(alpha == 255);
			copy_image(offx,offy,src,w,h,img->get_line_pitch());
		}
	}
	return 0;
}

bool get_cliped_box(int& offx,int& offy,int& w,int& h, int width,int height)
{
	if (offx < 0)
	{
		w += offx;
		offx = 0;
	}
	else if (offx >= width)
		return false;
	if (offx + w > width)
		w = width - offx;

	if (offy < 0)
	{
		h += offy;
		offy = 0;
	}
	else if (offy >= height)
		return false;
	if (offy + h > height)
		h = height - offy;
	return true;
}

int image::draw_box(int offx,int offy,int color,int alpha,int w,int h)
{
	if (!get_cliped_box(offx,offy,w,h,get_width(),get_height()))
		return -1;
	colorbyte* des = get_buf_offset(offx,offy);
	colorbyte a,r,g,b;
	G_GET_ARGB(color,a,r,g,b);
	a = alpha;
	if (a == 255)
	{
		for (int y=0; y<h; y++)
		{
			colorbyte* desy = des;
			for (int x=0; x<w; x++)
			{
				*desy ++ = r;
				*desy ++ = g;
				*desy ++ = b;
			}
			des += get_line_pitch();
		}
	}
	else
	{
		for (int y=0; y<h; y++)
		{
			colorbyte* desy = des;
			for (int x=0; x<w; x++)
			{
				*desy ++ = (r * a + *desy * (255-a)) / 255;
				*desy ++ = (g * a + *desy * (255-a)) / 255;
				*desy ++ = (b * a + *desy * (255-a)) / 255;
			}
			des += get_line_pitch();
		}
	}
	return 0;
}

//void image::compress()
//{
//	assert(m_buffer && !m_buffer_compress);
//	uLong size = max(compressBound(get_buf_size()),get_buf_size());
//	colorbyte* des = new colorbyte[size];
//	int rt = ::compress(des,&size,m_buffer,get_buf_size());
//	assert(rt == Z_OK);
//
//	m_sz_compress = size;
//	m_buffer_compress = des;
//
//	delete m_buffer;
//	m_buffer = NULL;
//}
//
//void image::uncompress() 
//{
//	assert(!m_buffer && m_buffer_compress);
//	uLong size = get_buf_size();
//	m_buffer = new colorbyte[size];
//
//	int rt = ::uncompress(m_buffer,&size,m_buffer_compress,m_sz_compress);
//	assert(rt == Z_OK);
//
//	delete m_buffer_compress;
//	m_buffer_compress = NULL;
//}

void image::set_palette_color(const color_palette* colors,int num_palette)
{
	m_pal_color_num = num_palette;
	m_pal_color = new color_palette[num_palette];
	memcpy(m_pal_color,colors,sizeof(color_palette)*num_palette);
}

void image::set_palette_alpha(const colorbyte* alphas,int num_palette)
{
	m_pal_alpha_num = num_palette;
	m_pal_alpha = new colorbyte[num_palette];
	memcpy(m_pal_alpha,alphas,sizeof(colorbyte)*num_palette);
}

colorbyte* image::render_256_argb() const
{
	int size = m_width*m_height;
	colorbyte* buf = new colorbyte[size*4];
	colorbyte* des = buf;
	const colorbyte* src = m_buffer;
	for (int i=0;i<size;i++)
	{
		colorbyte index = *src++;
		color_palette* pal = m_pal_color+index;
		*des ++ = pal->red;
		*des ++ = pal->green;
		*des ++ = pal->blue;
		if (index < m_pal_alpha_num)		
			*des++ = m_pal_alpha[index];
		else
			*des++ = 255;
	}

	return buf;
}