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
	
	static std::vector<math::Vector2> m_attachedParameters;
	
public:
	GLES2Sprite(GLES2ShaderContextPtr shaderContext);

	bool LoadSprite(
		VideoWeakPtr video,
		unsigned char *pBuffer,
		const unsigned int bufferLength,
		const unsigned int width = 0,
		const unsigned int height = 0);

	bool LoadSprite(
		VideoWeakPtr video,
		const str_type::string& fileName,
		const unsigned int width = 0,
		const unsigned int height = 0);

	bool Draw(
		const math::Vector2 &v2Pos,
		const math::Vector4& color,
		const float angle = 0.0f,
		const math::Vector2 &v2Scale = math::Vector2(1.0f,1.0f));

	bool DrawShaped(
		const math::Vector2 &v2Pos,
		const math::Vector2 &v2Size,
		const math::Vector4& color0,
		const math::Vector4& color1,
		const math::Vector4& color2,
		const math::Vector4& color3,
		const float angle = 0.0f);

	bool DrawOptimal(
		const math::Vector2& v2Pos,
		const math::Vector4& color,
		const float angle = 0.0f,
		const math::Vector2 &v2Size = math::Vector2(-1,-1));

	static void AttachParametersToOptimalRenderer(const std::vector<math::Vector2>& params);

	bool DrawShapedFast(const math::Vector2 &v2Pos, const math::Vector2 &v2Size, const math::Vector4& color);
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
