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
	const unsigned int frame,
	const bool flipX,
	const bool flipY)
{
	this->sprite = sprite;
	this->color0 = color;
	this->color1 = color;
	this->color2 = color;
	this->color3 = color;
	this->v2Pos = pos;
	this->v2Size = size;
	this->angle = angle;
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
		provider->GetVideo()->SetCurrentShader(ShaderPtr());

		sprite->SetRect(frame);

		const Vector2 frameSize(sprite->GetFrameSize());
		const Vector2 size(v2Size == Vector2(-1,-1) ? frameSize : v2Size);

		if (size.x == 0.0f || size.y == 0.0f)
			return true;

		const Rect2D rect(sprite->GetRect());

		const Vector2 bitmapSize = ((rect.size == rect.originalSize) ? frameSize : rect.size);
		const Vector2 virtualSize = ((rect.size == rect.originalSize) ? frameSize : rect.originalSize);
		const Vector2 absoluteOrigin = (virtualSize * v2Origin) - (rect.offset);
		const Vector2 relativeOrigin(absoluteOrigin.x / bitmapSize.x, absoluteOrigin.y / bitmapSize.y);

		sprite->SetOrigin(relativeOrigin);
		sprite->FlipX(flipX);
		sprite->FlipY(flipY);
		return sprite->DrawShaped(v2Pos, size, color0, color1, color2, color3, angle);
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
