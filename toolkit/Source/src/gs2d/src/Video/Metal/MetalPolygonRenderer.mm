#include "MetalPolygonRenderer.h"

#include "MetalVideo.h"

namespace gs2d {

MetalPolygonRenderer::MetalPolygonRenderer(
	MetalVideo* metalVideo,
	const std::vector<PolygonRenderer::Vertex>& vertices,
	const std::vector<uint32_t>& indices,
	const PolygonRenderer::POLYGON_MODE mode) :
	m_metalVideo(metalVideo)
{
	m_device = m_metalVideo->GetDevice();
	m_vertices.resize(vertices.size());
	for (uint32_t t = 0; t < vertices.size(); t++)
	{
		m_vertices[t].position.x = vertices[t].pos.x;
		m_vertices[t].position.y = vertices[t].pos.y;
		m_vertices[t].position.z = vertices[t].pos.z;
		m_vertices[t].position.w = 1.0f;
		m_vertices[t].texCoord.x = vertices[t].texCoord.x;
		m_vertices[t].texCoord.y = vertices[t].texCoord.y;
		m_vertices[t].texCoord.z = vertices[t].texCoord.x;
		m_vertices[t].texCoord.w = vertices[t].texCoord.y;
		m_vertices[t].normal.x = vertices[t].normal.x;
		m_vertices[t].normal.y = vertices[t].normal.y;
		m_vertices[t].normal.z = vertices[t].normal.z;
		m_vertices[t].normal.w = 1.0f;
	}

	m_vertexBuffer = [m_device newBufferWithBytes:&m_vertices[0] length:(sizeof(MetalVertex) * m_vertices.size()) options:MTLResourceOptionCPUCacheModeDefault];
}

MetalPolygonRenderer::~MetalPolygonRenderer()
{
}

void MetalPolygonRenderer::BeginRendering(const ShaderPtr& shader)
{
	shader->SetShader();
	[m_metalVideo->GetRenderCommandEncoder() setVertexBuffer:m_vertexBuffer offset:0 atIndex:0];
}

void MetalPolygonRenderer::Render()
{
	[m_metalVideo->GetRenderCommandEncoder() drawPrimitives:MTLPrimitiveTypeTriangleStrip vertexStart:0 vertexCount:4];
}

void MetalPolygonRenderer::EndRendering()
{
}

} // namespace gs2d
