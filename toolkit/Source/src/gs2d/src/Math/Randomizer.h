#ifndef GS2D_RANDOMIZER_H_
#define GS2D_RANDOMIZER_H_

#include "MersenneTwister.h"

namespace gs2d {
namespace math {

class Randomizer
{
	static MTRand m_mt;
public:

	static int Int(const int maxValue);
	static float Float(const float maxValue);
	static int Int(const int minValue, const int maxValue);
	static float Float(const float minValue, const float maxValue);
	static void Seed(const unsigned int seed);
};

} // namespace math
} // namespace gs2d

#endif
