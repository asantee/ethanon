#ifndef GS2D_GLINCLUDE_H_
#define GS2D_GLINCLUDE_H_

#ifdef __APPLE__
 #include "TargetConditionals.h"
#endif

//set macro NOMINMAX because windows.h conflicts with std macros
#ifdef _WIN32
 #define NOMINMAX
 #include <Windows.h>
#endif

#if defined(TARGET_OS_MAC)
 #include <OpenGL/gl3.h>
 #include <OpenGL/gl3ext.h>
#else

#include <GL/glew.h>

#endif

#endif
