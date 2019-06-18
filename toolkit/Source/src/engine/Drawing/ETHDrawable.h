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
		const Color color,
		const unsigned long time,
		const float scale);

	ETHTextDrawer(
		const ETHResourceProviderPtr& provider,
		const Vector2& pos,
		const str_type::string& text,
		const str_type::string& font,
		const Color color,
		const float scale);

	bool Draw(const unsigned long lastFrameElapsedTimeMS);
	bool IsAlive() const;

private:
	Vector2 v2Pos;
	str_type::string text;
	str_type::string font;
	Color color;
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
