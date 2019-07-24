#include "PolygonRenderer.h"

namespace gs2d {

PolygonRenderer::Vertex::Vertex(const math::Vector3& p, const math::Vector3& n, const math::Vector2& t) :
	pos(p),
	normal(n),
	texCoord(t)
{
}

} // namespace gs2d
