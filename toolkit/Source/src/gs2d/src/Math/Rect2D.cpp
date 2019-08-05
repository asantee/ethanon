#include "Rect2D.h"

namespace gs2d {
namespace math {

Rect2D::Rect2D() :
	pos(0.0f),
	size(1.0f),
	originalSize(1.0f),
	offset(0.0f)
{
}

Rect2D::Rect2D(const Vector2& p, const Vector2& s) :
	pos(p),
	size(s),
	originalSize(s),
	offset(0.0f)
{
}

Rect2D::Rect2D(const Vector2& p, const Vector2& s, const Vector2& o, const Vector2& os) :
	pos(p),
	size(s),
	originalSize(os),
	offset(o)
{
}

Rect2D::Rect2D(const float posX, const float posY, const float sizeX, const float sizeY) :
	pos(posX, posY),
	size(sizeX, sizeY),
	originalSize(sizeX, sizeY),
	offset(0.0f)
{
}

bool Rect2D::operator == (const Rect2D& r) const
{
	return (pos == r.pos && size == r.size);
}

bool Rect2D::operator != (const Rect2D& r) const
{
	return !(*this == r);
}

} // namespace math
} // namespace gs2d
