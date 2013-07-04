
#ifndef MOS_H
#define MOS_H
class window;
extern window* g_window;
inline window* get_window()
{
	return g_window;
}

class window_render;
window_render* get_render();

//dont support gdi anymore
class window_render_gl;
window_render_gl* get_render_gl();

extern unsigned int g_time_now;
inline unsigned int get_time_now()
{
	return g_time_now;
}

struct st_window_param;
bool init_window(const char* name,const char* title,st_window_param& st);
bool window_run();

#endif