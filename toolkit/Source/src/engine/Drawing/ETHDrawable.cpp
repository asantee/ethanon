#include "ETHDrawable.h"

#include "../Resource/ETHResourceProvider.h"

#include <Platform/Platform.h>

ETHTextDrawer::ETHTextDrawer(
	const ETHResourceProviderPtr& provider,
	const Vector2& pos, 
	const str_type::string& text,
	const str_type::string &font,
	const Color color,
	const unsigned long time,
	const float scale)
{
	this->v2Pos = pos;
	this->text = text;
	this->font = font;
	this->color = color;
	this->timeMS = time;
	this->elapsedTimeMS = 0;
	this->provider = provider;
	this->scale = scale;
}

ETHTextDrawer::ETHTextDrawer(
	const ETHResourceProviderPtr& provider,
	const Vector2& pos, 
	const str_type::string& text,
	const str_type::string& font,
	const Color color,
	const float scale)
{
	this->v2Pos = pos;
	this->text = text;
	this->font = font;
	this->color = color;
	this->timeMS = 0;
	this->elapsedTimeMS = 0;
	this->provider = provider;
	this->scale = scale;
}

bool ETHTextDrawer::Draw(const unsigned long lastFrameElapsedTimeMS)
{
	elapsedTimeMS += lastFrameElapsedTimeMS;
	Color color = this->color;

	if (timeMS > 0)
	{
		const float fade = 1.0f - Clamp((float)elapsedTimeMS / (float)this->timeMS, 0.0f, 1.0f);
		color.w = fade;
	}
	return provider->GetVideo()->DrawBitmapText(v2Pos, text, font, color, scale);
}

bool ETHTextDrawer::IsAlive() const
{
	if (elapsedTimeMS >= timeMS)
		return false;
	else
		return true;
}

ETHSpriteDrawer::ETHSpriteDrawer(
	const ETHResourceProviderPtr& provider,
	const SpritePtr& sprite,
	const Vector2& pos,
	const Vector2& size,
	const Vector4& color,
	const float angle,
	SpriteRectsPtr rects,
	const unsigned int frame,
	const bool flipX,
	const bool flipY)
{
	this->sprite = sprite;
	this->color = color;
	this->v2Pos = pos;
	this->v2Size = size;
	this->angle = angle;
	this->rects = rects;
	this->frame = frame;
	this->provider = provider;
	this->flipX = flipX;
	this->flipY = flipY;

	if (this->sprite)
		this->v2Origin = sprite->GetOrigin();
}

bool ETHSpriteDrawer::Draw(const unsigned long lastFrameElapsedTimeMS)
{
	GS2D_UNUSED_ARGUMENT(lastFrameElapsedTimeMS);
	if (sprite)
	{
		const Rect2D rect(rects ? rects->GetRect(frame) : Rect2D());
		const Vector2 frameSize(sprite->GetSize(rect));
		const Vector2 size(v2Size == Vector2(-1,-1) ? frameSize : v2Size);

		if (size.x == 0.0f || size.y == 0.0f)
			return true;

		const Vector2 virtualSize = ((rect.originalSize == Vector2(0.0f)) ? frameSize : rect.originalSize);
		const Vector2 absoluteOrigin = (virtualSize * v2Origin) - (rect.offset);
		const Vector2 relativeOrigin(absoluteOrigin.x / virtualSize.x, absoluteOrigin.y / virtualSize.y);

		sprite->Draw(Vector3(v2Pos, 0.0f), size, relativeOrigin, color, angle, rect, flipX, flipY, Sprite::GetDefaultShader());
		return true;
	}
	else
	{
		return false;
	}
}

bool ETHSpriteDrawer::IsAlive() const
{
	return false;
}
