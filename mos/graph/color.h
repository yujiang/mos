#ifndef __GRAPH_COLOR_H_
#define __GRAPH_COLOR_H_

#define G_COLOR_WHITE (g_color(-1))
#define G_COLOR_ALPHA(l) (unsigned char)((l)>>24)
#define G_RGB(r,g,b)			((unsigned long)( ((unsigned char)(r)|((unsigned short)((unsigned char)(g))<<8))|(((unsigned long)(unsigned char)(b))<<16) ))
#define G_ARGB(a,r,g,b)			((unsigned long)( ((unsigned char)(b)|((unsigned short)((unsigned char)(g))<<8))|(((unsigned long)(unsigned char)(r))<<16)|(((unsigned long)(unsigned char)(a))<<24) ))
#define G_GET_ARGB(c,a,r,g,b)	{a = (unsigned char)(c >> 24); r = (unsigned char)((c & 0x00ffffff) >> 16); g = (unsigned char)(((unsigned short)c) >> 8); b = (unsigned char)(c); }

#endif