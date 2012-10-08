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

#include "Sprite.h"
#include "Application.h"

namespace gs2d {
using namespace gs2d::math;


Sprite::Sprite() :
	m_nColumns(0),
	m_nRows(0),
	m_normalizedOrigin(Vector2(0.0f, 0.0f)),
	m_rect(Rect2Df(0,0,0,0)),
	m_rectMode(RM_TWO_TRIANGLES),
	m_currentRect(0)
{
}

void Sprite::SetRectMode(const RECT_MODE mode)
{
	m_rectMode = mode;
}

Sprite::RECT_MODE Sprite::GetRectMode() const
{
	return m_rectMode;
}

bool Sprite::Stretch(
	const Vector2& a,
	const Vector2& b,
	const float width,
	const Color& color0,
	const Color& color1)
{
	if (a == b || width <= 0.0f)
	{
		return true;
	}

	const Vector2 v2Dir(a - b);
	const float len = Distance(a, b);
	const float angle = RadianToDegree(GetAngle(v2Dir));
	const float lineWidth = (m_rect.size.x <= 0) ? (float)GetProfile().width : (float)m_rect.size.x;

	Vector2 origin = GetOrigin();
	SetOrigin(EO_CENTER_BOTTOM);

	const bool r =
		DrawShaped(
			a,
			Vector2((width >= 0.0f) ? width : lineWidth, len),
			color1,
			color1,
			color0,
			color0,
			angle);

	SetOrigin(origin);
	return r;
}

bool Sprite::SetupSpriteRects(const unsigned int columns, const unsigned int rows)
{
	m_rects.reset();

	if (columns <= 0 || rows <=0)
	{
		ShowMessage(GS_L("The number of rows or columns set can't be 0 or less - Sprite::SetupSpriteRects"), GSMT_ERROR);
		return false;
	}

	m_nColumns = columns;
	m_nRows = rows;
	const unsigned int nRects = columns * rows;
	m_rects = boost::shared_array<Rect2Df>(new Rect2Df [nRects]);

	const Vector2i size(GetBitmapSize());
	const unsigned int strideX = static_cast<unsigned int>(size.x) / columns, strideY = static_cast<unsigned int>(size.y) / rows;
	unsigned int index = 0;
	for (unsigned int y = 0; y < rows; y++)
	{
		for (unsigned int x = 0; x < columns; x++)
		{
			m_rects[index].pos.x = static_cast<float>(x * strideX);
			m_rects[index].pos.y = static_cast<float>(y * strideY);
			m_rects[index].size.x = static_cast<float>(strideX);
			m_rects[index].size.y = static_cast<float>(strideY);
			index++;
		}
	}

	SetRect(0);
	return true;
}

unsigned int Sprite::GetRectIndex() const
{
	return m_currentRect;
}

bool Sprite::SetRect(const unsigned int rect)
{
	m_currentRect = gs2d::math::Min(rect, GetNumRects() - 1);
	m_rect = m_rects[m_currentRect];
	return (m_currentRect == rect);
}

bool Sprite::SetRect(const unsigned int column, const unsigned int row)
{
	return SetRect((row * m_nColumns) + column);
}

void Sprite::SetRect(const Rect2Df& rect)
{
	m_rect = rect;
}

void Sprite::UnsetRect()
{
	m_rect = Rect2Df(0, 0, 0, 0);
	m_currentRect = 0;
}

Rect2Df Sprite::GetRect() const
{
	return m_rect;
}

unsigned int Sprite::GetNumRects() const
{
	return m_nColumns * m_nRows;
}

unsigned int Sprite::GetNumRows() const
{
	return m_nRows;
}

unsigned int Sprite::GetNumColumns() const
{
	return m_nColumns;
}

Rect2Df Sprite::GetRect(const unsigned int rect) const
{
	return m_rects[Min(GetNumRects() - 1, rect)];
}

Vector2 Sprite::GetOrigin() const
{
	return m_normalizedOrigin;
}

void Sprite::SetOrigin(const Vector2& origin)
{
	m_normalizedOrigin = origin;
}

void Sprite::SetOrigin(const ENTITY_ORIGIN origin)
{
	switch (origin)
	{
	case EO_RECT_CENTER:
	case EO_CENTER:
		m_normalizedOrigin.x = 1.0f / 2.0f;
		m_normalizedOrigin.y = 1.0f / 2.0f;
		break;
	case EO_RECT_CENTER_BOTTOM:
	case EO_CENTER_BOTTOM:
		m_normalizedOrigin.x = 1.0f / 2.0f;
		m_normalizedOrigin.y = 1.0f;
		break;
	case EO_RECT_CENTER_TOP:
	case EO_CENTER_TOP:
		m_normalizedOrigin.x = 1.0f / 2.0f;
		m_normalizedOrigin.y = 0.0f;
		break;
	default:
		m_normalizedOrigin.x = 0.0f;
		m_normalizedOrigin.y = 0.0f;
		break;
	};
}

math::Vector2 Sprite::GetFrameSize() const
{
	return (m_rect.size == Vector2(0, 0)) ? GetBitmapSizeF() : m_rect.size;
}

} // namespace gs2d
