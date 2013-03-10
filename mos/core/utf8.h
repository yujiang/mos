#ifndef __CORE_UTF8_H_
#define __CORE_UTF8_H_

#include <string>

//typedef unsigned short wchar_t;

namespace core
{
	wchar_t * UTF8ToUnicode( const char* str );
	void UnicodeToANSI(const wchar_t* src,int srclen, char* des, int deslen) ;
	const char* UnicodeCharToANSI(int ch);
}

#endif