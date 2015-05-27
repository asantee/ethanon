/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2013 Andre Santee
 http://ethanonengine.com/

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

#ifndef ETH_DRAWABLE_H_
#define ETH_DRAWABLE_H_

#include "../ETHTypes.h"
#include "../Resource/ETHResourceManager.h"

class ETHDrawable
{
public:
	virtual bool Draw(const unsigned long lastFrameElapsedTimeMS) = 0;
	virtual bool IsAlive() const = 0;
};

class ETHTextDrawer : public ETHDrawable
{
public:
	ETHTextDrawer(
		const ETHResourceProviderPtr& provider,
		const Vector2& pos,
		const str_type::string& text,
		const str_type::string& font,
		const GS_DWORD color,
		const unsigned long time,
		const float scale);

	ETHTextDrawer(
		const ETHResourceProviderPtr& provider,
		const Vector2& pos,
		const str_type::string& text,
		const str_type::string& font,
		const GS_DWORD color,
		const float scale);

	bool Draw(const unsigned long lastFrameElapsedTimeMS);
	bool IsAlive() const;

private:
	Vector2 v2Pos;
	str_type::string text;
	str_type::string font;
	GS_DWORD color;
	unsigned long timeMS;
	unsigned long elapsedTimeMS;
	float scale;
	ETHResourceProviderPtr provider;
};

class ETHRectangleDrawer : public ETHDrawable
{
public:
	ETHRectangleDrawer(
		const ETHResourceProviderPtr& provider,
		const Vector2& pos,
		const Vector2& size,
		const Color& color);

	ETHRectangleDrawer(
		const ETHResourceProviderPtr& provider,
		const Vector2& pos,
		const Vector2& size,
		const Color& color0,
		const Color& color1,
		const Color& color2,
		const Color& color3);

	bool Draw(const unsigned long lastFrameElapsedTimeMS);
	bool IsAlive() const;

private:
	Vector2 v2Pos;
	Vector2 v2Size;
	Color color0, color1, color2, color3;
	ETHResourceProviderPtr provider;
};


class ETHLineDrawer : public ETHDrawable
{
public:
	ETHLineDrawer(
		const ETHResourceProviderPtr& provider,
		const Vector2& a,
		const Vector2& b, 
		const Color& color0,
		const Color& color1,
		const float width);

	bool Draw(const unsigned long lastFrameElapsedTimeMS);
	bool IsAlive() const;

private:
	Vector2 a, b;
	Color colorA, colorB;
	float width;
	ETHResourceProviderPtr provider;
};

class ETHSpriteDrawer : public ETHDrawable
{
public:
	ETHSpriteDrawer(
		const ETHResourceProviderPtr& provider,
		const SpritePtr& sprite,
		const Vector2& pos,
		const Vector2& size,
		const Vector4& color,
		const float angle,
		const unsigned int frame,
		const bool flipX,
		const bool flipY);

	bool Draw(const unsigned long lastFrameElapsedTimeMS);
	bool IsAlive() const;

private:
	ETHResourceProviderPtr provider;
	Vector2 v2Pos;
	Vector2 v2Size;
	Vector2 v2Origin;
	SpritePtr sprite;
	Vector4 color0, color1, color2, color3;
	float angle;
	unsigned int frame;
	bool flipX, flipY;
};

#endif
