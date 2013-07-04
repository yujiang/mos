#pragma once

#include <windows.h>
#include <string>

struct NORMAL_MAP_HEADER
{
	int mapid;
	DWORD width;
	DWORD height;
};

struct NORMAL_MAP_POSITION
{
	int block_num;
	int *block_offset;
	int mask_seek;
	int mask_num;
	int *mask_offset;
};

struct NORMAL_MAP_BLOCK
{
	int mask_num;
	int *mask_index;
	int jpeg_len;
	char *jpeg_data;
	char *cell_data;
	char *bright_data;
};

struct NORMAL_BLOCK_INFO
{
	int mask_num;
	int *mask_index;
	int jpeg_len;
	int jpeg_offset;
	int cell_len;
	int cell_offset;
	int bright_len;
	int bright_offset;
	NORMAL_BLOCK_INFO(){mask_index = 0;}
	~NORMAL_BLOCK_INFO(){
		if(mask_index)
			delete mask_index;
	}
};

struct WRect {
	int x;
	int y;
	int w;
	int h;
};

struct MASK_DATA{
	WRect rect;
	int len;
	void *data;
	MASK_DATA(){data = 0;}
	~MASK_DATA(){
		if(data)
			delete data;
	}
};


#define BLOCKW	640
#define BLOCKH	480

class mapdata;

class WMask;
//class WBitmap;
class image;

class WMask
{
public:
	//const image* bmp;
	int kx,ky,w,h;
	BYTE * mask;
	int wm;
	void Create(int x,int y,int _w,int _h,void *ptr);
	//BYTE* Render8888();
	//BYTE* Render888();
};


class map_block
{
public:
	bool load_whole(mapdata *head);
	int m_MaskNum;

	image* m_Image;
	WMask *m_Mask;

	int x,y,num;
	void set_pos(int _x,int _y,int _num)
	{
		x = _x;
		y = _y;
		num = _num;
	}

	//unsigned char *m_Cell; //×èµ²ÅÐ¶Ï

	//unsigned char *m_BrightLayer; 
	//unsigned char *m_LightData; //ÎÞÓÃ
	map_block()
	{
		m_Image = 0;
		m_Mask = 0;
	}
	~map_block();
};

class mapdata
{
public:
	FILE *f;
	mapdata()
	{
		f = 0;
		_mask_data = 0;
		_block_info = 0;
	}
	~mapdata();
	NORMAL_MAP_HEADER _map_header;
	NORMAL_MAP_POSITION _map_position;
	MASK_DATA* _mask_data ;
	NORMAL_BLOCK_INFO* _block_info;
	int width,height;
	int get_width() const{return width;}
	int get_height() const{return height;}
	

	void *get_block_image(int n, int &len);
	//unsigned char *get_block_cell(int n, int &len);

	const int *get_block_maskindex(int n, int &len)
	{
		len = _block_info[n].mask_num;
		return _block_info[n].mask_index;
	}
	const MASK_DATA *get_mask(int index){ return &_mask_data[index];}
	int get_mask_num(int n) const{ return _block_info[n].mask_num;} 


	bool load_map(const std::string& map);	
	std::string m_map;

	void load_all_blocks();

	map_block* _blocks;
	int get_block_width() const{
		return (width+BLOCKW-1)/BLOCKW;
	}
	int get_block_height() const{
		return (height+BLOCKH-1)/BLOCKH;
	}
	void num_2_pos(int num,int& x,int& y){
		int w = get_block_width();
		y = num / w;
		x = num % w;
	}
	int pos_2_num(int x,int y){
		return y * get_block_width() + x;
	}	
	map_block* get_block(int x,int y){
		if (x < 0 || x >= get_block_width())
			return NULL;
		if (y < 0 || y >= get_block_height())
			return NULL;
		return &_blocks[pos_2_num(x,y)];
	}

	//void save_jpg_dir(const char* dir);
};