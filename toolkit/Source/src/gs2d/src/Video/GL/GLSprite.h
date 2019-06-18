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
	math::Vector2 m_bitmapSize;
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
		const math::Vector4& color,
		const float angle = 0.0f,
		const math::Vector2& v2Scale = math::Vector2(1.0f,1.0f));

	bool DrawShaped(
		const math::Vector2 &v2Pos,
		const math::Vector2 &v2Size,
		const math::Vector4& color0,
		const math::Vector4& color1,
		const math::Vector4& color2,
		const math::Vector4& color3,
		const float angle = 0.0f);

	bool SaveBitmap(
		const str_type::char_t* name,
		const Texture::BITMAP_FORMAT fmt,
		math::Rect2D* pRect = 0);

	bool DrawShapedFast(const math::Vector2 &v2Pos, const math::Vector2 &v2Size, const math::Vector4& color);

	bool DrawOptimal(
		const math::Vector2 &v2Pos,
		const math::Vector4& color,
		const float angle = 0.0f,
		const math::Vector2 &v2Size = math::constant::ONE_VECTOR2);

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
