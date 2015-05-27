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

#include "ETHDrawable.h"
#include "../Resource/ETHResourceProvider.h"
#include <Platform/Platform.h>

ETHTextDrawer::ETHTextDrawer(
	const ETHResourceProviderPtr& provider,
	const Vector2& pos, 
	const str_type::string& text,
	const str_type::string &font,
	const GS_DWORD color,
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
	const GS_DWORD color,
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
		color.a = (GS_BYTE)(fade * 255.0f);
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

ETHRectangleDrawer::ETHRectangleDrawer(
	const ETHResourceProviderPtr& provider,
	const Vector2& pos,
	const Vector2& size,
	const Color& color)
{
	this->v2Pos = pos;
	this->v2Size = size;
	this->color0 = color;
	this->color1 = color;
	this->color2 = color;
	this->color3 = color;
	this->provider = provider;
}

ETHRectangleDrawer::ETHRectangleDrawer(
	const ETHResourceProviderPtr& provider,
	const Vector2& pos,
	const Vector2& size,
	const Color& color0,
	const Color& color1,
	const Color& color2,
	const Color& color3)
{
	this->v2Pos = pos;
	this->v2Size = size;
	this->color0 = color0;
	this->color1 = color1;
	this->color2 = color2;
	this->color3 = color3;
	this->provider = provider;
}

bool ETHRectangleDrawer::Draw(const unsigned long lastFrameElapsedTimeMS)
{
	GS2D_UNUSED_ARGUMENT(lastFrameElapsedTimeMS);
	return provider->GetVideo()->DrawRectangle(v2Pos, v2Size, color0, color1, color2, color3, 0.0f);
}

bool ETHRectangleDrawer::IsAlive() const
{
	return false;
}

ETHLineDrawer::ETHLineDrawer(
	const ETHResourceProviderPtr& provider,
	const Vector2& a,
	const Vector2& b,
	const Color& color0,
	const Color& color1,
	const float width)
{
	this->a = a;
	this->b = b;
	this->colorA = color0;
	this->colorB = color1;
	this->width = width;
	this->provider = provider;
}

bool ETHLineDrawer::Draw(const unsigned long lastFrameElapsedTimeMS)
{
	GS2D_UNUSED_ARGUMENT(lastFrameElapsedTimeMS);
	VideoPtr video = provider->GetVideo();
	video->SetLineWidth(width);
	return video->DrawLine(a, b, colorA, colorB);
}

bool ETHLineDrawer::IsAlive() const
{
	return false;
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
		provider->GetVideo()->SetVertexShader(ShaderPtr());
		provider->GetVideo()->SetPixelShader(ShaderPtr());
		const Vector2 size(v2Size == Vector2(-1,-1) ? sprite->GetFrameSize() : v2Size);
		sprite->SetOrigin(v2Origin);
		sprite->SetRect(frame);
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
