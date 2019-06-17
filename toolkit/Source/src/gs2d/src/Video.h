#ifndef GS2D_VIDEO_H_
#define GS2D_VIDEO_H_

#include <list>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

#include "Application.h"
#include "Shader.h"
#include "Sprite.h"
#include "Window.h"
#include "Video/BitmapFontManager.h"

namespace gs2d {

/**
 * \brief Abstracts all video device operations
 *
 * This class works as a renderer. Sending all data to
 * the video device and telling it how to render it.
 * It also instantiates textures, sprites and shaders.
 */
class Video :
	public Application,
	public Window,
	public BitmapFontManager
{
	math::Vector2 m_cameraPos;

	float m_lineWidth;
	float m_depth;
	float m_virtualScreenHeight;

public:
	enum ALPHA_MODE
	{
		AM_PIXEL = 0,
		AM_ADD = 1,
		AM_ALPHA_TEST = 2,
		AM_NONE = 3,
		AM_MODULATE = 4,
		AM_UNKNOWN = 0xFFFFFFFF
	};

	enum TEXTUREFILTER_MODE
	{
		TM_UNKNOWN = 0xFFFFFFFF,
		TM_NEVER = 0,
		TM_IFNEEDED = 1,
		TM_ALWAYS = 2
	};

	enum BLEND_MODE
	{
		BM_ADD = 0,
		BM_MODULATE = 1,
	};

	struct VIDEO_MODE
	{
		bool operator == (const VIDEO_MODE& other) const;
		bool operator < (const VIDEO_MODE &other) const;
		unsigned int width, height;
		Texture::PIXEL_FORMAT pf;
		GS_DWORD idx;
	};

	Video();

	typedef std::list<VIDEO_MODE> VIDEO_MODE_LIST;

	/// Loads the texture from a file in virtual memory
	virtual TexturePtr CreateTextureFromFileInMemory(
		const void *pBuffer,
		const unsigned int bufferLength,
		Color mask,
		const unsigned int width = 0,
		const unsigned int height = 0,
		const unsigned int nMipMaps = 0) = 0;

	/// Loads the texture from a file in a hard disk
	virtual TexturePtr LoadTextureFromFile(
		const str_type::string& fileName,
		Color mask,
		const unsigned int width = 0,
		const unsigned int height = 0,
		const unsigned int nMipMaps = 0) = 0;

	/// Creates a texture as render target.
	virtual TexturePtr CreateRenderTargetTexture(
		const unsigned int width,
		const unsigned int height,
		const Texture::TARGET_FORMAT fmt) = 0;

	/// Creates a sprite from a texture in virtual memory
	virtual SpritePtr CreateSprite(
		GS_BYTE *pBuffer,
		const unsigned int bufferLength,
		Color mask = constant::ZERO,
		const unsigned int width = 0,
		const unsigned int height = 0) = 0;

	/// Creates a sprite from a texture in a file
	virtual SpritePtr CreateSprite(
		const str_type::string& fileName,
		Color mask = constant::ZERO,
		const unsigned int width = 0,
		const unsigned int height = 0) = 0;

	/// Creates a sprite as render target
	virtual SpritePtr CreateRenderTarget(
		const unsigned int width,
		const unsigned int height,
		const Texture::TARGET_FORMAT format = Texture::TF_DEFAULT) = 0;

	/// Create a shader object and load/compile it.
	virtual ShaderPtr LoadShaderFromFile(
        const std::string& vsFileName,
        const std::string& vsEntry,
        const std::string& psFileName,
        const std::string& psEntry) = 0;

	/// Create a shader object and load/compile it.
	virtual ShaderPtr LoadShaderFromString(
		const std::string& vsShaderName,
		const std::string& vsCodeAsciiString,
        const std::string& vsEntry,
		const std::string& psShaderName,
		const std::string& psCodeAsciiString,
		const std::string& psEntry) = 0;

	virtual boost::any GetVideoInfo() = 0;

    virtual ShaderPtr GetDefaultShader() = 0;
    virtual ShaderPtr GetRectShader() = 0;
    virtual ShaderPtr GetFastShader() = 0;
    virtual ShaderPtr GetModulateShader() = 0;
    virtual ShaderPtr GetAddShader() = 0;
    virtual ShaderPtr GetCurrentShader() = 0;

	virtual ShaderContextPtr GetShaderContext() = 0;

	virtual bool SetCurrentShader(ShaderPtr shader) = 0;

	virtual boost::any GetGraphicContext() = 0;

	virtual VIDEO_MODE GetVideoMode(const unsigned int modeIdx) const = 0;
	virtual unsigned int GetVideoModeCount() const = 0;

	virtual bool ResetVideoMode(
		const VIDEO_MODE& mode,
		const bool toggleFullscreen = false) = 0;

	virtual bool ResetVideoMode(
		const unsigned int width,
		const unsigned int height,
		const Texture::PIXEL_FORMAT pfBB,
		const bool toggleFullscreen = false) = 0;

	virtual bool SetRenderTarget(SpritePtr pTarget, const unsigned int target = 0) = 0;
	virtual unsigned int GetMaxRenderTargets() const = 0;
	virtual unsigned int GetMaxMultiTextures() const = 0;
	virtual bool SetBlendMode(const unsigned int passIdx, const BLEND_MODE mode) = 0;
	virtual BLEND_MODE GetBlendMode(const unsigned int passIdx) const = 0;
	virtual bool UnsetTexture(const unsigned int passIdx) = 0;

	virtual void SetZBuffer(const bool enable) = 0;
	virtual bool GetZBuffer() const = 0;

	virtual void SetZWrite(const bool enable) = 0;
	virtual bool GetZWrite() const = 0;

	virtual bool SetClamp(const bool set) = 0;
	virtual bool GetClamp() const = 0;

	virtual bool SetScissor(const math::Rect2D &rect) = 0;
	virtual bool SetScissor(const bool &enable) = 0;
	virtual math::Rect2D GetScissor() const = 0;
	virtual void UnsetScissor() = 0;

	virtual bool DrawLine(const math::Vector2 &p1, const math::Vector2 &p2, const Color& color1, const Color& color2) = 0;

	virtual bool DrawRectangle(
		const math::Vector2 &v2Pos,
		const math::Vector2 &v2Size,
		const Color& color,
		const float angle = 0.0f,
		const Sprite::ENTITY_ORIGIN origin = Sprite::EO_DEFAULT) = 0;

	virtual bool DrawRectangle(
		const math::Vector2 &v2Pos,
		const math::Vector2 &v2Size,
		const Color& color0,
		const Color& color1,
		const Color& color2,
		const Color& color3,
		const float angle = 0.0f,
		const Sprite::ENTITY_ORIGIN origin = Sprite::EO_DEFAULT) = 0;

	virtual void SetBGColor(const Color& backgroundColor) = 0;
	virtual Color GetBGColor() const = 0;

	virtual bool BeginSpriteScene(const Color& dwBGColor = constant::ZERO) = 0;
	virtual bool EndSpriteScene() = 0;
	virtual bool BeginTargetScene(const Color& dwBGColor = constant::ZERO, const bool clear = true) = 0;
	virtual bool EndTargetScene() = 0;

	virtual bool SetAlphaMode(const ALPHA_MODE mode) = 0;
	virtual ALPHA_MODE GetAlphaMode() const = 0;

	virtual bool SetFilterMode(const TEXTUREFILTER_MODE tfm) = 0;
	virtual TEXTUREFILTER_MODE GetFilterMode() const = 0;

	virtual bool Rendering() const = 0;

	virtual bool SaveScreenshot(
		const str_type::char_t* fileName,
		const Texture::BITMAP_FORMAT fmt = Texture::BF_BMP,
		math::Rect2D rect = math::Rect2D(0,0,0,0)) = 0;

	virtual math::Vector2 ComputeCarretPosition(
		const str_type::string& font,
		const str_type::string& text,
		const unsigned int pos);
	
	virtual math::Vector2 ComputeTextBoxSize(const str_type::string& font, const str_type::string& text);
	
	virtual unsigned int FindClosestCarretPosition(
		const str_type::string& font,
		const str_type::string &text,
		const math::Vector2 &textPos,
		const math::Vector2 &reference);
	
	virtual bool DrawBitmapText(
		const math::Vector2 &v2Pos,
		const str_type::string& text,
		const str_type::string& font,
		const Color& color,
		const float scale = 1.0f);

	bool ManageLoop();

	virtual bool SetCameraPos(const math::Vector2& pos);
	virtual bool MoveCamera(const math::Vector2& dir);
	virtual math::Vector2 GetCameraPos() const;
	virtual bool SetSpriteDepth(const float depth);
	virtual float GetSpriteDepth() const;
	virtual void SetLineWidth(const float width);
	virtual float GetLineWidth() const;
	virtual void SetVirtualScreenHeight(const float height);
	virtual float GetVirtualScreenHeight() const;
	virtual float GetScaleFactor() const;
};

/// Instantiate a Video object (must be defined in the API specific code)
GS2D_API VideoPtr CreateVideo(
	const unsigned int width,
	const unsigned int height,
	const str_type::string& winTitle,
	const bool windowed,
	const bool sync,
	const Platform::FileIOHubPtr& fileIOHub,
	const Texture::PIXEL_FORMAT pfBB = Texture::PF_UNKNOWN,
	const bool maximizable = false);

#if defined(ANDROID) || defined(APPLE_IOS)
/// Instantiate a Video object (must be defined in the API specific code)
GS2D_API VideoPtr CreateVideo(const unsigned int width, const unsigned int height, const Platform::FileIOHubPtr& fileIOHub);
#endif

} // namespace gs2d

#endif
