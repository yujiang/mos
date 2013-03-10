
#ifndef MOS_H
#define MOS_H

extern window* g_window;
inline window* get_window()
{
	return g_window;
}

extern unsigned int g_time_now;
inline unsigned int get_time_now()
{
	return g_time_now;
}

bool init_window(const char* name,const char* title,st_window_param& st);
bool window_run();

#endif