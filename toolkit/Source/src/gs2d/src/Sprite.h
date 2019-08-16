#ifndef SPRITE_SPRITE_H_
#define SPRITE_SPRITE_H_

#include "PolygonRenderer.h"

#include "Platform/FileManager.h"

#include "SpriteRects.h"

namespace gs2d {

using namespace Platform;

class Sprite;

typedef boost::shared_ptr<Sprite> SpritePtr;

class Sprite
{
	static PolygonRendererPtr m_polygonRenderer;
	static math::Vector2 m_virtualScreenResolution;
	static float m_parallaxIntensity;

	static ShaderPtr m_defaultShader;
	static ShaderPtr m_fastShader;
	static ShaderPtr m_solidColorShader;
	static ShaderPtr m_highlightShader;
	static ShaderPtr m_addShader;
	static ShaderPtr m_modulateShader;
	static ShaderPtr m_solidColorAddShader;
	static ShaderPtr m_solidColorModulateShader;

	TexturePtr m_texture;

	float m_pixelDensity;
	math::Vector2 m_origin;

public:
	static void Initialize(Video* video);
	static void Finish();

	static const float PARALLAX_INTENSITY_FIX;

	static void SetVirtualScreenResolution(const math::Vector2& resolution);
	static math::Vector2 GetVirtualScreenResolution();
	static void SetVirtualScreenHeight(const math::Vector2& currentScreenResolution, const float height);
	static void SetParallaxIntensity(const float intensity);
	static float GetParallaxIntensity();

	static ShaderPtr GetDefaultShader();
	static ShaderPtr GetFastShader();
	static ShaderPtr GetSolidColorShader();
	static ShaderPtr GetHighlightShader();
	static ShaderPtr GetAddShader();
	static ShaderPtr GetModulateShader();
	static ShaderPtr GetSolidColorAddShader();
	static ShaderPtr GetSolidColorModulateShader();

	Sprite(
		Video* video,
		const std::string& fileName,
		const float pixelDensity = 1.0f);

	math::Vector2 GetSize(const math::Rect2D& rect) const;

	void Draw(
		const math::Vector3& pos,
		const math::Vector2& origin,
		const float scale,
		const float angle,
		const math::Rect2D& rect) const;

	void Draw(
		const math::Vector3& pos,
		const math::Vector2& size,
		const math::Vector2& origin,
		const Color& color,
		const float angle,
		const math::Rect2D& rect,
		const bool flipX,
		const bool flipY,
		ShaderPtr shader,
		ShaderParametersPtr shaderParameters = ShaderParametersPtr()) const;

	void Draw(
		const math::Vector2& cameraPos,
		const math::Vector3& pos,
		const math::Vector2& size,
		const math::Vector2& origin,
		const Color& color,
		const float angle,
		const math::Rect2D& rect,
		const bool flipX,
		const bool flipY,
		ShaderPtr shader,
		ShaderParametersPtr shaderParameters = nullptr) const;

	void Draw(
		const math::Vector3& pos,
		const float scale,
		const float angle,
		const math::Rect2D& rect) const;

	void Draw(
		const math::Vector3& pos,
		const math::Vector2& size,
		const Color& color,
		const float angle,
		const math::Rect2D& rect,
		const bool flipX,
		const bool flipY,
		ShaderPtr shader,
		ShaderParametersPtr shaderParameters = ShaderParametersPtr()) const;

	void Draw(
		const math::Vector2& cameraPos,
		const math::Vector3& pos,
		const math::Vector2& size,
		const Color& color,
		const float angle,
		const math::Rect2D& rect,
		const bool flipX,
		const bool flipY,
		ShaderPtr shader,
		ShaderParametersPtr shaderParameters = ShaderParametersPtr()) const;

	void BeginFastDraw() const;
	void FastDraw(
		const math::Vector3& pos,
		const math::Vector2& size,
		const math::Vector2& origin,
		const Color& color,
		const math::Rect2D& rect) const;
	void EndFastDraw() const;

	void SetPixelDensity(const float density);
	float GetPixelDensity() const;

	void SetOrigin(const math::Vector2& origin);
	math::Vector2 GetOrigin() const;

	TexturePtr GetTexture();
};

} // namespace sprite

#endif
