#ifndef GS2D_COLOR_H_
#define GS2D_COLOR_H_

#include "../Types.h"

namespace gs2d {

#pragma warning(push)
#pragma warning(disable:4201)

struct Color
{
	Color();
	Color(const GS_DWORD color);
	Color(const GS_BYTE na, const GS_BYTE nr, const GS_BYTE ng, const GS_BYTE nb);
	void SetColor(const GS_BYTE na, const GS_BYTE nr, const GS_BYTE ng, const GS_BYTE nb);
	void SetColor(const GS_DWORD color);
	Color &operator = (GS_DWORD color);
	operator GS_DWORD () const;
	void SetAlpha(const GS_BYTE na);
	void SetRed(const GS_BYTE nr);
	void SetGreen(const GS_BYTE ng);
	void SetBlue(const GS_BYTE nb);
	union
	{
		struct
		{
			GS_BYTE b, g, r, a;
		};
		GS_DWORD color;
	};
};

#pragma warning( pop )

unsigned long ARGB(const GS_BYTE a, const GS_BYTE r, const GS_BYTE g, const GS_BYTE b);

namespace constant {
	const Color ZERO(0x0);
	const Color BLACK(0xFF000000);
	const Color WHITE(0xFFFFFFFF);
	const Color RED(0xFFFF0000);
	const Color GREEN(0xFF00FF00);
	const Color BLUE(0xFF0000FF);
	const Color YELLOW(0xFFFFFF00);
} // constant

} // gs2d

#endif
