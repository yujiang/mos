#include "image_zgp.h"
#include "../graph/image.h"
#include "device/file.h"
#include "mos.h"
#include "../graph/graph.h"
#include "crc32.h"

//本来为了通用，想使用png的格式，
//并且发现主要zgp的格式并不优化，比png要大一倍，
//但是png没有块的信息，而为了分块染色并且在内存动态展开，还是必须用的。
//所以。。。

#include <windows.h>

#include <string>
#include <fstream>
#include <iostream>
using namespace std;

DWORD color565_8888(DWORD color)
{
	return (color<<5&0x7fc00)|(color<<8&0xf80000)|((color<<3|(color&7))&0x3ff);
};

struct st_header{
	WORD id;
	WORD ver;
	WORD len;
};

//#define CHECK_BLOCK if (block < 0 || mask_type == block)
#define CHECK_BLOCK 

__inline DWORD MakeSrcDword(DWORD wb)
{
	return ((wb << 16) | wb) & 0x7e0f81f;
}

DECLARE_COUNTER(image_zgp)

bool sprite_zgp::create_sprite(WORD** pal,void* _data)
{
	WSpriteStruct *spt_src=(WSpriteStruct*)(_data);
	WSpriteStruct *spt_this=&m_data;
	spt_this->kx = spt_src->kx; 
	spt_this->ky = spt_src->ky; 
	spt_this->w = spt_src->w; 
	spt_this->h = spt_src->h; 
	spt_this->data=0;
	spt_this->sprite_userdata=spt_src->sprite_userdata;
	spt_this->line=(BYTE **)&spt_src->line;
	for (int i=0;i<spt_this->h;i++) {
		spt_this->line[i]+=(DWORD)_data;
		//BYTE p = *(spt_this->line[i]);
	}
	spt_this->pal=pal; 
	spt_type = 0;
	//sprite是紧凑的，不需要做trim了。
	return true;
}

bool sprite_zgp::create_empty()
{
	WSpriteStruct *spt_this=&m_data;
	spt_this->kx=0;
	spt_this->ky=0;
	spt_this->w=0;
	spt_this->h=0;
	spt_this->line=0;
	spt_this->data=0;
	spt_this->pal=0;
	spt_this->sprite_userdata = 0;	
	spt_type = 1;	
	return true;
}

//没有考虑分块信息？所以并不好用！
void sprite_zgp::render8888(int x,int y,DWORD* buf8888,int pitch,int block,WORD** pal) const
{
	DWORD* deslineptr = buf8888 + pitch * y + x;
	if (pal == NULL)
		pal = m_data.pal;
	int w = m_data.w;
	int h = m_data.h;
	int off = 0;

	WORD tpixel;
	//渲染到buf
	for (int i=0; i<h; i++,deslineptr+=pitch)
	{
		DWORD* lineptr = deslineptr;
		BYTE* src = m_data.line[i];
		if(*src == 0xff && *(src+1)==0xff ) //mean重复
			src=m_data.line[i-1];
		for (int j=0; j<w;)
		{
			BYTE mask_type, alpha;
			BYTE flag, style;
			style=*src++;
			if (style==0) 
				break;
			flag=style&3;
			style=style>>2;
			int k;
			int m;
			switch(flag)
			{
			case 0://全透明
				{
					m = min(j+style,off);
					k = max(j,m);
					m = min(j+style,w);
					if (k < m)
					{
						for (int z = 0; z < m-k; z++)
						{
							//*lineptr++ = tpixel;
							*lineptr = 0;								
							lineptr++;
						}
						//lineptr += m - k;
						j=m;
					}
					else
						j = k;
					break;
				}
			case 1://连续多个不透明并且color相同像素
				{
					mask_type = *src++;
					tpixel=pal[mask_type][*src];
					m = min(j+style,off);
					k = max(j,m);
					m = min(j+style,w);
					if (k < m)
					{
						for (int z = 0; z < m-k; z++)
						{
							//*lineptr++ = tpixel;
							CHECK_BLOCK
								*lineptr = color565_8888(tpixel) | 0xff000000;								
							lineptr++;
						}
						j = m;
					}
					else
						j = k;

					++src;
					break;
				}
			case 2://多个半透明并且color相同像素
				{
					alpha= *src++;
					mask_type = alpha>>5;
					alpha&=31;
					tpixel=pal[mask_type][*src];
					m = min(j+style,off);
					k = max(j,m);
					m = min(j+style,w);
					if (k < m)
					{
						//DWORD sd = MakeSrcDword(tpixel);
						//alpha = 32 - (alpha+1);
						for (int z = 0; z < m-k; z++)
						{
							//MakeAlpha32Inverse((WORD*)lineptr,sd,alpha);
							CHECK_BLOCK
								*lineptr = color565_8888(tpixel) | (alpha << (24+3));								
							lineptr++;
						}
						j = m;
					}
					else
						j = k;

					++src;
					break;
				}
			case 3://连续多个不透明并且color不相同像素
				{
					mask_type = *src++;
					//int mask_offset = mask_type*256;
					const WORD* pp = pal[mask_type];

					for (k=j;k<j+style && k<off;k++,++src);
					for (;k<j+style && k<w;k++,++lineptr,++src) 
					{
						//*lineptr=pal[*src+mask_offset];
						CHECK_BLOCK
							*lineptr=color565_8888(pp[*src]) | 0xff000000;
					}
					j=k;

					break;
				}
			}
		}
	}
}


