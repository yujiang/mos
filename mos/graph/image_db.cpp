#include "image_db.h"
#include <string>
#include "device/file.h"
#include <unordered_map>
#include <assert.h>

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

const char* my_itoa(int num)
{
	static char buf[64];
	//itoa(num,buf,10);
	sprintf(buf,"%04d",num);
	return buf;
}

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
