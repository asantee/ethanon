#ifndef GS2D_VIDEO_GLPOLYGON_RENDERER_H_
#define GS2D_VIDEO_GLPOLYGON_RENDERER_H_

#include "../../PolygonRenderer.h"

#include "GLInclude.h"

namespace gs2d {

class GLPolygonRenderer : public PolygonRenderer
{
	GLuint m_vertexArrayObject;
	GLuint m_arrayBufferObject;
	GLuint m_arrayElementObject;

	PolygonRenderer::POLYGON_MODE m_mode;
	GLenum m_glMode;

	std::vector<unsigned int> m_indices;
	std::vector<PolygonRenderer::Vertex> m_vertices;

public:
	GLPolygonRenderer(
		const std::vector<PolygonRenderer::Vertex>& vertices,
		const std::vector<uint32_t>& indices,
		const PolygonRenderer::POLYGON_MODE mode);
	
	~GLPolygonRenderer();

	void BeginRendering(const ShaderPtr& shader);
	void Render();
	void EndRendering();
};

} // namespace sprite

#endif
