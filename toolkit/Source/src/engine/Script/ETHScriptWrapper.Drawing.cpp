#include "ETHScriptWrapper.h"
#include "../Drawing/ETHParticleDrawer.h"

Vector2 ETHScriptWrapper::ComputeCarretPosition(const str_type::string &font, const str_type::string &text, const unsigned int pos)
{
	return m_provider->GetVideo()->ComputeCarretPosition(font, text, pos);
}

Vector2 ETHScriptWrapper::ComputeTextBoxSize(const str_type::string &font, const str_type::string &text)
{
	return m_provider->GetVideo()->ComputeTextBoxSize(font, text);
}

void ETHScriptWrapper::DrawText(const Vector2 &v2Pos, const str_type::string &text, const str_type::string &font, const uint32_t color, const float scale)
{
	m_drawableManager.Insert(boost::shared_ptr<ETHDrawable>(new ETHTextDrawer(m_provider, v2Pos, text, font, color, 0x0, scale)));
}

str_type::string ETHScriptWrapper::AssembleColorCode(const uint32_t color)
{
	return gs2d::BitmapFont::AssembleColorCode(color);
}

void ETHScriptWrapper::LoadSprite(const str_type::string& name)
{
	if (WarnIfRunsInMainFunction(GS_L("LoadSprite")))
		return;
	LoadAndGetSprite(name);
}

bool ETHScriptWrapper::ReleaseSprite(const str_type::string& name)
{
	if (WarnIfRunsInMainFunction(GS_L("ReleaseSprite")))
		return false;
	return m_provider->GetGraphicResourceManager()->ReleaseResource(name);
}

SpritePtr ETHScriptWrapper::LoadAndGetSprite(const str_type::string &name)
{
	str_type::string resourceDirectory = m_provider->GetFileIOHub()->GetResourceDirectory();
	str_type::string path = resourceDirectory + name;
	return m_provider->GetGraphicResourceManager()->AddFile(m_provider->GetFileManager(), m_provider->GetVideo(), path, resourceDirectory, false, false);
}

Vector2 ETHScriptWrapper::GetSpriteSize(const str_type::string &name)
{
	SpritePtr pSprite = LoadAndGetSprite(name);
	if (pSprite)
	{
		return pSprite->GetBitmapSizeF();
	}
	else
	{
		return Vector2(0,0);
	}
}

Vector2 ETHScriptWrapper::GetSpriteFrameSize(const str_type::string& name)
{
	SpritePtr pSprite = LoadAndGetSprite(name);
	if (pSprite)
	{
		return pSprite->GetFrameSize();
	}
	else
	{
		return Vector2(0,0);
	}
}

void ETHScriptWrapper::SetupSpriteRects(const str_type::string& name, const unsigned int columns, const unsigned int rows)
{
	SpritePtr pSprite = LoadAndGetSprite(name);
	if (pSprite)
	{
		Sprite::RectsPtr packedFrames = m_provider->GetGraphicResourceManager()->GetPackedFrames(Platform::GetFileName(name));
		if (packedFrames)
			pSprite->SetRects(packedFrames);
		else
			pSprite->SetupSpriteRects(columns, rows);
	}
}

void ETHScriptWrapper::SetSpriteRect(const str_type::string& name, const unsigned int frame)
{
	SpritePtr pSprite = LoadAndGetSprite(name);
	if (pSprite)
	{
		pSprite->SetRect(frame);
	}
}

void ETHScriptWrapper::SetSpriteOrigin(const str_type::string& name, const Vector2& origin)
{
	SpritePtr pSprite = LoadAndGetSprite(name);
	if (pSprite)
	{
		pSprite->SetOrigin(origin);
	}
}

void ETHScriptWrapper::SetSpriteFlipX(const str_type::string &name, const bool flip)
{
	SpritePtr pSprite = LoadAndGetSprite(name);
	if (pSprite)
	{
		pSprite->FlipX(flip);
	}
}

void ETHScriptWrapper::SetSpriteFlipY(const str_type::string &name, const bool flip)
{
	SpritePtr pSprite = LoadAndGetSprite(name);
	if (pSprite)
	{
		pSprite->FlipY(flip);
	}
}

void ETHScriptWrapper::DrawShapedFromPtr(
	const SpritePtr& sprite,
	const Vector2 &v2Pos,
	const Vector2 &v2Size,
	const uint32_t color,
	const float angle)
{
	DrawShapedFromPtr(sprite, v2Pos, v2Size, Vector4(Color(color)), angle);
}

void ETHScriptWrapper::DrawShapedFromPtr(
	const SpritePtr& sprite,
	const Vector2 &v2Pos,
	const Vector2 &v2Size,
	const Vector4 &color,
	const float angle)
{
	m_drawableManager.Insert(boost::shared_ptr<ETHDrawable>(
		new ETHSpriteDrawer(
			m_provider,
			sprite,
			v2Pos,
			v2Size,
			color,
			angle,
			sprite->GetRectIndex(),
			sprite->GetFlipX(),
			sprite->GetFlipY())));
}

void ETHScriptWrapper::DrawSprite(const str_type::string &name, const Vector2 &v2Pos, const uint32_t color, const float angle)
{
	SpritePtr sprite = LoadAndGetSprite(name);
	DrawShapedFromPtr(sprite, v2Pos, Vector2(-1,-1), color, angle);
}

void ETHScriptWrapper::DrawShaped(const str_type::string &name, const Vector2 &v2Pos, const Vector2 &v2Size, const uint32_t color, const float angle)
{
	SpritePtr sprite = LoadAndGetSprite(name);
	DrawShapedFromPtr(sprite, v2Pos, v2Size, color, angle);
}

void ETHScriptWrapper::DrawSprite(const str_type::string &name, const Vector2 &v2Pos, const float alpha, const Vector3 &color, const float angle)
{
	SpritePtr sprite = LoadAndGetSprite(name);
	DrawShapedFromPtr(sprite, v2Pos, Vector2(-1,-1), Vector4(color, alpha), angle);
}

void ETHScriptWrapper::DrawShaped(const str_type::string &name, const Vector2 &v2Pos, const Vector2 &v2Size, const float alpha, const Vector3 &color, const float angle)
{
	SpritePtr sprite = LoadAndGetSprite(name);
	DrawShapedFromPtr(sprite, v2Pos, v2Size, Vector4(color, alpha), angle);
}

void ETHScriptWrapper::PlayParticleEffect(const str_type::string& fileName, const Vector2& pos, const float angle, const float scale)
{
	m_drawableManager.Insert(
		boost::shared_ptr<ETHDrawable>(
			new ETHParticleDrawer(
				m_provider,
				m_provider->GetGraphicResourceManager(),
				m_provider->GetShaderManager(),
				m_provider->GetFileIOHub()->GetResourceDirectory(),
				fileName,
				pos,
				angle,
				scale)));
}

void ETHScriptWrapper::DrawFadingText(const Vector2 &v2Pos, const str_type::string &text, const str_type::string &font, const uint32_t color, unsigned long time, const float scale)
{
	m_drawableManager.Insert(boost::shared_ptr<ETHDrawable>(
		new ETHTextDrawer(m_provider, v2Pos, text, font, color, time, scale)));
}
