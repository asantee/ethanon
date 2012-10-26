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

#ifndef GS2D_GLSPRITE_H_
#define GS2D_GLSPRITE_H_

#include "../../Sprite.h"

#include "GLVideo.h"
#include "GLTexture.h"

namespace gs2d {

class GLSprite : public Sprite
{
	GLVideoWeakPtr m_video;
	GLTexturePtr m_texture;
	math::Vector2 m_size;
	Sprite::TYPE m_type;

public:
	bool LoadSprite(
		VideoWeakPtr video,
		GS_BYTE* pBuffer,
		const unsigned int bufferLength,
		Color mask = constant::ZERO,
		const unsigned int width = 0,
		const unsigned int height = 0);

	bool LoadSprite(
		VideoWeakPtr video,
		const str_type::string& fileName,
		Color mask = constant::ZERO,
		const unsigned int width = 0,
		const unsigned int height = 0);

	bool CreateRenderTarget(
		VideoWeakPtr video,
		const unsigned int width,
		const unsigned int height,
		const Texture::TARGET_FORMAT format = Texture::TF_DEFAULT);

	bool Draw(
		const math::Vector2& v2Pos,
		const Color& color = constant::WHITE,
		const float angle = 0.0f,
		const math::Vector2& v2Scale = math::Vector2(1.0f,1.0f));

	bool DrawShaped(
		const math::Vector2 &v2Pos,
		const math::Vector2 &v2Size,
		const Color& color0,
		const Color& color1,
		const Color& color2,
		const Color& color3,
		const float angle = 0.0f);

	bool SaveBitmap(
		const wchar_t* wcsName,
		const Texture::BITMAP_FORMAT fmt,
		math::Rect2D* pRect = 0);

	bool DrawShapedFast(const math::Vector2 &v2Pos, const math::Vector2 &v2Size, const Color& color);

	bool DrawOptimal(
		const math::Vector2 &v2Pos,
		const Color& color = constant::WHITE,
		const float angle = 0.0f,
		const math::Vector2 &v2Size = math::Vector2(-1,-1));

	void BeginFastRendering();
	void EndFastRendering();

	TextureWeakPtr GetTexture();

	Texture::PROFILE GetProfile() const;
	math::Vector2i GetBitmapSize() const;
	math::Vector2 GetBitmapSizeF() const;

	TYPE GetType() const;
	boost::any GetTextureObject();

	void GenerateBackup();
	bool SetAsTexture(const unsigned int passIdx);

	/// Used on API's that must handle lost devices
	void OnLostDevice();

	/// Used on API's that must handle lost devices
	void RecoverFromBackup();

	void SetSpriteDensityValue(const float value);
	float GetSpriteDensityValue() const;
};

} // namespace gs2d

#endif
