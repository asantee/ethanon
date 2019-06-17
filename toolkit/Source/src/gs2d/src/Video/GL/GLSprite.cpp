#include "GLSprite.h"

#include "../../Platform/Platform.h"

namespace gs2d {

bool GLSprite::LoadSprite(
	VideoWeakPtr video,
	GS_BYTE* pBuffer,
	const unsigned int bufferLength,
	Color mask,
	const unsigned int width,
	const unsigned int height)
{
	m_video = boost::dynamic_pointer_cast<GLVideo>(video.lock());

	TexturePtr tex = m_video.lock()->CreateTextureFromFileInMemory(pBuffer, bufferLength, mask, width, height, 0);
	m_texture = boost::dynamic_pointer_cast<GLTexture>(tex);
	if (!m_texture)
		 return false;

	m_type = Sprite::T_BITMAP;
	Texture::PROFILE profile = m_texture->GetProfile();
	m_bitmapSize = math::Vector2(static_cast<float>(profile.width), static_cast<float>(profile.height));

	SetupSpriteRects(1, 1);
	return true;
}

bool GLSprite::LoadSprite(
	VideoWeakPtr video,
	const str_type::string& fileName,
	Color mask,
	const unsigned int width,
	const unsigned int height)
{
	m_video = boost::dynamic_pointer_cast<GLVideo>(video.lock());

	TexturePtr tex = m_video.lock()->LoadTextureFromFile(fileName, mask, width, height, 0);
	m_texture = boost::dynamic_pointer_cast<GLTexture>(tex);
	if (!m_texture)
		 return false;

	m_type = Sprite::T_BITMAP;
	Texture::PROFILE profile = m_texture->GetProfile();
	m_bitmapSize = math::Vector2(static_cast<float>(profile.width), static_cast<float>(profile.height));

	SetupSpriteRects(1, 1);
	return true;
}

bool GLSprite::CreateRenderTarget(
	VideoWeakPtr video,
	const unsigned int width,
	const unsigned int height,
	const Texture::TARGET_FORMAT format)
{
	m_video = boost::dynamic_pointer_cast<GLVideo>(video.lock());

	m_texture = boost::dynamic_pointer_cast<GLTexture>(m_video.lock()->CreateRenderTargetTexture(width, height, format));
	m_bitmapSize = math::Vector2(static_cast<float>(width), static_cast<float>(height));
	m_type = T_TARGET;
	SetupSpriteRects(1, 1);
	return true;
}

Texture::PROFILE GLSprite::GetProfile() const
{
	return m_texture->GetProfile();
}

TextureWeakPtr GLSprite::GetTexture()
{
	return m_texture;
}

Sprite::TYPE GLSprite::GetType() const
{
	return m_type;
}

math::Vector2i GLSprite::GetBitmapSize() const
{
	return GetBitmapSizeF().ToVector2i();
}

math::Vector2 GLSprite::GetBitmapSizeF() const
{
	return m_bitmapSize;
}

bool GLSprite::DrawShaped(
	const math::Vector2 &v2Pos,
	const math::Vector2 &v2Size,
	const math::Vector4& color0,
	const math::Vector4& color1,
	const math::Vector4& color2,
	const math::Vector4& color3,
	const float angle)
{
	if (v2Size == math::Vector2(0,0))
	{
		return true;
	}

	// compute flip parameters that will be sent to the VS
	math::Vector2 flipMul, flipAdd;
	GetFlipShaderParameters(flipAdd, flipMul);

	// centralizes the sprite according to the origin
	math::Vector2 v2Center = m_normalizedOrigin * v2Size;

	GLVideo* video = m_video.lock().get();
	ShaderPtr currentShader = video->GetCurrentShader();

	math::Matrix4x4 mRot;
	if (angle != 0.0f)
		mRot = math::RotateZ(math::DegreeToRadian(angle));
	currentShader->SetMatrixConstant("rotationMatrix", mRot);

	// rounds up the final position to avoid alpha distortion
	math::Vector2 v2FinalPos = v2Pos;

	currentShader->SetConstant("size", v2Size);
	currentShader->SetConstant("entityPos", v2FinalPos);
	currentShader->SetConstant("center", v2Center);
	currentShader->SetConstant("flipMul", flipMul);
	currentShader->SetConstant("flipAdd", flipAdd);
	currentShader->SetConstant("bitmapSize", GetBitmapSizeF());
	currentShader->SetConstant("scroll", GetScroll());
	currentShader->SetConstant("multiply", GetMultiply());

	const bool setCameraPos = currentShader->ConstantExist("cameraPos");
	if (setCameraPos)
		currentShader->SetConstant("cameraPos", video->GetCameraPos());

	if (m_rect.size.x == 0 || m_rect.size.y == 0)
	{
		currentShader->SetConstant("rectSize", GetBitmapSizeF());
		currentShader->SetConstant("rectPos", 0, 0);
	}
	else
	{
		currentShader->SetConstant("rectSize", m_rect.size);
		currentShader->SetConstant("rectPos", m_rect.pos);
	}

	currentShader->SetConstant("color0", color0);
	currentShader->SetConstant("color1", color1);
	currentShader->SetConstant("color2", color2);
	currentShader->SetConstant("color3", color3);

	if (currentShader->ConstantExist("depth"))
		currentShader->SetConstant("depth", video->GetSpriteDepth());

	currentShader->SetTexture("diffuse", m_texture);

	currentShader->SetShader();

	// draw the one-pixel-quad applying the vertex shader
	video->GetRectRenderer().Draw(m_rectMode);

	return true;
}

bool GLSprite::Draw(
	const math::Vector2& v2Pos,
	const math::Vector4& color,
	const float angle,
	const math::Vector2& v2Scale)
{
	const math::Vector2 v2Size(GetFrameSize() * v2Scale);
	return DrawShaped(v2Pos, v2Size, color, color, color, color, angle);
}

bool GLSprite::DrawOptimal(
	const math::Vector2 &v2Pos,
	const math::Vector4& color,
	const float angle,
	const math::Vector2 &v2Size)
{
	return DrawShaped(v2Pos, v2Size, color, color, color, color, angle);
}

void GLSprite::SetSpriteDensityValue(const float value)
{
	if (m_type == T_TARGET)
		return;

	Texture::PROFILE profile = m_texture->GetProfile();
	m_bitmapSize = math::Vector2(static_cast<float>(profile.width) / value, static_cast<float>(profile.height) / value);
	m_densityValue = value;
	SetupSpriteRects(1, 1);
}

float GLSprite::GetSpriteDensityValue() const
{
	return m_densityValue;
}

boost::any GLSprite::GetTextureObject()
{
	return m_texture;
}

void GLSprite::BeginFastRendering()
{
	GLVideo* video = m_video.lock().get();
	video->SetCurrentShader(video->GetFastShader());
	ShaderPtr current = video->GetCurrentShader();
	current->SetConstant("bitmapSize", GetBitmapSizeF());
	current->SetTexture("diffuse", m_texture);
	current->SetShader();
}

bool GLSprite::DrawShapedFast(const math::Vector2 &v2Pos, const math::Vector2 &v2Size, const math::Vector4& color)
{
	if (v2Size == math::Vector2(0,0))
	{
		return true;
	}

	GLVideo* video = m_video.lock().get();
	ShaderPtr current = video->GetCurrentShader();

	// rounds up the final position to avoid alpha distortion
	math::Vector2 v2FinalPos = v2Pos;

	current->SetConstant("size", v2Size);
	current->SetConstant("entityPos", v2FinalPos);
	current->SetConstant("color0", color);

	if (m_rect.size.x == 0 || m_rect.size.y == 0)
	{
		current->SetConstant("rectSize", GetBitmapSizeF());
		current->SetConstant("rectPos", 0, 0);
	}
	else
	{
		current->SetConstant("rectSize", m_rect.size);
		current->SetConstant("rectPos", m_rect.pos);
	}

	current->SetShader();

	video->GetRectRenderer().Draw(m_rectMode);
	return true;
}

void GLSprite::EndFastRendering()
{
	m_video.lock()->SetCurrentShader(ShaderPtr());
}

bool GLSprite::SaveBitmap(
	const str_type::char_t* name,
	const Texture::BITMAP_FORMAT fmt,
	math::Rect2D* pRect)
{
	return m_texture->SaveBitmap(name, fmt);
}

bool GLSprite::SetAsTexture(const unsigned int passIdx)
{
	GLVideoPtr video = m_video.lock();
	video->GetCurrentShader()->SetTexture("pass1", m_texture);
	return true;
}

void GLSprite::GenerateBackup()
{
	m_texture->SaveTargetSurfaceBackup();
}

void GLSprite::RecoverFromBackup()
{
	m_texture->Recover();
}

void GLSprite::OnLostDevice()
{
	// TODO
}

} // namespace gs2d
