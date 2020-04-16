#ifndef GS2D_ORIENTED_BOUNDING_BOX_H_
#define GS2D_ORIENTED_BOUNDING_BOX_H_

#include "Vector2.h"

#include <boost/shared_ptr.hpp>

namespace gs2d {
namespace math {

/**
 * \brief Describes an oriented bounding box
 *
 * This class stores and compares two boxes for oriented BB overlapping.
 * The code was originally written by Morgan McGuire and posted in this
 * thread: http://www.flipcode.com/archives/2D_OBB_Intersection.shtml
 */
class OrientedBoundingBox
{
public:
	OrientedBoundingBox(const Vector2& center, const Vector2& size, const float angle);
	bool Overlaps(const OrientedBoundingBox& other) const;

private:
	Vector2 corner[4];
	Vector2 axis[2];
	float origin[2];

	bool Overlaps1Way(const OrientedBoundingBox& other) const;
	void ComputeAxes();
};

typedef boost::shared_ptr<gs2d::math::OrientedBoundingBox> OrientedBoundingBoxPtr;

} // math
} // gs2d

#endif
