#include "Randomizer.h"
#include "GameMath.h"

namespace gs2d {
namespace math {

MTRand Randomizer::m_mt;

int Randomizer::Int(const int maxValue)
{
	return static_cast<int>(m_mt.randInt(static_cast<MTRand::uint32>(maxValue)));
}

int Randomizer::Int(const int minValue, const int maxValue)
{
	const int min = Min(minValue, maxValue);
	const int max = Max(minValue, maxValue);
	const int diff = Abs(max - min);
	return Int(diff) + min;
}

float Randomizer::Float(const float maxValue)
{
	return static_cast<float>(m_mt.rand(static_cast<double>(maxValue)));
}

float Randomizer::Float(const float minValue, const float maxValue)
{
	const float min = Min(minValue, maxValue);
	const float max = Max(minValue, maxValue);
	const float diff = Abs(max - min);
	return Float(diff) + min;
}

void Randomizer::Seed(const unsigned int seed)
{
	m_mt.seed(static_cast<MTRand::uint32>(seed));
}

} // namespace math
} // namespace gs2d
