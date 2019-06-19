#ifndef GS2D_RECT2Di_H_
#define GS2D_RECT2Di_H_

#include "Vector2i.h"

namespace gs2d {
namespace math {

struct Rect2Di
{
	Rect2Di();
	Rect2Di(const Vector2i& pos, const Vector2i& size);
	Rect2Di(const int posX, const int posY, const int sizeX, const int sizeY);
	bool operator == (const Rect2Di& r) const;
	bool operator != (const Rect2Di& r) const;

	Vector2i pos, size;
};

} // namespace math
} // namespace gs2d

#endif
