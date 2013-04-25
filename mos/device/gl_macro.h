#ifndef __GL_MACRO_H_
#define __GL_MACRO_H_

#define CHECK_GL_ERROR_DEBUG() \
	do { \
	GLenum __error = glGetError(); \
	if(__error) { \
	printf("OpenGL error 0x%04X in %s %s %d\n", __error, __FILE__, __FUNCTION__, __LINE__); \
	} \
	} while (false)

#endif