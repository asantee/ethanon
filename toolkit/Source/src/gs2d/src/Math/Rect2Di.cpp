#include "Rect2Di.h"

namespace gs2d {
namespace math {

Rect2Di::Rect2Di() : pos(0, 0), size(0, 0)
{
}

Rect2Di::Rect2Di(const Vector2i& pos, const Vector2i& size)
{
	this->pos = pos;
	this->size = size;
}

Rect2Di::Rect2Di(const int posX, const int posY, const int sizeX, const int sizeY) :
	pos(posX, posY),
	size(sizeX, sizeY)
{
}

bool Rect2Di::operator == (const Rect2Di& r) const
{
	return (pos == r.pos && size == r.size);
}

bool Rect2Di::operator != (const Rect2Di& r) const
{
	return !(*this == r);
}

} // namespace math
} // namespace gs2d
