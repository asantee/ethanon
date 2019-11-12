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
/* Windows GL lib defaults to 1.2 and  newer features must are available through GL extensions
* GL_GLEXT_PROTOTYPES must be defined before glext.h include (anywhere before).
*/

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#endif

#endif
