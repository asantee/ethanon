#ifndef GS2D_GLES2INCLUDE_H_
#define GS2D_GLES2INCLUDE_H_

#ifdef __APPLE__
 #include "TargetConditionals.h"
#endif

#if defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_IPHONE)
  #include <OpenGLES/ES2/gl.h>
  #include <OpenGLES/ES2/glext.h>
#elif defined(__ANDROID__)
  #include <GLES2/gl2.h>
  #include <GLES2/gl2ext.h>
#endif

#endif
