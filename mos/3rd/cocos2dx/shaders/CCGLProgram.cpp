/****************************************************************************
Copyright 2012 cocos2d-x.org
Copyright 2011 Jeff Lamarche
Copyright 2012 Goffredo Marocchi
Copyright 2012 Ricardo Quesada

http://www.cocos2d-x.org
 
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN false EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "CCGLProgram.h"
#include "ccGLStateCache.h"
#include <stdlib.h>
//#include "ccMacros.h"
//#include "support/data_support/uthash.h"
// extern
//#include "kazmath/GL/matrix.h"
//#include "kazmath/kazmath.h"

NS_CC_BEGIN


CCGLProgram::CCGLProgram()
: m_uProgram(0)
, m_uVertShader(0)
, m_uFragShader(0)
, m_bUsesTime(false)
{
}

CCGLProgram::~CCGLProgram()
{
    CCLOGINFO("cocos2d: %s %d deallocing 0x%X", __FUNCTION__, __LINE__, this);

    // there is no need to delete the shaders. They should have been already deleted.
    CCAssert(m_uVertShader == 0, "Vertex Shaders should have been already deleted");
    CCAssert(m_uFragShader == 0, "Fragment Shaders should have been already deleted");

    if (m_uProgram) 
    {
        ccGLDeleteProgram(m_uProgram);
    }

}

bool CCGLProgram::initWithVertexShaderByteArray(const GLchar* vShaderByteArray, const GLchar* fShaderByteArray)
{
    m_uProgram = glCreateProgram();
    CHECK_GL_ERROR_DEBUG();

    m_uVertShader = m_uFragShader = 0;

    if (vShaderByteArray)
    {
        if (!compileShader(&m_uVertShader, GL_VERTEX_SHADER, vShaderByteArray))
        {
            CCLOG("cocos2d: ERROR: Failed to compile vertex shader");
        }
    }

    // Create and compile fragment shader
    if (fShaderByteArray)
    {
        if (!compileShader(&m_uFragShader, GL_FRAGMENT_SHADER, fShaderByteArray))
        {
            CCLOG("cocos2d: ERROR: Failed to compile fragment shader");
        }
    }

    if (m_uVertShader)
    {
        glAttachShader(m_uProgram, m_uVertShader);
    }
    CHECK_GL_ERROR_DEBUG();

    if (m_uFragShader)
    {
        glAttachShader(m_uProgram, m_uFragShader);
    }
    //m_pHashForUniforms = NULL;
    
    CHECK_GL_ERROR_DEBUG();

    return true;
}

//bool CCGLProgram::initWithVertexShaderFilename(const char* vShaderFilename, const char* fShaderFilename)
//{
//    const GLchar * vertexSource = (GLchar*) CCString::createWithContentsOfFile(CCFileUtils::sharedFileUtils()->fullPathFromRelativePath(vShaderFilename))->getCString();
//    const GLchar * fragmentSource = (GLchar*) CCString::createWithContentsOfFile(CCFileUtils::sharedFileUtils()->fullPathFromRelativePath(fShaderFilename))->getCString();
//
//    return initWithVertexShaderByteArray(vertexSource, fragmentSource);
//}
//
//const char* CCGLProgram::description()
//{
//    return CCString::createWithFormat("<CCGLProgram = %08X | Program = %i, VertexShader = %i, FragmentShader = %i>", this, m_uProgram, m_uVertShader, m_uFragShader)->getCString();
//}

bool CCGLProgram::compileShader(GLuint * shader, GLenum type, const GLchar* source)
{
    GLint status;
 
    if (!source)
    {
        return false;
    }
    
    const GLchar *sources[] = {
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32 && CC_TARGET_PLATFORM != CC_PLATFORM_LINUX && CC_TARGET_PLATFORM != CC_PLATFORM_MAC)
        (type == GL_VERTEX_SHADER ? "precision highp float;\n" : "precision mediump float;\n"),
#endif
        "uniform mat4 CC_PMatrix;\n"
        "uniform mat4 CC_MVMatrix;\n"
        "uniform mat4 CC_MVPMatrix;\n"
        "uniform vec4 CC_Time;\n"
        "uniform vec4 CC_SinTime;\n"
        "uniform vec4 CC_CosTime;\n"
        "uniform vec4 CC_Random01;\n"
        "//CC INCLUDES END\n\n",
        source,
    };

    *shader = glCreateShader(type);
    glShaderSource(*shader, sizeof(sources)/sizeof(*sources), sources, NULL);
    glCompileShader(*shader);

    glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);

    if (! status)
    {
        GLsizei length;
		glGetShaderiv(*shader, GL_SHADER_SOURCE_LENGTH, &length);
		GLchar* src = (GLchar *)malloc(sizeof(GLchar) * length);
		
		glGetShaderSource(*shader, length, NULL, src);
		CCLOG("cocos2d: ERROR: Failed to compile shader:\n%s", src);
        
        if (type == GL_VERTEX_SHADER)
        {
            CCLOG("cocos2d: %s", vertexShaderLog());
        }
        else
        {
            CCLOG("cocos2d: %s", fragmentShaderLog());
        }
        free(src);

        abort();
    }
    return (status == GL_TRUE);
}

void CCGLProgram::addAttribute(const char* attributeName, GLuint index)
{
    glBindAttribLocation(m_uProgram, index, attributeName);
}

bool CCGLProgram::link()
{
    CCAssert(m_uProgram != 0, "Cannot link invalid program");
    
    GLint status = GL_TRUE;
    
    glLinkProgram(m_uProgram);

    if (m_uVertShader)
    {
        glDeleteShader(m_uVertShader);
    }
    
    if (m_uFragShader)
    {
        glDeleteShader(m_uFragShader);
    }
    
    m_uVertShader = m_uFragShader = 0;
	
#if DEBUG
    glGetProgramiv(m_uProgram, GL_LINK_STATUS, &status);
	
    if (status == GL_FALSE)
    {
        CCLOG("cocos2d: ERROR: Failed to link program: %i", m_uProgram);
        ccGLDeleteProgram(m_uProgram);
        m_uProgram = 0;
    }
#endif
	
    return (status == GL_TRUE);
}

void CCGLProgram::use()
{
    ccGLUseProgram(m_uProgram);
}

const char* CCGLProgram::logForOpenGLObject(GLuint object, GLInfoFunction infoFunc, GLLogFunction logFunc)
{
	GLint logLength = 0, charsWritten = 0;

	infoFunc(object, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength < 1)
		return 0;

	static char buf[10*1024];
	if (logLength >= sizeof(buf))
		return "no buf";
	logFunc(object, sizeof(buf), &charsWritten, buf);
	return buf;
}

const char* CCGLProgram::vertexShaderLog()
{
    return this->logForOpenGLObject(m_uVertShader, (GLInfoFunction)&glGetShaderiv, (GLLogFunction)&glGetShaderInfoLog);
}

const char* CCGLProgram::fragmentShaderLog()
{
    return this->logForOpenGLObject(m_uFragShader, (GLInfoFunction)&glGetShaderiv, (GLLogFunction)&glGetShaderInfoLog);
}

const char* CCGLProgram::programLog()
{
    return this->logForOpenGLObject(m_uProgram, (GLInfoFunction)&glGetProgramiv, (GLLogFunction)&glGetProgramInfoLog);
}

// Uniform cache


void CCGLProgram::reset()
{
    m_uVertShader = m_uFragShader = 0;
    //memset(m_uUniforms, 0, sizeof(m_uUniforms));
    

    // it is already deallocated by android
    //ccGLDeleteProgram(m_uProgram);
    m_uProgram = 0;

}

NS_CC_END
