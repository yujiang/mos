#ifndef IMAGE_ZGP_H
#define IMAGE_ZGP_H

#include "../graph/rect.h"
#include <assert.h>
#include "../counter.h"
#include <unordered_map>

typedef unsigned short      WORD;
typedef unsigned long       DWORD;
typedef unsigned char       BYTE;

struct WSpritePakFileHeader {
	WORD dir;
	WORD frame;
	WORD width;
	WORD height;
	short kx;
	short ky;
	char pal_num;
	char null;	//为空
	unsigned short extern_len;
};  

struct WSpriteStruct {
	int kx;									// 中心点 x
	int ky;									// 中心点 y
	int w;									// 宽度
	int h;									// 高度
	BYTE **line;							// 第一扫描行指针
	WORD** pal;								// 调色盘
	DWORD sprite_userdata;					// 额外数据
	void *data;	
};


class sprite_zgp
{
public:
	int spt_type;
	WSpriteStruct m_data;
	bool create_sprite(WORD** pal,void* _data);
	bool create_empty();

	g_point get_cg() const{
		return g_point(m_data.kx,m_data.ky);
	}

	void render8888(int x,int y,DWORD* buf8888,int pitch,int block = -1,WORD** pal = 0) const;
};

class image;
#define ZGP_MAX_PARTS 6

class image_zgp
{
public:
	counter<image_zgp> m_counter;

	WSpritePakFileHeader pak;
	WORD* pal;
	WORD* pals[ZGP_MAX_PARTS];

	std::unordered_map<int,WORD*> m_pals_hsv;

	sprite_zgp* sprites;
	char* _data;

	image_zgp(){
		pal = 0;
		sprites = 0;
		_data = 0;
	}
	~image_zgp();

	bool loadzgp_file(const char* zgp);
	bool loadzgp_memory(const char* zgp,char* _data);

	int get_frame_num() const{
		return pak.dir * pak.frame;
	}
	sprite_zgp* get_sprite(int frame) const{
		assert(frame < get_frame_num());
		return &sprites[frame];
	}
	sprite_zgp* get_sprite_dirframe(int dir,int frame) const{
		return get_sprite(pak.frame*dir+frame);
	}

	image* get_sprite_image(int frame,int block = -1) const;
	image* get_sprite_image_parts(int frame,const DWORD* parts) ;

	unsigned int m_time_use;
	void mark_use_zgp(unsigned int time){
		m_time_use = time;
	}

	//static image* create_image_zgp(const char* file,int frame);
	//WORD* create_pal_hsv(int part,int h,int s,int v);
	WORD* find_pal(int part,DWORD hsv);
};

//get zgp info...
bool regist_zgp(const char* file,int& dir,int& frame);

class file_source;
file_source* get_file_source_zgp();
#endif