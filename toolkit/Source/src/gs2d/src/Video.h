#ifndef GS2D_VIDEO_H_
#define GS2D_VIDEO_H_

#include <list>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdint>

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

	float m_depth;
	float m_virtualScreenHeight;

protected:
	bool m_rendering;

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

	struct VIDEO_MODE
	{
		bool operator == (const VIDEO_MODE& other) const;
		bool operator < (const VIDEO_MODE &other) const;
		unsigned int width, height;
		Texture::PIXEL_FORMAT pf;
		uint32_t idx;
	};

	Video();

    bool IsRendering() const;

	typedef std::list<VIDEO_MODE> VIDEO_MODE_LIST;

	/// Loads the texture from a file in virtual memory
	virtual TexturePtr CreateTextureFromFileInMemory(
		const void *pBuffer,
		const unsigned int bufferLength,
		const unsigned int nMipMaps = 0) = 0;

	/// Loads the texture from a file in a hard disk
	virtual TexturePtr LoadTextureFromFile(
		const str_type::string& fileName,
		const unsigned int nMipMaps = 0) = 0;

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

	virtual void SetZBuffer(const bool enable) = 0;
	virtual bool GetZBuffer() const = 0;

	virtual void SetBGColor(const Color& backgroundColor) = 0;
	virtual Color GetBGColor() const = 0;

	virtual bool BeginRendering(const Color& bgColor = math::constant::ZERO_VECTOR4) = 0;
	virtual bool EndRendering() = 0;

	virtual bool SetAlphaMode(const ALPHA_MODE mode) = 0;
	virtual ALPHA_MODE GetAlphaMode() const = 0;

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
