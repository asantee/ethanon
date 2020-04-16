#ifndef GS2D_RECT2D_H_
#define GS2D_RECT2D_H_

#include "Vector2.h"

namespace gs2d {
namespace math {

struct Rect2D
{
	Rect2D();
	Rect2D(const Vector2& p, const Vector2& s);
	Rect2D(const Vector2& p, const Vector2& s, const Vector2& o, const Vector2& os);
	Rect2D(const float posX, const float posY, const float sizeX, const float sizeY);
	bool operator == (const Rect2D& r) const;
	bool operator != (const Rect2D& r) const;

	Vector2 pos, size, offset, originalSize;
};

} // namespace math
} // namespace gs2d

#endif
