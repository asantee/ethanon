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

#include "GLSprite.h"

namespace gs2d {

bool GLSprite::LoadSprite(
	VideoWeakPtr video,
	GS_BYTE* pBuffer,
	const unsigned int bufferLength,
	Color mask,
	const unsigned int width,
	const unsigned int height)
{
	GLVideoPtr glVideo = boost::dynamic_pointer_cast<GLVideo>(video.lock());
	m_video = glVideo;

	TexturePtr tex = m_video.lock()->CreateTextureFromFileInMemory(pBuffer, bufferLength, mask, width, height, 0);
	m_texture = boost::dynamic_pointer_cast<GLTexture>(tex);
	if (!m_texture)
		 return false;

	m_type = Sprite::T_BITMAP;
	Texture::PROFILE profile = m_texture->GetProfile();
	m_size = math::Vector2(static_cast<float>(profile.width), static_cast<float>(profile.height));

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
	VideoPtr v = video.lock();

	Platform::FileBuffer out;
	v->GetFileIOHub()->GetFileManager()->GetFileBuffer(fileName, out);
	if (!out)
	{
		ShowMessage(fileName + " could not load buffer", GSMT_ERROR);
		return false;
	}
	return LoadSprite(video, out->GetAddress(), static_cast<unsigned int>(out->GetBufferSize()), mask, width, height);
}

bool GLSprite::CreateRenderTarget(
	VideoWeakPtr video,
	const unsigned int width,
	const unsigned int height,
	const Texture::TARGET_FORMAT format)
{
	// TODO
	return false;
}

bool GLSprite::Draw(
	const math::Vector2& v2Pos,
	const Color& color,
	const float angle,
	const math::Vector2& v2Scale)
{
	// TODO
	return false;
}

bool GLSprite::DrawShaped(
	const math::Vector2 &v2Pos,
	const math::Vector2 &v2Size,
	const Color& color0,
	const Color& color1,
	const Color& color2,
	const Color& color3,
	const float angle)
{
	// TODO
	return false;
}

bool GLSprite::SaveBitmap(
	const wchar_t* wcsName,
	const Texture::BITMAP_FORMAT fmt,
	math::Rect2D* pRect)
{
	// TODO
	return false;
}

bool GLSprite::DrawShapedFast(const math::Vector2 &v2Pos, const math::Vector2 &v2Size, const Color& color)
{
	// TODO
	return false;
}

bool GLSprite::DrawOptimal(
	const math::Vector2 &v2Pos,
	const Color& color,
	const float angle,
	const math::Vector2 &v2Size)
{
	// TODO
	return false;
}

void GLSprite::BeginFastRendering()
{
	// TODO
}

void GLSprite::EndFastRendering()
{
	// TODO
}

TextureWeakPtr GLSprite::GetTexture()
{
	// TODO
	return TextureWeakPtr();
}

Texture::PROFILE GLSprite::GetProfile() const
{
	// TODO
	return Texture::PROFILE();
}

math::Vector2i GLSprite::GetBitmapSize() const
{
	// TODO
	return math::Vector2i();
}

math::Vector2 GLSprite::GetBitmapSizeF() const
{
	// TODO
	return math::Vector2();
}

Sprite::TYPE GLSprite::GetType() const
{
	// TODO
	return Sprite::T_NOT_LOADED;
}

boost::any GLSprite::GetTextureObject()
{
	// TODO
	return false;
}

void GLSprite::GenerateBackup()
{
	// TODO
}

bool GLSprite::SetAsTexture(const unsigned int passIdx)
{
	// TODO
	return false;
}

void GLSprite::OnLostDevice()
{
	// TODO
}

void GLSprite::RecoverFromBackup()
{
	// TODO
}

void GLSprite::SetSpriteDensityValue(const float value)
{
	// TODO
}

float GLSprite::GetSpriteDensityValue() const
{
	// TODO
	return false;
}

} // namespace gs2d
