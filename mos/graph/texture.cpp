#include "graph.h"
#include "texture.h"
#include "graph.h"
#include "device/window_render.h"
#include "mos.h"
#include <assert.h>

DECLARE_COUNTER(texture)

texture::texture()
{
	m_alloc_id = m_counter.s_alloc_num;
	m_name = 0;
}

texture::~texture()
{
}

void texture_sub::mark_use_texture(unsigned int time)
{
	m_time_use = time;
	m_tex->m_texture->mark_use_texture(time);
}

texture_sub* texture_mul::create_line_sub(st_line& line,int w)
{
	g_rect rc = line.rc;
	rc.l = line.rc.r;
	rc.r = rc.l + w;
	line.rc.r = rc.r;
	texture_sub* sub = new texture_sub(this,rc);
	line.subs.push_back(sub);
	m_sub_num++;
	return sub;
}

texture_sub* texture_mul::add_image_ontexture(const image* img,const g_rect& rc) 
{
	texture_sub* sub = create_sub(img,rc);
	sub->m_rc_tex = sub->m_rc;
	sub->m_rc_tex.r = sub->m_rc_tex.l + rc.width();
	sub->m_rc_tex.b = sub->m_rc_tex.t + rc.height();
	m_texture->draw_image_ontexture(sub->m_rc.l,sub->m_rc.t,img,&rc);
	assert(sub);
	return sub;
}

texture_sub* texture_mul::create_sub(const image* img,const g_rect& rc) 
{
	int w = rc.width();
	int h = rc.height();
	assert(w <= m_texture->get_tex_width() && h <= m_texture->get_tex_height());

	for (auto it = m_lines.begin(); it != m_lines.end(); ++it)
	{
		st_line& line = *it;
		if (line.rc.height() >= h)
		{
			for (auto it2 = line.subs.begin(); it2 != line.subs.end(); ++it2)
			{
				auto sub = *it2;
				if (sub->m_released && sub->m_rc.width() >= w)
				{
					sub->m_released = false;
					m_release_sub--;
					return sub;
				}
			}
			if (line.rc.r + w <= m_texture->get_tex_width())
			{
				return create_line_sub(line,w);
			}
		}
	}

	int lh = get_free_height();
	if (lh >= h) 
	{
		st_line t;
		t.rc = g_rect(0,get_last_height(),0,get_last_height()+h);
		m_lines.push_back(t);
		st_line& line = m_lines.back();
		return create_line_sub(line,w);
	}
	return 0;
}

bool texture_mul::find_free(int w,int h) const
{
	if (get_free_height() >= h) 
		return true;
	for (auto it = m_lines.begin(); it != m_lines.end(); ++it)
	{
		const st_line& line = *it;
		if (line.rc.height() >= h)
		{
			if (line.rc.r + w <= m_texture->get_tex_width())
				return true;
			for (auto it2 = line.subs.begin(); it2 != line.subs.end(); ++it2)
			{
				auto sub = *it2;
				if (sub->m_released && sub->m_rc.width() >= w)
					return true;
			}
		}
	}
	return false;
}

int texture_mul::get_last_height() const
{
	if (m_lines.empty())
		return 0;
	auto it = m_lines.back();
	return it.rc.b;
}

int texture_mul::get_free_height() const
{
	return m_texture->get_tex_height() - get_last_height();
}

texture_mul::~texture_mul()
{
	delete m_texture;
	m_texture = 0;
	for (auto it = m_lines.begin(); it != m_lines.end(); ++it)
	{
		st_line& line = *it;
		for (auto it2 = line.subs.begin(); it2 != line.subs.end(); ++it2)
		{
			auto sub = *it2;
			delete sub;
		}
	}
	m_lines.clear();
}

void texture_mul::merge()
{
	for (auto it = m_lines.begin(); it != m_lines.end(); ++it)
	{
		st_line& line = *it;
		for (auto it2 = line.subs.begin(); it2 != line.subs.end();)
		{
			auto sub = *it2;
			if (sub->m_released)
			{
				if (it2+1 == line.subs.end())
				{
					line.rc.r = sub->m_rc.l;
					delete sub;
					m_sub_num--;
					m_release_sub--;
					line.subs.erase(it2);
					break;
				}
				else
				{
					auto sub2 = *(it2+1);
					if (sub2->m_released)
					{
						sub2->m_rc.l = sub->m_rc.l;
						delete sub;
						m_sub_num--;
						m_release_sub--;
						it2 = line.subs.erase(it2);
						continue;
					}
				}
			}
			 ++it2;
		}
	}
}

void texture_mul::released_sub(texture_sub* sub)
{
	sub->release();
	m_release_sub++;
	if (m_release_sub >= 10 && m_release_sub * 2 > m_sub_num)
	{
		//int n1 = m_release_sub;
		//int n2 = m_sub_num; 
		merge();
		//printf("texture_mul:merged %d/%d -- >%d/%d\n",n1,n2,m_release_sub,m_sub_num);
	}
}
