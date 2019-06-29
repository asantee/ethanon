#include "GameMath.h"

#include <math.h>

namespace gs2d {
namespace math {

float Util::RadianToDegree(const float angle)
{
	return (angle / (constant::PI * 2)) * 360.0f;
}

float Util::DegreeToRadian(const float angle)
{
	return angle * (constant::PI / 180.0f);
}

} // namespace math
} // namespace gs2d
