#include "file.h"
#include "stdio.h"
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
	FILE* f = fopen(file,"rb");
	if (!f)
		return 0;
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	if (size == 0)
		return 0;
	fseek(f, 0, SEEK_SET);
	char* buf = new char[size];
	size_t sz = fread(buf,1,size,f);
	if (sz == size)
		return buf;
	delete buf;
	return 0;
}

char* read_imagefile(const char* file,size_t& size)
{
	return read_file(get_resourcefile(file),size);
}

const char* get_resourcefile(const char* file)
{
	static char buf[128];
	sprintf(buf,"resources/%s",file);
	return buf;
}