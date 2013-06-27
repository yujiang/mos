#pragma once

#ifndef _COUNTER_H_
#define _COUNTER_H_

template <typename T>
class counter {
public:
	static int s_count_num;
	static int s_alloc_num;
	static int s_released_num;
	counter() {
		s_count_num++;
		s_alloc_num++;
	}
	void released() {
		s_released_num ++;
	}
	~counter() {
		s_count_num --;
	}
};



#define DECLARE_COUNTER(T) \
int counter<T>::s_count_num = 0; \
int counter<T>::s_alloc_num = 0; \
int counter<T>::s_released_num = 0;

#endif