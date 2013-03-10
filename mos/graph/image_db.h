#ifndef __GRAPH_IMAGE_DB_H_
#define __GRAPH_IMAGE_DB_H_

#include "rect.h"
#include <string>

struct st_redirect
{
	std::string file_texture;
	std::string file_image;
	g_rect rc;
};

//button.jpg 1 --> ui.jpg (200,300,64,32)
const st_redirect* redirect_image_file(const char* file,int frame);
void regist_image_file(const char* file,int frame,st_redirect& rc);

#endif