#include "mapdata.h"
#include "../graph/image.h"
#include "../graph/graph.h"

#include <assert.h>

#define ASSERT2(a,b)
#define ASSERT3(a,b,c)

void *alloc_mask(int w, int h) 
{
	return new char [(w+3)/4*h];
}

int decompress (const void *in , void *out)
{
	unsigned char *op;
	const unsigned char *ip;
	unsigned long t;
	unsigned char *m_pos;

	op = (unsigned char *) out;
	ip = (const unsigned char *) in;

	if (*ip > 17) {
		t = *ip++ - 17;
		if (t < 4)
			goto match_next;
		do *op++ = *ip++; while (--t > 0);
		goto first_literal_run;
	}

	for(;;) {
		t = *ip++;
		if (t >= 16) goto match;
		if (t == 0) {
			while (*ip == 0) {
				t += 255;
				ip++;
			}
			t += 15 + *ip++;
		}

		* (unsigned long *) op = * ( unsigned long *) ip;
		op += 4; ip += 4;
		if (--t > 0)
		{
			if (t >= 4)
			{
				do {
					* (unsigned long * ) op = * ( unsigned long * ) ip;
					op += 4; ip += 4; t -= 4;
				} while (t >= 4);
				if (t > 0) do *op++ = *ip++; while (--t > 0);
			}
			else do *op++ = *ip++; while (--t > 0);
		}

first_literal_run:

		t = *ip++;
		if (t >= 16)
			goto match;

		m_pos = op - 0x0801;
		m_pos -= t >> 2;
		m_pos -= *ip++ << 2;

		*op++ = *m_pos++; *op++ = *m_pos++; *op++ = *m_pos;

		goto match_done;

		for(;;)
		{
match:
			if (t >= 64)
			{

				m_pos = op - 1;
				m_pos -= (t >> 2) & 7;
				m_pos -= *ip++ << 3;
				t = (t >> 5) - 1;

				goto copy_match;

			}
			else if (t >= 32)
			{
				t &= 31;
				if (t == 0)	{
					while (*ip == 0) {
						t += 255;
						ip++;
					}
					t += 31 + *ip++;
				}

				m_pos = op - 1;
				m_pos -= (* ( unsigned short * ) ip) >> 2;
				ip += 2;
			}
			else if (t >= 16) {
				m_pos = op;
				m_pos -= (t & 8) << 11;
				t &= 7;
				if (t == 0) {
					while (*ip == 0) {
						t += 255;
						ip++;
					}
					t += 7 + *ip++;
				}
				m_pos -= (* ( unsigned short *) ip) >> 2;
				ip += 2;
				if (m_pos == op)
					goto eof_found;
				m_pos -= 0x4000;
			}
			else {
				m_pos = op - 1;
				m_pos -= t >> 2;
				m_pos -= *ip++ << 2;
				*op++ = *m_pos++; *op++ = *m_pos;
				goto match_done;
			}

			if (t >= 6 && (op - m_pos) >= 4) {
				* (unsigned long *) op = * ( unsigned long *) m_pos;
				op += 4; m_pos += 4; t -= 2;
				do {
					* (unsigned long *) op = * ( unsigned long *) m_pos;
					op += 4; m_pos += 4; t -= 4;
				} while (t >= 4);
				if (t > 0) do *op++ = *m_pos++; while (--t > 0);
			}
			else {
copy_match:
				*op++ = *m_pos++; *op++ = *m_pos++;
				do *op++ = *m_pos++; while (--t > 0);
			}

match_done:

			t = ip[-2] & 3;
			if (t == 0)	break;

match_next:
			do *op++ = *ip++; while (--t > 0);
			t = *ip++;
		}
	}

eof_found:
	//   if (ip != ip_end) return -1;
	return (op - (unsigned char*)out);
}

//int _load_jpeg_ex(WBitmap *bmp,void *jpeg_buf,DWORD buf_size);

