#ifndef GS2D_GLES2_VIDEO_POLYGON_RENDERER_H_
#define GS2D_GLES2_VIDEO_POLYGON_RENDERER_H_

#include "../../PolygonRenderer.h"
#include "../../Platform/FileLogger.h"

#include "GLES2Video.h"

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
		const std::vector<unsigned int>& indices,
		const PolygonRenderer::POLYGON_MODE mode);
	~GLES2PolygonRenderer();

	void BeginRendering(const ShaderPtr& shader);
	void Render();
	void EndRendering();
};

typedef std::shared_ptr<GLES2PolygonRenderer> GLES2PolygonRendererPtr;

} // namespace gs2d

#endif
