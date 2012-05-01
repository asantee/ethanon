/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2012 Andre Santee
 http://www.asantee.net/ethanon/

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the
	Software without restriction, including without limitation the rights to use, copy,
	modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
	and to permit persons to whom the Software is furnished to do so, subject to the
	following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
--------------------------------------------------------------------------------------*/

#ifndef GLES2_RECT_RENDERER_H_
#define GLES2_RECT_RENDERER_H_

#ifdef APPLE_IOS
  #include <OpenGLES/ES2/gl.h>
  #include <OpenGLES/ES2/glext.h>
#endif

#ifdef ANDROID
  #include <GLES2/gl2.h>
  #include <GLES2/gl2ext.h>
#endif

#include "../../Platform/Platform.h"
#include "../../Platform/FileLogger.h"

namespace gs2d {

class GLES2RectRenderer
{
	GLuint m_vertexBuffer;
	GLuint m_indexBuffer;
	
	mutable struct LatestLocations
	{
		LatestLocations() :
			positionLocation(-1),
			texCoordLocation(-1)
		{
		}
		int positionLocation;
		int texCoordLocation;
	} m_latestLocations;

public:
	GLES2RectRenderer(const Platform::FileLogger& logger);
	void BeginFastDraw(const int positionLocation, const int texCoordLocation, const Platform::FileLogger& logger) const;
	void FastDraw(const Platform::FileLogger& logger) const;
	void EndFastDraw(const Platform::FileLogger& logger) const;
	void SetPositionLocations(const int positionLocation, const int texCoordLocation, const Platform::FileLogger& logger) const;
	void Draw(const int positionLocation, const int texCoordLocation, const Platform::FileLogger& logger) const;

	void BeginFastDrawFromClientMem(const int positionLocation, const int texCoordLocation, const Platform::FileLogger& logger) const;
	void FastDrawFromClientMem(const Platform::FileLogger& logger) const;
	void EndFastDrawFromClientMem(const Platform::FileLogger& logger) const;
	void DrawFromClientMem(const int positionLocation, const int texCoordLocation, const Platform::FileLogger& logger) const;
};

} // namespace gs2d

#endif