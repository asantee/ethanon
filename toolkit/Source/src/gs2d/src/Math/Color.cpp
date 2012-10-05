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

#include "Color.h"


namespace gs2d {

Color::Color() : color(0)
{
}

Color::Color(const GS_DWORD color)
{
	this->color = color;
}

Color::Color(const GS_BYTE na, const GS_BYTE nr, const GS_BYTE ng, const GS_BYTE nb)
{
	a = na;
	r = nr;
	g = ng;
	b = nb;
}

void Color::SetColor(const GS_BYTE na, const GS_BYTE nr, const GS_BYTE ng, const GS_BYTE nb)
{
	a = na;
	r = nr;
	g = ng;
	b = nb;
}

void Color::SetColor(const GS_DWORD color)
{
	this->color = color;
}

Color::operator GS_DWORD() const
{
	return color;
}

Color& Color::operator = (GS_DWORD color)
{
	this->color = color;
	return *this;
}

void Color::SetAlpha(const GS_BYTE na)
{
	a = na;
}

void Color::SetRed(const GS_BYTE nr)
{
	r = nr;
}

void Color::SetGreen(const GS_BYTE ng)
{
	g = ng;
}

void Color::SetBlue(const GS_BYTE nb)
{
	b = nb;
}

} // namespace gs2d
