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

#include "GLSDLVideo.h"

#include "../GL/GLSprite.h"
#include "../GL/Cg/GLCgShader.h"
#include <SDL/SDL.h>

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
	const Shader::SHADER_PROFILE profile,
	const char *entry)
{
	ShaderPtr shader = ShaderPtr(new GLCgShader(this));
	if (shader->LoadShaderFromFile(m_shaderContext, fileName, focus, profile, entry))
	{
		return shader;
	}
	return ShaderPtr();
}

ShaderPtr GLSDLVideo::LoadShaderFromString(
	const str_type::string& shaderName,
	const std::string& codeAsciiString,
	const Shader::SHADER_FOCUS focus,
	const Shader::SHADER_PROFILE profile,
	const char *entry)
{
	ShaderPtr shader(new GLCgShader(this));
	if (shader->LoadShaderFromString(m_shaderContext, shaderName, codeAsciiString, focus, profile, entry))
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
	TexturePtr texture(new GLTexture(weak_this, GetFileIOHub()->GetFileManager()));
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
	TexturePtr texture(new GLTexture(weak_this, GetFileIOHub()->GetFileManager()));
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
	TexturePtr texture(new GLTexture(weak_this, GetFileIOHub()->GetFileManager()));
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
	if (SDL_SetVideoMode(width, height, 0, AssembleFlags(IsWindowed(), IsMaximizable(), SyncEnabled())) != 0)
	{
		m_screenSize.x = static_cast<float>(width);
		m_screenSize.y = static_cast<float>(height);

		GLVideo::StartApplication(width, height, GetWindowTitle(), IsWindowed(), SyncEnabled());
		RecoverAll();

		ScreenSizeChangeListenerPtr listener = m_screenSizeChangeListener.lock();
		if (listener)
			listener->ScreenSizeChanged(GetScreenSizeF());

		return true;
	}
	else
	{
		return false;
	}
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
