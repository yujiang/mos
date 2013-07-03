#ifndef __DEVICE_FILE_H_
#define __DEVICE_FILE_H_

//一次性读取所有内容
char* read_imagefile(const char* file,size_t& size);
#include <memory.h>
//read from memory
inline void mread(void* dest,char*& src,size_t size)
{
	memcpy(dest,src,size);
	src += size;
}

//file wrap
struct MOS_FILE_STRUCT;
typedef MOS_FILE_STRUCT* MOS_FILE_HANDLE;

MOS_FILE_HANDLE mfopen(const char* file,const char* mode);
MOS_FILE_HANDLE mfopen_resource(const char* file,const char* mode);
size_t mfread ( void   *buffer,  size_t size,  size_t count,  MOS_FILE_HANDLE stream) ;
int mfseek ( MOS_FILE_HANDLE stream, long int offset, int origin );
int mfclose ( MOS_FILE_HANDLE stream );
long int mftell ( MOS_FILE_HANDLE stream );

//some func for path
const char* get_resourcefile(const char* file);
const char* get_windows_font(const char* file);

//itoa not need buffer
const char* my_itoa(int num);

#endif