void mread(void* dest,char*& src,size_t size)
{
	memcpy(dest,src,size);
	src += size;
}


const int pal512 = 256 * 2; //256色的word


//在手机上图片都做成png 256色的，每个方向是一行，铺在一起。
bool image_zgp::loadzgp_file(const char* zgp)
{
	FILE* f = fopen(get_resourcefile(zgp),"rb");
	if (!f)
		return false;
	int seek = fseek(f,0,SEEK_END);
	size_t size = ftell(f);
	fseek(f,0,SEEK_SET);
	//char* _data = new char[size];
	_data = new char[size];
	fread(_data,size,1,f);
	fclose(f);
	bool rt = loadzgp_memory(zgp,_data);
	//delete _data;
	return true;
}

bool image_zgp::loadzgp_memory(const char* zgp,char* _data)
{
	st_header header;
	char* data = (char* )_data;
	mread(&header,data,sizeof(header));
	mread(&pak,data,sizeof(pak));

	if (pak.extern_len)
	{
		char *ex_data = new char[pak.extern_len];
		mread(ex_data, data,pak.extern_len);
		delete []ex_data;
	}

	int allframe = pak.frame * pak.dir;
	size_t offset_size = allframe * sizeof(DWORD);

	//int m_Size=size + (pak->pal_num-1)*pal512 - header.len - sizeof(header)-pak->extern_len - offset_size;

	pal = new WORD[256 * pak.pal_num];
	mread(pal,data,pal512);
	pals[0] = pal;
	for(int i=1;i<pak.pal_num;i++)
	{
		//分块了。
		memcpy((char *)pal+pal512*i, pal, pal512);
		pals[i] = pal+256*i;
	}

	DWORD* offtbl = new DWORD[offset_size];
	mread(offtbl,data,offset_size);

	sprites = new sprite_zgp[allframe];
	int frame = 0;

	int width = 0;
	int height = 0;
	int y = 0;
	for (int i=0; i<pak.dir; i++)
	{
		int x = 0;
		int h = 0;
		for (int j=0; j<pak.frame; j++,frame++)
		{
			sprite_zgp& spr = sprites[frame];
			if (offtbl[frame])
			{
				char* off = data + offtbl[frame] - offset_size;
				spr.create_sprite(pals,off);
			}
			else
			{
				spr.create_empty();
			}

			//sprites[frame].set_pos(x,y);
			x += spr.m_data.w;
			if (h < spr.m_data.h)
				h = spr.m_data.h;
		}
		if (width < x)
			width = x;
		y += h;
	}
	height = y;

	width = ((width + 3) / 4) * 4;


	//delete [] pal;
	delete []offtbl;
	//delete []m_Data;
	//delete []sprites;

	//delete []_data;
	return true;
}

bool regist_zgp(const char* file,int& dir,int& frame)
{
	FILE* f = fopen(get_resourcefile(file),"rb");
	if (!f)
		return false;
	st_header header;
	fread(&header,sizeof(header),1,f);
	WSpritePakFileHeader pak;
	fread(&pak,sizeof(pak),1,f);
	fclose(f);
	dir = pak.dir;
	frame = pak.frame;
	return true;
}

image_zgp::~image_zgp()
{
	delete [] pal;
	delete []sprites;
	delete _data;
	for (auto it = m_pals_hsv.begin(); it != m_pals_hsv.end(); ++it)
		delete it->second;
	m_pals_hsv.clear();
}

image* image_zgp::get_sprite_image(int frame,int block ) const
{
	sprite_zgp* s = get_sprite(frame);

	image* i = new image();
	i->create_image_dynamic(s->m_data.w,s->m_data.h,4);
	s->render8888(0,0,(DWORD*)i->get_buffer(),i->get_width(),block,(WORD**)pals);
	i->set_cg(s->get_cg());
	i->rgb2bgr();
	return i;
}

image* image_zgp::get_sprite_image_parts(int frame,const DWORD* parts) 
{
	if (parts == 0)
		return  get_sprite_image(frame);

	sprite_zgp* s = get_sprite(frame);

	image* i = new image();
	i->create_image_dynamic(s->m_data.w,s->m_data.h,4);

	WORD* pals[ZGP_MAX_PARTS];
	for (int i=0;i<pak.pal_num;i++)
	{
		pals[i] = find_pal(i,parts[i]);
	}

	s->render8888(0,0,(DWORD*)i->get_buffer(),i->get_width(),-1,pals);
	i->set_cg(s->get_cg());
	i->rgb2bgr();
	return i;	
}


