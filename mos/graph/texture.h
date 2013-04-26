#ifndef __GRAPH_TEXTURE_H_
#define __GRAPH_TEXTURE_H_

//wrap opengl texture not directx.
struct g_rect;
class st_cell;

typedef enum {

	//! 32-bit texture: RGBA8888
	kCCTexture2DPixelFormat_RGBA8888,
	//! 24-bit texture: RGBA888
	kCCTexture2DPixelFormat_RGB888,
	//! 16-bit texture without Alpha channel
	kCCTexture2DPixelFormat_RGB565,
	//! 8-bit textures used as masks
	kCCTexture2DPixelFormat_A8,
	//! 8-bit intensity texture
	kCCTexture2DPixelFormat_I8,
	//! 16-bit textures used as masks
	kCCTexture2DPixelFormat_AI88,
	//! 16-bit textures: RGBA4444
	kCCTexture2DPixelFormat_RGBA4444,
	//! 16-bit textures: RGB5A1
	kCCTexture2DPixelFormat_RGB5A1,    
	//! 4-bit PVRTC-compressed texture: PVRTC4
	kCCTexture2DPixelFormat_PVRTC4,
	//! 2-bit PVRTC-compressed texture: PVRTC2
	kCCTexture2DPixelFormat_PVRTC2,

	//! Default texture format: RGBA8888
	kCCTexture2DPixelFormat_Default = kCCTexture2DPixelFormat_RGBA8888,

	// backward compatibility stuff
	kTexture2DPixelFormat_RGBA8888 = kCCTexture2DPixelFormat_RGBA8888,
	kTexture2DPixelFormat_RGB888 = kCCTexture2DPixelFormat_RGB888,
	kTexture2DPixelFormat_RGB565 = kCCTexture2DPixelFormat_RGB565,
	kTexture2DPixelFormat_A8 = kCCTexture2DPixelFormat_A8,
	kTexture2DPixelFormat_RGBA4444 = kCCTexture2DPixelFormat_RGBA4444,
	kTexture2DPixelFormat_RGB5A1 = kCCTexture2DPixelFormat_RGB5A1,
	kTexture2DPixelFormat_Default = kCCTexture2DPixelFormat_Default

} CCTexture2DPixelFormat;


struct formatinfo{
	CCTexture2DPixelFormat format;
	bool alpha;
	int bits;
	int internalformat;
	int glformat;
	int gltype;
	int unpack_aliment;
};

const formatinfo* get_formatinfo(CCTexture2DPixelFormat format);
unsigned int bitsPerPixelForFormat(CCTexture2DPixelFormat format);
bool hasAlpha(CCTexture2DPixelFormat format);

class texture
{
public:
	virtual bool create_texture_dynamic(int width,int height,CCTexture2DPixelFormat format) = 0;
	virtual bool create_texture(const image* img,const g_rect* rc,CCTexture2DPixelFormat format = kCCTexture2DPixelFormat_Default) = 0;

	virtual int draw_image_ontexture(int x,int y,const image* img) = 0;

	int draw_cell(const st_cell& cell,const g_rect* rc );

	unsigned int m_time_use;
	void mark_use_texture(unsigned int time){
		m_time_use = time;
	}
};

//texture class: 
//texture_whole	美术把ui尽可能的拼到同一个图，还比如说icon
//texture_part	每个part是whole的一个rect
//texture,		比如动画这些不拼的
//texture_font	把所有字合并在同一个贴图，并根据使用的频繁做更新，
//texture_char	类似part，是font的一个rect


#endif