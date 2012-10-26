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

#ifndef GS2D_GLES2_SPRITE_H_
#define GS2D_GLES2_SPRITE_H_

#include "../../Video.h"
#include "GLES2Shader.h"
#include "GLES2Texture.h"
#include <boost/shared_array.hpp>

namespace gs2d {

class GLES2Sprite : public Sprite
{
private:
	GLES2ShaderContext* m_shaderContext;

	static const str_type::string SPRITE_LOG_FILE;
	static Platform::FileLogger m_logger;
	
	TYPE m_type;
	math::Vector2 m_bitmapSize;
	GLES2TexturePtr m_texture;
	Video* m_video;
	float m_densityValue;
	
public:
	GLES2Sprite(GLES2ShaderContextPtr shaderContext);

	bool LoadSprite(
		VideoWeakPtr video,
		GS_BYTE *pBuffer,
		const unsigned int bufferLength,
		Color mask = gs2d::constant::ZERO,
		const unsigned int width = 0,
		const unsigned int height = 0);

	bool LoadSprite(
		VideoWeakPtr video,
		const str_type::string& fileName,
		Color mask = gs2d::constant::ZERO,
		const unsigned int width = 0,
		const unsigned int height = 0);

	bool CreateRenderTarget(
		VideoWeakPtr video,
		const unsigned int width,
		const unsigned int height,
		const Texture::TARGET_FORMAT format = Texture::TF_DEFAULT);

	bool Draw(
		const math::Vector2 &v2Pos,
		const Color& color = gs2d::constant::WHITE,
		const float angle = 0.0f,
		const math::Vector2 &v2Scale = math::Vector2(1.0f,1.0f));

	bool DrawShaped(
		const math::Vector2 &v2Pos,
		const math::Vector2 &v2Size,
		const Color& color0,
		const Color& color1,
		const Color& color2,
		const Color& color3,
		const float angle = 0.0f);

	bool DrawOptimal(
		const math::Vector2 &v2Pos,
		const Color& color = gs2d::constant::WHITE,
		const float angle = 0.0f,
		const math::Vector2 &v2Size = math::Vector2(-1,-1));

	bool SaveBitmap(const wchar_t *wcsName, const Texture::BITMAP_FORMAT fmt, math::Rect2D *pRect = 0);

	bool DrawShapedFast(const math::Vector2 &v2Pos, const math::Vector2 &v2Size, const Color& color);
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

	void OnLostDevice();

	void RecoverFromBackup();

	void SetSpriteDensityValue(const float value);
	float GetSpriteDensityValue() const;
};

} // namespace gs2d

#endif
