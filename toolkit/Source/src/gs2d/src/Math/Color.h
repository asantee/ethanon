#ifndef GS2D_COLOR_H_
#define GS2D_COLOR_H_

#include "Vector4.h"

#include <cstdint>

namespace gs2d {

struct Color : public math::Vector4
{
	static uint32_t ARGB(
		const unsigned char a,
		const unsigned char r,
		const unsigned char g,
		const unsigned char b);

	Color();
	Color(const math::Vector4& v);
	Color(const Color& v);
	Color(const float fr, const float fg, const float fb, const float fa);
	Color(const uint32_t color);

	uint32_t To32BitARGB() const;
	
	float GetA() const;
	float GetR() const;
	float GetG() const;
	float GetB() const;
};

namespace constant {
	const Color BLACK(0.0f, 0.0f, 0.0f, 1.0f);
	const Color WHITE(1.0f, 1.0f, 1.0f, 1.0f);
} // namespace constant

} // namespace gs2d

#endif