bool map_block::load_whole(mapdata*head)
{
	int n = num;
	//if (((head->get_width()+BLOCKW-1)/BLOCKW)*((head->get_height()+BLOCKH-1)/BLOCKH)<=(unsigned)n)
	//	return false;	// 地图没有那么多块
	//head may be release here!!!

	void *compress_data = 0;
	int i = 0, result = 0, len = 0;
	void *temp;
	m_MaskNum = head->get_mask_num(n);
	if (m_MaskNum < 0 || m_MaskNum > 100000)
		return false;

	//m_Image = new WBitmap;

	if( m_MaskNum > 0)
	{
		//int x, y;
		const int *maskindex = head->get_block_maskindex(n, len);
		//ASSERT(len == m_MaskNum);
		m_Mask=new WMask[m_MaskNum];
		//x=(head->get_width()+BLOCKW-1)/BLOCKW;
		//y=n/x;
		//x=n%x;
		for(i=0; i< m_MaskNum; i++)
		{
			const MASK_DATA *mask = head->get_mask(maskindex[i]);
			//printf("get mask %d %d\n",n,maskindex[i]);
			int size = (mask->rect.w+3)/4*mask->rect.h;
			temp = alloc_mask(mask->rect.w, mask->rect.h);
			result = decompress(mask->data,temp);
			ASSERT2(result == size,"decompress mask error!");
			m_Mask[i].Create((int)mask->rect.x-BLOCKW*x, (int)mask->rect.y-BLOCKH*y,mask->rect.w,mask->rect.h, temp);
			//delete temp;
		}
	}

	// load JPEG
	compress_data = head->get_block_image(n, len);
	
	//int jpeg_result = _load_jpeg_ex(m_Image, compress_data, len);

	//int w = (head->get_width()+BLOCKW-1)/BLOCKW;
	//int y = i / w;
	//int x = i % w;

	char filename[256];
	sprintf(filename,"%s_%02d%02d.jpg",head->m_map.c_str(),y,x);
	m_Image = get_graph()->find_image_raw(filename,0,0);
	if (!m_Image)
	{
		m_Image = image::create_image_file_buffer(filename,compress_data,len);
		get_graph()->set_image_raw(filename,m_Image);
	}
	else
		m_Image ->image_add_ref();

	delete [] compress_data;
	ASSERT3(jpeg_result==0,"地图jpeg Error",jpeg_result);
	// load cell
	//compress_data = head->get_block_cell(n, len);
	//m_Cell = 
	//memcpy(m_Cell, compress_data, len);
	//delete[] compress_data;
	return true;
}



