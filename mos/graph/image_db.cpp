#include "image_db.h"
#include <string>
#include "device/file.h"
#include <unordered_map>
#include <assert.h>

using namespace std;

//button.jpg 1 --> ui.jpg (200,300,64,32)
struct st_file_frame
{
	std::string file;
	int frame;
};

int g_file_num = 0;
std::unordered_map<std::string,int> g_hm_file;

int get_file_hash(const char* file)
{
	auto it = g_hm_file.find(file);
	if (it != g_hm_file.end())
		return it->second;
	g_hm_file[file] = g_file_num;
	return g_file_num++;
}

std::unordered_map<int,st_redirect> g_hm_redirect;

void regist_image_file(const char* file,int frame, st_redirect& rc)
{
	assert(frame < 1000);
	//保证唯一性
	rc.id_texture = (std::string)(file) + my_itoa(frame);
	g_hm_redirect[get_file_hash(file)*1000+frame] = rc;
}

const st_redirect* redirect_image_file(const char* file,int frame)
{
	auto it = g_hm_redirect.find(get_file_hash(file)*1000+frame);
	if (it != g_hm_redirect.end())
		return &it->second;
	return 0;
}

std::unordered_map<string,string> g_hm_image_palette;

void regist_image_palette(const char* image,const char* palette_file)
{
	g_hm_image_palette[image] = palette_file;
}

bool is_image_use_palette(const char* image)
{
	return get_image_palette(image) != 0;
}

const char* get_image_palette(const char* image)
{
	auto it = g_hm_image_palette.find(image);
	if (it != g_hm_image_palette.end())
		return it->second.c_str();
	return 0;
}