// a2u.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "utf8.h"
#include "iconv/iconv.h"
#include "assert.h"

using namespace std;

namespace core
{
	wchar_t * UTF8ToUnicode( const char* str ) 
	{
		size_t len = strlen(str)+1; //包括后面的0也一并转换。
		wchar_t * des = new wchar_t[len+1];
		//iconv_t cd = iconv_open("UTF-16","UTF-8");
		iconv_t cd = iconv_open("UCS-2LE","UTF-8");
		if (cd == (iconv_t)-1)  
		{  
			printf("iconv_open UTF8->UCS-2LE error %d\n",errno) ;  
			return NULL;  
		}
		size_t deslen = (len+1)*2;
		char* pdes = (char*) des;
		const char* src = str;
		size_t sz = iconv(cd,&src,&len,&pdes,&deslen);
		if (sz == (size_t)-1)  
		{  
			printf("iconv UTF8->UCS-2LE error %d\n",errno) ;  
			return NULL;  
		} 
		iconv_close(cd);
		//u8_toucs(des,len+1,(char*)str,len);
		return des;
	}

	void UnicodeToANSI(const wchar_t* src,int srclen, char* des, int deslen) 
	{
		assert(deslen >= srclen * 2);		
		iconv_t cd = iconv_open("GBK","UCS-2LE");
		if (cd == (iconv_t)-1)  
		{  
			printf("iconv_open UCS-2LE->GBK error %d\n",errno) ;  
			return ;  
		}
		const char* s = (const char*)src;
		char* d = des;
		size_t sn = srclen * 2;
		size_t dn = deslen;
		size_t sz = iconv(cd,&s,&sn,&d,&dn);
		if (sz == (size_t)-1)  
		{  
			printf("iconv UCS-2LE->GBK error %d %d\n",*src,errno) ;  
			return;
		} 
		iconv_close(cd);
	}	

	const char* UnicodeCharToANSI(int ch)
	{
		static wchar_t t[2] = {0};
		static char buf[8] = {0};
		t[0] = ch;
		core::UnicodeToANSI(t,2,buf,4);
		return buf;
	}
}