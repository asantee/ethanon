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

unsigned long ARGB(const GS_BYTE a, const GS_BYTE r, const GS_BYTE g, const GS_BYTE b)
{
	return ((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff));
}

} // namespace gs2d
