#ifndef GS2D_SPRITE_RECTS_H_
#define GS2D_SPRITE_RECTS_H_

#include "Math/Rect2D.h"

#include <boost/shared_ptr.hpp>

#include <vector>

namespace gs2d {

class SpriteRects;

typedef boost::shared_ptr<SpriteRects> SpriteRectsPtr;

class SpriteRects
{
	std::vector<math::Rect2D> m_rects;

public:
	SpriteRects();
	SpriteRects(const std::vector<math::Rect2D>& rects);

	void SetRects(const unsigned int columns, const unsigned int rows);
	void SetRects(const std::vector<math::Rect2D>& rects);
	
	void AddRect(const math::Rect2D& rect);

	math::Rect2D GetRect(const unsigned int r) const;

	unsigned int GetNumRects() const;
};

} // namespace gs2d

#endif
