#ifndef __DEVICE_FILE_H_
#define __DEVICE_FILE_H_

char* read_imagefile(const char* file,size_t& size);
const char* get_resourcefile(const char* file);

const char* my_itoa(int num);
const char* get_windows_font(const char* file);

#endif