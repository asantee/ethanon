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
