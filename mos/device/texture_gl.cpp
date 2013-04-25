#include "texture_gl.h"
#include "device/window_render.h"
#include "mos.h"
#include "graph/image.h"
#include <assert.h>
#include "gl_macro.h"
//#include "cocos2dx/include/ccMacros.h"
//#include "cocos2dx/shaders/ccGLStateCache.h"


//USING_NS_CC;

bool is_2_mi(unsigned int n)
{
	return (n &(n-1)) == 0;
}

//接近2的幂的数 比如30->32 33->64
unsigned long ccNextPOT(unsigned long x)
{
	x = x - 1;
	x = x | (x >> 1);
	x = x | (x >> 2);
	x = x | (x >> 4);
	x = x | (x >> 8);
	x = x | (x >>16);
	return x + 1;
}


texture_gl::texture_gl()
{
	m_textureId = 0;
}

texture_gl::~texture_gl()
{
	if(m_textureId)
	{
		glDeleteTextures(1, &m_textureId);
		m_textureId = 0;
	}
}

bool texture_gl::create_texture(const image* img,const g_rect* rc,CCTexture2DPixelFormat format) 
{
	g_rect rect = rc ? *rc : img->get_rect();

	unsigned char* buf = img->get_buf_offset(rect.l,rect.t);
	
	//buf = new unsigned char[rect.width()*rect.height()*4];
	//memset(buf,255,rect.width()*rect.height()*4);
	format = img->m_bits_pixel == 3 ? kCCTexture2DPixelFormat_RGB888 : kCCTexture2DPixelFormat_RGBA8888;
	bool rt = initWithData(buf,img->get_width(),format,rect.width(),rect.height(),rect.width(),rect.height());
	//delete buf;
	return rt;
}

bool texture_gl::create_texture_dynamic(int width,int height,CCTexture2DPixelFormat format) 
{
	assert(is_2_mi(width) && is_2_mi(height));
	int bits = bitsPerPixelForFormat(format)/8;
	char* data = new char[width*height*bits];
	memset(data,0,sizeof(char)*width*height*bits);
	bool rt = initWithData(data,0,format,width,height,width,height);
	delete data;
	return rt;
}

int texture_gl::draw_image_ontexture(int x,int y,const image* img) 
{
	unsigned char* buf = img->get_buffer();
	return updateWithData(buf,img->m_width,x,y,img->m_width,img->m_height);
}

bool texture_gl::updateWithData(const void* data, int rowLength,int offx,int offy,int width, int height)
{
	const formatinfo* info = get_formatinfo(m_format);
	if (!info)
		return false;

	//glGenTextures(1, &m_textureId);
	//glActiveTexture(GL_TEXTURE0 );
	glBindTexture(GL_TEXTURE_2D, m_textureId);

	glTexSubImage2D(GL_TEXTURE_2D, 0, offx, offy, width, height, info->glformat, info->gltype, data);	

	CHECK_GL_ERROR_DEBUG();

	return true;
}

bool texture_gl::initWithData(const void *data, int rowLength, CCTexture2DPixelFormat pixelFormat, int pixelsWide, int pixelsHigh, int width, int height)
{
	// XXX: 32 bits or POT textures uses UNPACK of 4 (is this correct ??? )
	const formatinfo* info = get_formatinfo(pixelFormat);
	if (!info)
		return false;

	glGenTextures(1, &m_textureId);
	//glActiveTexture(GL_TEXTURE0 );
	glBindTexture(GL_TEXTURE_2D, m_textureId);

	//int align = ( info->unpack_aliment == 4 || ( is_2_mi(pixelsWide) && is_2_mi(pixelsHigh) )) ? 4 : 1;
	//glPixelStorei(GL_UNPACK_ALIGNMENT,align);

	if (rowLength > 0)
		glPixelStorei(GL_UNPACK_ROW_LENGTH, rowLength);

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	// Specify OpenGL texture image

	glTexImage2D(GL_TEXTURE_2D, 0, info->internalformat, (GLsizei)pixelsWide, (GLsizei)pixelsHigh, 0, info->glformat, info->gltype, data);	

	CHECK_GL_ERROR_DEBUG();

	m_tex_width = pixelsWide;
	m_tex_height = pixelsHigh;
	m_width = width;
	m_height = height;
	m_format = pixelFormat;

	return true;
}	

formatinfo c_formatinfos[] = {
	{kCCTexture2DPixelFormat_RGBA8888,true,32,GL_RGBA,GL_RGBA,GL_UNSIGNED_BYTE,4},
	{kCCTexture2DPixelFormat_RGB888,false,32,GL_RGB,GL_RGB,GL_UNSIGNED_BYTE,1},
	{kCCTexture2DPixelFormat_RGB565,false,16,GL_RGB,GL_RGB,GL_UNSIGNED_SHORT_5_6_5,1},
	{kCCTexture2DPixelFormat_A8,true,8,GL_ALPHA8,GL_ALPHA,GL_UNSIGNED_BYTE,1},
	{kCCTexture2DPixelFormat_I8,false,8,GL_LUMINANCE,GL_LUMINANCE,GL_UNSIGNED_BYTE,1},
	{kCCTexture2DPixelFormat_AI88,true,16,GL_LUMINANCE_ALPHA,GL_LUMINANCE_ALPHA,GL_UNSIGNED_BYTE,1},
	{kCCTexture2DPixelFormat_RGBA4444,true,16,GL_RGBA,GL_RGBA,GL_UNSIGNED_SHORT_4_4_4_4,1},
	{kCCTexture2DPixelFormat_RGB5A1,true,16,GL_RGBA,GL_RGBA,GL_UNSIGNED_SHORT_5_5_5_1,1},
	//{kCCTexture2DPixelFormat_PVRTC4,false,4,GL_RGBA,GL_RGBA,GL_UNSIGNED_BYTE,1},
	//{kCCTexture2DPixelFormat_PVRTC2,false,2,GL_RGBA,GL_RGBA,GL_UNSIGNED_BYTE,1},
};

const formatinfo* get_formatinfo(CCTexture2DPixelFormat format)
{
	return &c_formatinfos[format];
}

bool hasAlpha(CCTexture2DPixelFormat format)
{
	return get_formatinfo(format)->alpha;
}

unsigned int bitsPerPixelForFormat(CCTexture2DPixelFormat format)
{
	return get_formatinfo(format)->bits;
}

