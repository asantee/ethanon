#ifndef GS2D_GLES2_VIDEO_POLYGON_RENDERER_H_
#define GS2D_GLES2_VIDEO_POLYGON_RENDERER_H_

#ifdef APPLE_IOS
  #include <OpenGLES/ES2/gl.h>
#endif

#ifdef ANDROID
  #include <GLES2/gl2.h>
#endif

#include "../../PolygonRenderer.h"

namespace gs2d {

class GLES2PolygonRenderer : public PolygonRenderer
{
	GLuint m_vertexBuffer;
	GLuint m_indexBuffer;
	
	GLenum m_glMode;

	GLsizei m_vertexCount;

public:
	GLES2PolygonRenderer(
		const std::vector<PolygonRenderer::Vertex>& vertices,
		const std::vector<uint32_t>& indices,
		const PolygonRenderer::POLYGON_MODE mode);
	~GLES2PolygonRenderer();

	void BeginRendering(const ShaderPtr& shader);
	void Render();
	void EndRendering();
};

typedef std::shared_ptr<GLES2PolygonRenderer> GLES2PolygonRendererPtr;

} // namespace gs2d

#endif
