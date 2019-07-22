#include "PolygonRenderer.h"

namespace gs2d {

PolygonRenderer::Vertex::Vertex(const math::Vector3& p, const math::Vector3& v, const math::Vector2& t) :
	pos(p),
	v1(v),
	texCoord(t)
{
}

} // namespace gs2d
