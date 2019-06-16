#include "GLSDLVideo.h"

#include "../GL/GLSprite.h"
#include "../GL/Cg/GLCgShader.h"

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

bool GLSDLVideo::EndSpriteScene()
{
	ComputeFPSRate();
	GLVideo::EndSpriteScene();
	return SDLWindow::EndSpriteScene();
}

ShaderPtr GLSDLVideo::LoadShaderFromFile(
	const str_type::string& fileName,
	const Shader::SHADER_FOCUS focus,
	const char *entry)
{
	ShaderPtr shader = ShaderPtr(new GLCgShader(this));
	if (shader->LoadShaderFromFile(m_shaderContext, fileName, focus, entry))
	{
		return shader;
	}
	return ShaderPtr();
}

ShaderPtr GLSDLVideo::LoadShaderFromString(
	const str_type::string& shaderName,
	const std::string& codeAsciiString,
	const Shader::SHADER_FOCUS focus,
	const char *entry)
{
	ShaderPtr shader(new GLCgShader(this));
	if (shader->LoadShaderFromString(m_shaderContext, shaderName, codeAsciiString, focus, entry))
	{
		return shader;
	}
	return ShaderPtr();
}

TexturePtr GLSDLVideo::CreateTextureFromFileInMemory(
	const void *pBuffer,
	const unsigned int bufferLength,
	Color mask,
	const unsigned int width,
	const unsigned int height,
	const unsigned int nMipMaps)
{
	TexturePtr texture(GLTexture::Create(weak_this, GetFileIOHub()->GetFileManager()));
	if (texture->LoadTexture(weak_this, pBuffer, mask, width, height, nMipMaps, bufferLength))
	{
		return texture;
	}
	return TexturePtr();
}

TexturePtr GLSDLVideo::LoadTextureFromFile(
	const str_type::string& fileName,
	Color mask,
	const unsigned int width,
	const unsigned int height,
	const unsigned int nMipMaps)
{
	TexturePtr texture(GLTexture::Create(weak_this, GetFileIOHub()->GetFileManager()));
	if (texture->LoadTexture(weak_this, fileName, mask, width, height, nMipMaps))
	{
		return texture;
	}
	return TexturePtr();
}

TexturePtr GLSDLVideo::CreateRenderTargetTexture(
	const unsigned int width,
	const unsigned int height,
	const Texture::TARGET_FORMAT fmt)
{
	TexturePtr texture(GLTexture::Create(weak_this, GetFileIOHub()->GetFileManager()));
	if (texture->CreateRenderTarget(weak_this, width, height, fmt))
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

SpritePtr GLSDLVideo::CreateSprite(
	GS_BYTE *pBuffer,
	const unsigned int bufferLength,
	Color mask,
	const unsigned int width,
	const unsigned int height)
{
	SpritePtr sprite(new GLSprite);
	if (sprite->LoadSprite(weak_this, pBuffer, bufferLength, mask, width, height))
	{
		return sprite;
	}
	return SpritePtr();
}

SpritePtr GLSDLVideo::CreateSprite(
	const str_type::string& fileName,
	Color mask,
	const unsigned int width,
	const unsigned int height)
{
	SpritePtr sprite(new GLSprite);
	if (sprite->LoadSprite(weak_this, fileName, mask, width, height))
	{
		return sprite;
	}
	return SpritePtr();
}

SpritePtr GLSDLVideo::CreateRenderTarget(
	const unsigned int width,
	const unsigned int height,
	const Texture::TARGET_FORMAT format)
{
	SpritePtr sprite(new GLSprite);
	if (sprite->CreateRenderTarget(weak_this, width, height, format))
	{
		return sprite;
	}
	return SpritePtr();
}

} // namespace gs2d
