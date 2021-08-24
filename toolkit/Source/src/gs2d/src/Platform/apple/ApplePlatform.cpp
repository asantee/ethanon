#include "ApplePlatform.h"

#include "../../Application.h"
#include "../FileLogger.h"
#include "../../PolygonRenderer.h"

#include <iostream>
#include <sstream>

namespace gs2d {

PolygonRendererPtr PolygonRenderer::Create(
	const std::vector<PolygonRenderer::Vertex>& vertices,
	const std::vector<uint32_t>& indices,
	const PolygonRenderer::POLYGON_MODE mode)
{
	//return boost::shared_ptr<PolygonRenderer>(new GLPolygonRenderer(vertices, indices, mode));
	return PolygonRendererPtr();
}

} // namespace gs2d

namespace Platform {

} // namespace Platform