bool mapdata::load_map(const std::string& map)
{
	int i;
	f = fopen(map.c_str(), "rb");
	if (!f)
		return false;

	fread(&_map_header, sizeof(_map_header), 1, f);

	if(_map_header.mapid != 'M1.0')
	{
		fclose(f);
		return false;
	}

	m_map = map;

	width = _map_header.width;
	height = _map_header.height;

	_map_position.block_num = get_block_width()*get_block_height();
	_blocks = new map_block[_map_position.block_num];
	for (int i=0; i<_map_position.block_num; i++)
	{
		int x,y;
		num_2_pos(i,x,y);
		_blocks[i].set_pos(x,y,i);
	}

	_map_position.block_offset = new int[_map_position.block_num];
	fread(_map_position.block_offset, 4, _map_position.block_num, f);

	fread(&_map_position.mask_seek, 4, 1, f);
	fread(&_map_position.mask_num, 4, 1, f);

	if (_map_position.mask_num > 0)
	{
		_map_position.mask_offset = new int[_map_position.mask_num];
		fread(_map_position.mask_offset, 4, _map_position.mask_num, f);

		_mask_data = new MASK_DATA[_map_position.mask_num];

		for(i = 0; i < _map_position.mask_num; i++)
		{
			fseek(f, _map_position.mask_offset[i], SEEK_SET);
			fread(&_mask_data[i].rect, sizeof(WRect), 1, f);
			fread(&_mask_data[i].len, 4, 1, f);
			_mask_data[i].data = new char[_mask_data[i].len];
			fread(_mask_data[i].data, _mask_data[i].len, 1, f);
		}
	}

	_block_info = new NORMAL_BLOCK_INFO[_map_position.block_num];
	for(i = 0; i < _map_position.block_num; i++)
	{
		NORMAL_BLOCK_INFO *p = &_block_info[i];
		fseek(f, _map_position.block_offset[i], SEEK_SET);
		fread(&p->mask_num, 4, 1, f);
		if(p->mask_num > 0)
		{
			p->mask_index = new int[p->mask_num];
			fread(p->mask_index, 4, p->mask_num, f);
		}
		else
			p->mask_index = 0;
		fseek(f, 4, SEEK_CUR);
		fread(&p->jpeg_len, 4, 1, f);
		p->jpeg_offset = ftell(f);

		p->cell_offset = p->jpeg_offset + p->jpeg_len + 8;
		fseek(f, p->cell_offset-4, SEEK_SET);
		fread(&p->cell_len, 4, 1, f);

		p->bright_offset = p->cell_offset + p->cell_len + 8;
		fseek(f, p->bright_offset-4, SEEK_SET);
		fread(&p->bright_len, 4, 1, f);

		fseek(f, 0, 0);
	}

	//loadblocks();
	return true;
}

void mapdata::load_all_blocks()
{
	//_blocks = new map_block[_map_position.block_num];
	for (int i=0; i<_map_position.block_num; i++)
	{
		_blocks[i].load_whole(this);
	}
}

mapdata::~mapdata()
{
	if (f)
	{
		fclose(f);
		f = 0;
	}

	delete []_mask_data;
	delete []_block_info;

	delete []_blocks;
}


void *mapdata::get_block_image(int n, int &len)
{
	fseek(f, _block_info[n].jpeg_offset, SEEK_SET);
	len = _block_info[n].jpeg_len;
	char *data = new char[_block_info[n].jpeg_len];
	fread(data, _block_info[n].jpeg_len, 1, f);
	return data;
}

//int save_jpg(const char* file,WBitmap *bmp);
//int save_png(const char* file,WMask *mask);
//
//void mapdata::save_jpg_dir(const char* dir)
//{
//	CreateDirectory(dir,NULL);
//	int w = (width+BLOCKW-1)/BLOCKW;
//	for (int i=0; i<_map_position.block_num; i++)
//	{
//		char buf[128];
//		int y = i / w;
//		int x = i % w;
//		sprintf(buf,"%s\\%02d%02d.jpg",dir,y,x);
//		printf("save... %s\n",buf);
//		map_block* p = &_blocks[i];
//		save_jpg(buf,p->m_Image);
//
//		for (int j=0; j<p->m_MaskNum;j++)
//		{
//			sprintf(buf,"%s\\%02d%02d_%02d.jpg",dir,y,x,j);
//			save_png(buf,&p->m_Mask[j]);
//		}
//	}
//
//}

map_block::~map_block()
{
	if (m_Image)
	{
		m_Image->image_release();
		m_Image = 0;
	}
}

void WMask::Create(int x,int y,int _w,int _h,void *ptr)
{
	kx = x;
	ky = y;
	w = _w;
	h = _h;

	wm = (w+3)/4;
	//printf("WMask::Create x %d y %d w %d h %d\n",x,y,_w,_h);
	mask = (BYTE *) ptr; //alpha channel
	if (ky < 0)
	{
		h -= -ky;
		mask += -ky * wm;
		ky = 0;
	}
	assert(kx >= 0 && ky >= 0 && w >=0 && h >= 0 );
	if (ky + h > BLOCKH)
		h = BLOCKH - ky;

	assert(kx + w <= BLOCKW && ky + h <= BLOCKH);
	//bmp = b;
}
