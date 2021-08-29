#include "MetalPolygonRenderer.h"

namespace gs2d {

PolygonRendererPtr PolygonRenderer::Create(
	const std::vector<Vertex>& vertices,
	const std::vector<uint32_t>& indices,
	const POLYGON_MODE mode)
{
	return PolygonRendererPtr(new MetalPolygonRenderer(vertices, indices, mode));
}

MetalPolygonRenderer::MetalPolygonRenderer(
	const std::vector<PolygonRenderer::Vertex>& vertices,
	const std::vector<uint32_t>& indices,
	const PolygonRenderer::POLYGON_MODE mode)
{
}

MetalPolygonRenderer::~MetalPolygonRenderer()
{
}

void MetalPolygonRenderer::BeginRendering(const ShaderPtr& shader)
{
}

void MetalPolygonRenderer::Render()
{
}

void MetalPolygonRenderer::EndRendering()
{
}

} // namespace gs2d
