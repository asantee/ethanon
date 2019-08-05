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
		SpriteRectsPtr rects,
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
	SpriteRectsPtr rects;
	Vector4 color;
	float angle;
	unsigned int frame;
	bool flipX, flipY;
};

#endif
