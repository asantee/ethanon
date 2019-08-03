#ifndef GS2D_GLINCLUDE_H_
#define GS2D_GLINCLUDE_H_

#ifdef __APPLE__
 #include "TargetConditionals.h"
#endif

#if defined(TARGET_OS_MAC)
 #include <OpenGL/gl3.h>
 #include <OpenGL/gl3ext.h>
#else
 #include <GL/gl.h>
 #include <GL/glu.h>
#endif

#endif
