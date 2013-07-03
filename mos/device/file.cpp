#include "file.h"
#include "stdio.h"
#include <assert.h>

#define RESOURCE_PATH "resources"

#pragma warning(disable:4996)

const char* get_windows_font(const char* file)
{
	static char buf[128];
	sprintf(buf,"c:/Windows/Fonts/%s",file);
	return buf;
}

const char* my_itoa(int num)
{
	static char buf[64];
	//itoa(num,buf,10);
	sprintf(buf,"%04d",num);
	return buf;
}

char* read_file(const char* file,size_t& size)
{
	MOS_FILE_STRUCT* f = mfopen(file,"rb");
	if (!f)
		return 0;
	mfseek(f, 0, SEEK_END);
	size = mftell(f);
	if (size == 0)
		return 0;
	mfseek(f, 0, SEEK_SET);
	char* buf = new char[size];
	size_t sz = mfread(buf,1,size,f);
	assert(sz == size);
	return buf;
}

char* read_imagefile(const char* file,size_t& size)
{
	return read_file(get_resourcefile(file),size);
}

const char* get_resourcefile(const char* file)
{
	static char buf[256];
	sprintf(buf,RESOURCE_PATH"/%s",file);
	return buf;
}

//////////////////////////////////////////////////////////////////////////
//简单封装，之后可能用到打包,cache等
struct MOS_FILE_STRUCT
{
	MOS_FILE_STRUCT(FILE* _f):f(_f){}
	~MOS_FILE_STRUCT(){
		if (f)
		{
			fclose(f);
			f = 0;
		}
	}
	FILE* f;
};

MOS_FILE_HANDLE mfopen(const char* file,const char* mode)
{
	//for resource.
	//mode must "rb" can not support write.
	FILE* f = fopen(file,"rb");
	if (!f)
		return NULL;
	return new MOS_FILE_STRUCT(f);
}

MOS_FILE_HANDLE mfopen_resource(const char* file,const char* mode)
{
	return mfopen(get_resourcefile(file),mode);
}

size_t mfread ( void   *buffer,  size_t size,  size_t count,  MOS_FILE_HANDLE stream) 
{
	return fread(buffer,size,count,stream->f);
}

int mfseek ( MOS_FILE_HANDLE stream, long int offset, int origin )
{
	return fseek(stream->f,offset,origin);
}

int mfclose ( MOS_FILE_HANDLE stream )
{
	int rt = fclose(stream->f);
	delete stream;
	return rt;
}

long int mftell ( MOS_FILE_HANDLE stream )
{
	return ftell(stream->f);
}