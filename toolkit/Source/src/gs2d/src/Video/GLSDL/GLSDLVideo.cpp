#include "GLSDLVideo.h"

#include "../GL/GLShader.h"
#include "../GL/GLTexture.h"

#include <SDL2/SDL.h>

namespace gs2d {

VideoPtr CreateVideo(
	const unsigned int width,
	const unsigned int height,
	const str_type::string& winTitle,
	const bool windowed,
	const bool sync,
	const Platform::FileIOHubPtr& fileIOHub,
	const Texture::PIXEL_FORMAT pfBB,
	const bool maximizable)
{
	return GLSDLVideo::Create(width, height, winTitle, windowed, sync, fileIOHub, pfBB, maximizable);
}

boost::shared_ptr<GLSDLVideo> GLSDLVideo::Create(
	const unsigned int width,
	const unsigned int height,
	const str_type::string& winTitle,
	const bool windowed,
	const bool sync,
	const Platform::FileIOHubPtr& fileIOHub,
	const Texture::PIXEL_FORMAT pfBB,
	const bool maximizable)
{
	boost::shared_ptr<GLSDLVideo> p(
		new GLSDLVideo(fileIOHub, width, height, winTitle, windowed, sync, maximizable));
	p->weak_this = p;
	return p;
}

GLSDLVideo::GLSDLVideo(
	Platform::FileIOHubPtr fileIOHub,
	const unsigned int width,
	const unsigned int height,
	const str_type::string& winTitle,
	const bool windowed,
	const bool sync,
	const bool maximizable) :
	SDLWindow(fileIOHub)
{
	StartApplication(width, height, winTitle, windowed, sync, Texture::PF_UNKNOWN, maximizable);
}

bool GLSDLVideo::StartApplication(
	const unsigned int width,
	const unsigned int height,
	const str_type::string& winTitle,
	const bool windowed,
	const bool sync,
	const Texture::PIXEL_FORMAT pfBB,
	const bool maximizable)
{
	bool rW, rGL;
	rW = SDLWindow::StartApplication(width, height, winTitle, windowed, sync, pfBB, maximizable);
	rGL =  GLVideo::StartApplication(width, height, winTitle, windowed, sync, pfBB, maximizable);
	return (rW && rGL);
}

bool GLSDLVideo::EndRendering()
{
	ComputeFPSRate();
	GLVideo::EndRendering();
	return SDLWindow::EndRendering();
}

ShaderPtr GLSDLVideo::LoadShaderFromFile(
	const std::string& vsFileName,
	const std::string& vsEntry,
	const std::string& psFileName,
	const std::string& psEntry)
{
	ShaderPtr shader = ShaderPtr(new GLShader(GetFileIOHub()->GetFileManager()));
	if (shader->LoadShaderFromFile(ShaderContextPtr(), vsFileName, vsEntry, psFileName, psEntry))
	{
		return shader;
	}
	return ShaderPtr();
}

ShaderPtr GLSDLVideo::LoadShaderFromString(
	const std::string& vsShaderName,
	const std::string& vsCodeAsciiString,
	const std::string& vsEntry,
	const std::string& psShaderName,
	const std::string& psCodeAsciiString,
	const std::string& psEntry)
{
	ShaderPtr shader(new GLShader(GetFileIOHub()->GetFileManager()));
	if (shader->LoadShaderFromString(
		ShaderContextPtr(),
		vsShaderName,
		vsCodeAsciiString,
		vsEntry,
		psShaderName,
		psCodeAsciiString,
		psEntry))
	{
		return shader;
	}
	return ShaderPtr();
}

TexturePtr GLSDLVideo::CreateTextureFromFileInMemory(
	const void *pBuffer,
	const unsigned int bufferLength,
	const unsigned int width,
	const unsigned int height,
	const unsigned int nMipMaps)
{
	TexturePtr texture(GLTexture::Create(weak_this, GetFileIOHub()->GetFileManager()));
	if (texture->LoadTexture(weak_this, pBuffer, width, height, nMipMaps, bufferLength))
	{
		return texture;
	}
	return TexturePtr();
}

TexturePtr GLSDLVideo::LoadTextureFromFile(
	const str_type::string& fileName,
	const unsigned int width,
	const unsigned int height,
	const unsigned int nMipMaps)
{
	TexturePtr texture(GLTexture::Create(weak_this, GetFileIOHub()->GetFileManager()));
	if (texture->LoadTexture(weak_this, fileName, width, height, nMipMaps))
	{
		return texture;
	}
	return TexturePtr();
}

bool GLSDLVideo::ResetVideoMode(
	const VIDEO_MODE& mode,
	const bool toggleFullscreen)
{
	return ResetVideoMode(mode.width, mode.height, mode.pf, toggleFullscreen);
}

bool GLSDLVideo::ResetVideoMode(
	const unsigned int width,
	const unsigned int height,
	const Texture::PIXEL_FORMAT pfBB,
	const bool toggleFullscreen)
{
	return ResetVideoMode(width, height, pfBB, toggleFullscreen, true);
}

bool GLSDLVideo::ResetVideoMode(
	const unsigned int width,
	const unsigned int height,
	const Texture::PIXEL_FORMAT pfBB,
	const bool toggleFullscreen,
	const bool forceWindowResize)
{
	if (toggleFullscreen)
	{
		SDL_SetWindowFullscreen(m_window, (IsWindowed()) ? SDL_WINDOW_FULLSCREEN : 0);
	}

	if (forceWindowResize)
	{
		SDL_SetWindowSize(m_window, static_cast<int>(width), static_cast<int>(height));
	}

	// perform OpenGL context cleanup
	SDL_GL_DeleteContext(m_glcontext);
	m_glcontext = SDL_GL_CreateContext(m_window);

	m_screenSize.x = static_cast<float>(width);
	m_screenSize.y = static_cast<float>(height);

	GLVideo::StartApplication(width, height, GetWindowTitle(), IsWindowed(), SyncEnabled());
	RecoverAll();

	// call listener if there's any
	ScreenSizeChangeListenerPtr listener = m_screenSizeChangeListener.lock();
	if (listener)
	{
		listener->ScreenSizeChanged(GetScreenSizeF());
	}

	return true;
}

} // namespace gs2d
