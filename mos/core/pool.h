#ifndef __CORE_POOL_H_
#define __CORE_POOL_H_

#include <list>

namespace core
{

template <typename T>
class temp_pool
{
public:
	std::list<T*> m_list_used;
	std::list<T*> m_list_free;
public:
	~temp_pool(){
		for (auto it = m_list_free.begin(); it != m_list_free.end(); ++it)
			delete (*it);
		for (auto it = m_list_used.begin(); it != m_list_used.end(); ++it)
			delete (*it);
	}
	void clear_all()
	{
		for (auto it = m_list_used.begin(); it != m_list_used.end(); ++it)
			(*it)->reinit();
		m_list_free.insert(m_list_free.end(),m_list_used.begin(),m_list_used.end());
		m_list_used.clear();
	}
	T* construct()
	{
		T* t;
		if (m_list_free.empty())
		{
			t = new T;
		}
		else
		{
			t = m_list_free.back();
			m_list_free.pop_back();
		}
		m_list_used.push_back(t);
		return t;
	}
};

};

#endif