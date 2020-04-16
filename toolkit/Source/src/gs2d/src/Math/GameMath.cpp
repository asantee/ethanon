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

bool Util::IsPowerOfTwo(const unsigned int n)
{
    return (n != 0) && ((n & (n - 1)) == 0);
}

unsigned int Util::FindNextPowerOfTwoValue(const unsigned int n)
{
    unsigned int x = 1;
    unsigned int r;
    while ((r = (unsigned int)pow(2, (double)x++)) < n);
    return r;
}

} // namespace math
} // namespace gs2d
