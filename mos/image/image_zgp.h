#ifndef IMAGE_ZGP_H
#define IMAGE_ZGP_H

//get zgp info...
bool regist_zgp(const char* file,int& dir,int& frame);

class file_source;
file_source* get_file_source_zgp();
#endif