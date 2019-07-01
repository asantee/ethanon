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
#include "../../Sprite.h"

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
	void Draw(const int positionLocation, const int texCoordLocation, const Sprite::RECT_MODE mode, const Platform::FileLogger& logger) const;

	void BeginFastDrawFromClientMem(const int positionLocation, const int texCoordLocation, const Platform::FileLogger& logger) const;
	void FastDrawFromClientMem(const Platform::FileLogger& logger) const;
	void EndFastDrawFromClientMem(const Platform::FileLogger& logger) const;
	void DrawFromClientMem(const int positionLocation, const int texCoordLocation, const Platform::FileLogger& logger) const;
};

} // namespace gs2d

#endif