WORD* create_pal_hsv(WORD* pal,int h,int s,int v);

WORD* image_zgp::find_pal(int part,DWORD hsv)
{
	if (hsv == 0)
		return pals[part];

	int index = part << 24 | (hsv);
	if (m_pals_hsv[index])
		return m_pals_hsv[index];
	
	hsv &= 0x00ffffff;
	//h -180 180
	//s -100 100
	//v -20 20
	int h = (hsv & 0x00FFFFFF) >> 14;
	int s = (hsv & 0x00003FFF) >> 6;
	int v = (hsv & 0x0000003F) ;

	WORD* pal = create_pal_hsv(pals[part],h-180,s-100,v-20);
	m_pals_hsv[index] = pal;
	return pal;
}

//////////////////////////////////////////////////////////////////////////
class zgp_source : public file_source
{
public:
	zgp_source()
	{
		m_clear_zgp = 100;
	}
	int m_clear_zgp;
	//bool is_file(const char* file);
	const char* get_texture_file(const char* _file,int frame,const unsigned long* parts_pal_hsv) const;
	const char* get_file_ext() const{
		return "zgp";
	}

	//所有zgp资源。
	std::unordered_map<std::string,image_zgp*> zgp_map;
	image_zgp* find_file(const char* file);

	image* find_image_file(const char* file,int frame,const unsigned long* parts_pal_hsv);
	image* create_image_file(const char* file,int frame,const unsigned long* parts_pal_hsv);

	void auto_clear_resource();
	void close_resource();
	void dump_resource() const;
};

image_zgp* zgp_source::find_file(const char* file)
{
	image_zgp* zgp = zgp_map[file];
	if (!zgp)
	{
		zgp = new image_zgp;
		if (zgp->loadzgp_file(file))
		{
			zgp_map[file] = zgp;
		}
		else
		{
			delete zgp;
			zgp = NULL;
		}
	}
	return zgp;
}

image* zgp_source::create_image_file(const char* file,int frame,const DWORD* parts_pal_hsv)
{
	image_zgp* zgp = find_file(file);
	if (zgp)
	{
		zgp->mark_use_zgp(g_time_now);
		return zgp->get_sprite_image_parts(frame,parts_pal_hsv);
	}
	return NULL;
}

//bool zgp_source::is_file(const char* file)
//{
//	const char* p = file + strlen(file) - 4;
//	return strcmp(p,".zgp") == 0;
//}

const char* zgp_source::get_texture_file(const char* _file,int frame,const unsigned long* parts_pal_hsv) const
{
	static char file[128];
	if (parts_pal_hsv == NULL)
		sprintf(file,"%s_%04d",_file,frame);
	else
	{
		unsigned int hashcode = crc_buffer((char*)parts_pal_hsv,sizeof(*parts_pal_hsv) * ZGP_MAX_PARTS); //= hash_part(parts_pal_hsv); //一般这里冲突的可能性不大。
		if (hashcode == 0)
			sprintf(file,"%s_%04d",_file,frame);
		else
			sprintf(file,"%s_%04d_%x",_file,frame,hashcode);
	}
	return file;
}

image* zgp_source::find_image_file(const char* _file,int frame,const unsigned long* parts_pal_hsv)
{
	const char* file = get_texture_file(_file,frame,parts_pal_hsv);
	image* i = get_graph()->image_map[file];
	if (!i)
	{
		i = create_image_file(_file,frame,parts_pal_hsv);
		if (i)
			get_graph()->image_map[file] = i;
	}
	if (i)
		i->mark_use_image(g_time_now);
	return i;
}

void zgp_source::auto_clear_resource()
{
	unsigned long time = get_time_now();

	for (auto it = zgp_map.begin(); it != zgp_map.end(); )
	{
		image_zgp* img = it->second;
		if (img && TIME_NOTUSE(img,m_clear_zgp) )
		{
#ifdef _DEBUG_RESOURCE
			std::cout << "clear zgp " << it->first << " time: " << TIME(img) << std::endl;
#endif
			delete img;
			it = zgp_map.erase(it);
		}
		else
			++it;
	}	
}

void zgp_source::close_resource()
{
	for (auto it = zgp_map.begin(); it != zgp_map.end(); ++it)
		delete it->second;
	zgp_map.clear();
}

void zgp_source::dump_resource() const
{
	unsigned long time = get_time_now();
	for (auto it = zgp_map.begin(); it != zgp_map.end(); ++it)
	{
		const image_zgp* img = it->second;
		std::cout << it->first << " time: " << TIME(img) << std::endl;
	}
}

file_source* get_file_source_zgp()
{
	static zgp_source s_zgp_source;
	return &s_zgp_source;
}

