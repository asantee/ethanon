#ifndef GS2D_GL_SDL_VIDEO_H_
#define GS2D_GL_SDL_VIDEO_H_

#include "../GL/GLVideo.h"
#include "../../Platform/sdl/SDLWindow.h"

namespace gs2d {

class GLSDLVideo : public GLVideo, public SDLWindow
{
	bool StartApplication(
		const unsigned int width,
		const unsigned int height,
		const str_type::string& winTitle,
		const bool windowed,
		const bool sync,
		const Texture::PIXEL_FORMAT pfBB = Texture::PF_UNKNOWN,
		const bool maximizable = false);

	boost::weak_ptr<GLSDLVideo> weak_this;

protected:
	bool ResetVideoMode(
		const unsigned int width,
		const unsigned int height,
		const Texture::PIXEL_FORMAT pfBB,
		const bool toggleFullscreen,
		const bool forceWindowResize);

public:
	static boost::shared_ptr<GLSDLVideo> Create(
		const unsigned int width,
		const unsigned int height,
		const str_type::string& winTitle,
		const bool windowed,
		const bool sync,
		const Platform::FileIOHubPtr& fileIOHub,
		const Texture::PIXEL_FORMAT pfBB,
		const bool maximizable);

	GLSDLVideo(
		Platform::FileIOHubPtr fileIOHub,
		const unsigned int width,
		const unsigned int height,
		const str_type::string& winTitle,
		const bool windowed,
		const bool sync,
		const bool maximizable);

	TexturePtr CreateTextureFromFileInMemory(
		const void *pBuffer,
		const unsigned int bufferLength,
		const unsigned int width,
		const unsigned int height,
		const unsigned int nMipMaps);
	
	TexturePtr LoadTextureFromFile(
		const str_type::string& fileName,
		const unsigned int width,
		const unsigned int height,
		const unsigned int nMipMaps);

	SpritePtr CreateSprite(
		unsigned char *pBuffer,
		const unsigned int bufferLength,
		const unsigned int width = 0,
		const unsigned int height = 0);
	
	SpritePtr CreateSprite(
		const str_type::string& fileName,
		const unsigned int width = 0,
		const unsigned int height = 0);

	ShaderPtr LoadShaderFromFile(
	    const std::string& vsFileName,
        const std::string& vsEntry,
        const std::string& psFileName,
        const std::string& psEntry);
	
	ShaderPtr LoadShaderFromString(
		const std::string& vsShaderName,
        const std::string& vsCodeAsciiString,
        const std::string& vsEntry,
        const std::string& psShaderName,
        const std::string& psCodeAsciiString,
        const std::string& psEntry);

	bool EndRendering();
	
	bool ResetVideoMode(
		const VIDEO_MODE& mode,
		const bool toggleFullscreen = false);

	bool ResetVideoMode(
		const unsigned int width,
		const unsigned int height,
		const Texture::PIXEL_FORMAT pfBB,
		const bool toggleFullscreen = false);
};
	
} // namespace gs2d

#endif
