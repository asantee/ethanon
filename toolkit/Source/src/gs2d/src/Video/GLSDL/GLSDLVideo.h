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
		Color mask,
		const unsigned int width,
		const unsigned int height,
		const unsigned int nMipMaps);
	
	TexturePtr LoadTextureFromFile(
		const str_type::string& fileName,
		Color mask,
		const unsigned int width,
		const unsigned int height,
		const unsigned int nMipMaps);
	
	TexturePtr CreateRenderTargetTexture(
		const unsigned int width,
		const unsigned int height,
		const Texture::TARGET_FORMAT fmt);
	
	SpritePtr CreateSprite(
		GS_BYTE *pBuffer,
		const unsigned int bufferLength,
		Color mask = constant::ZERO,
		const unsigned int width = 0,
		const unsigned int height = 0);
	
	SpritePtr CreateSprite(
		const str_type::string& fileName,
		Color mask = constant::ZERO,
		const unsigned int width = 0,
		const unsigned int height = 0);
	
	SpritePtr CreateRenderTarget(
		const unsigned int width,
		const unsigned int height,
		const Texture::TARGET_FORMAT format = Texture::TF_DEFAULT);

	ShaderPtr LoadShaderFromFile(
		const str_type::string& fileName,
		const Shader::SHADER_FOCUS focus,
		const Shader::SHADER_PROFILE profile = Shader::SP_HIGHEST,
		const char *entry = 0);
	
	ShaderPtr LoadShaderFromString(
		const str_type::string& shaderName,
		const std::string& codeAsciiString,
		const Shader::SHADER_FOCUS focus,
		const Shader::SHADER_PROFILE profile = Shader::SP_HIGHEST,
		const char *entry = 0);

	bool EndSpriteScene();
	
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
