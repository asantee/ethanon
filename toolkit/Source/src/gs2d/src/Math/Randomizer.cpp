/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2012 Andre Santee
 http://www.asantee.net/ethanon/

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the
	Software without restriction, including without limitation the rights to use, copy,
	modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
	and to permit persons to whom the Software is furnished to do so, subject to the
	following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
--------------------------------------------------------------------------------------*/

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
