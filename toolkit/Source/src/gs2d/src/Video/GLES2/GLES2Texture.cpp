/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2013 Andre Santee
 http://ethanonengine.com/

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

#include "GLES2Texture.h"
#include "GLES2Video.h"
#include "GLES2UniformParameter.h"
#include <SOIL.h>
#include <sstream>

namespace gs2d {

const str_type::string GLES2Texture::TEXTURE_LOG_FILE("GLES2Texture.log.txt");
const str_type::string GLES2Texture::ETC1_FILE_FORMAT("pkm");
GLuint GLES2Texture::m_textureID(1000);
Platform::FileLogger GLES2Texture::m_logger(Platform::FileLogger::GetLogDirectory() + GLES2Texture::TEXTURE_LOG_FILE);
	
void CheckFrameBufferStatus(const Platform::FileLogger& logger, const GLuint fbo, const GLuint tex, const bool showSuccessMessage)
{
	const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	str_type::stringstream ss;
	ss << GS_L("fboID ") << fbo << GS_L(" (") << tex << GS_L("): ");
	switch (status)
	{
	case GL_FRAMEBUFFER_COMPLETE:
		if (showSuccessMessage)
		{
			ss << GS_L(" render target texture created successfully");
			logger.Log(ss.str(), Platform::FileLogger::INFO);
		}
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		ss << GS_L(" incomplete attachment");
		logger.Log(ss.str(), Platform::FileLogger::ERROR);
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
		ss << GS_L(" incomplete dimensions");
		logger.Log(ss.str(), Platform::FileLogger::ERROR);
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		ss << GS_L(" incomplete missing attachment");
		logger.Log(ss.str(), Platform::FileLogger::ERROR);
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED:
		ss << GS_L(" unsupported");
		logger.Log(ss.str(), Platform::FileLogger::ERROR);
		break;
	default:
		ss << GS_L(" unknown status");
		logger.Log(ss.str(), Platform::FileLogger::ERROR);
	}
}

void ApplyPixelMask(unsigned char *ht_map, const Color mask, const int channels, const int width, const int height)
{
	if (channels == 4)
	{
		const std::size_t numBytes = width * height * channels;
		for (std::size_t i = 0; i < numBytes; i += channels)
		{
			unsigned char& r = ht_map[i + 0];
			unsigned char& g = ht_map[i + 1];
			unsigned char& b = ht_map[i + 2];
			unsigned char& a = ht_map[i + 3];

			if ((r == mask.r && g == mask.g && b == mask.b && mask.a == 0xFF)
				|| (a == 0x0))
			{
				r = g = b = a = 0x0;
			}
		}
	}
}

GLES2Texture::TEXTURE_INFO::TEXTURE_INFO() :
	m_texture(0),
	m_frameBuffer(0)
{
}

GLES2Texture::GLES2Texture(VideoWeakPtr video, const str_type::string& fileName, Platform::FileManagerPtr fileManager) :
		m_fileManager(fileManager),
		m_fileName(fileName),
		m_type(TT_NONE)
{
}

GLES2Texture::~GLES2Texture()
{
	if (m_textureInfo.m_texture != 0)
	{
		GLuint textures[1] = { m_textureInfo.m_texture };
		glDeleteTextures(1, textures);
		std::stringstream ss;
		ss << m_fileName << ": texture deleted ID " << m_textureInfo.m_texture; 
		m_logger.Log(ss.str(), Platform::FileLogger::INFO);
		m_textureInfo.m_texture = 0;
	}
	if (m_textureInfo.m_frameBuffer != 0)
	{
		GLuint buffers[1] = { m_textureInfo.m_frameBuffer };
		glDeleteFramebuffers(1, buffers);
		std::stringstream ss;
		ss << "Frame buffer deleted ID " << m_textureInfo.m_frameBuffer; 
		m_logger.Log(ss.str(), Platform::FileLogger::INFO);
	}
}

bool GLES2Texture::SetTexture(const unsigned int passIdx)
{
	GLES2UniformParameter::m_boundTexture2D = m_textureInfo.m_texture;
	glBindTexture(GL_TEXTURE_2D, m_textureInfo.m_texture);
	return true;
}

Texture::PROFILE GLES2Texture::GetProfile() const
{
	return m_profile;
}

Texture::TYPE GLES2Texture::GetTextureType() const
{
	return m_type;
}

boost::any GLES2Texture::GetTextureObject()
{
	return m_textureInfo.m_texture;
}

bool GLES2Texture::CreateRenderTarget(
	VideoWeakPtr video,
	const unsigned int width,
	const unsigned int height,
	const Texture::TARGET_FORMAT fmt)
{
	m_textureInfo.m_texture = m_textureID++;

	SetTexture(0);

	const GLint glfmt = (fmt == Texture::TF_ARGB) ? GL_RGBA : GL_RGB;

	glTexImage2D(GL_TEXTURE_2D, 0, glfmt,
		static_cast<GLsizei>(width), static_cast<GLsizei>(height),
		0, static_cast<GLenum>(glfmt), (fmt == Texture::TF_ARGB) ? GL_UNSIGNED_BYTE : GL_UNSIGNED_SHORT_5_6_5, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// attach 2D texture
	glGenFramebuffers(1, &m_textureInfo.m_frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_textureInfo.m_frameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureInfo.m_texture, 0);

	// create depth buffer... removed for now... will be back whenever it's needed
	/*GLuint depthRenderbuffer;
	glGenRenderbuffers(1, &depthRenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);*/

	CheckFrameBufferStatus(m_logger, m_textureInfo.m_frameBuffer, m_textureInfo.m_texture, true);
	UnbindFrameBuffer();

	m_type = TT_RENDER_TARGET;
	m_profile.width = width;
	m_profile.height = height;
	m_profile.originalWidth = m_profile.width;
	m_profile.originalHeight = m_profile.height;

	GLES2UniformParameter::m_boundTexture2D = 0;
	glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}

bool GLES2Texture::LoadTexture(
	VideoWeakPtr video,
	const str_type::string& fileName,
	Color mask,
	const unsigned int width,
	const unsigned int height,
	const unsigned int nMipMaps)
{
	m_fileName = fileName;
	const bool compressed = HasETC1CompressedVersion(m_fileName);

	Platform::FileBuffer out;
	m_fileManager->GetFileBuffer(m_fileName, out);
	if (!out)
	{
		m_logger.Log(m_fileName + " could not load buffer", Platform::FileLogger::ERROR);
		return false;
	}
	return LoadTexture(video, out->GetAddress(), mask, width, height, nMipMaps, out->GetBufferSize(), compressed);
}

bool GLES2Texture::LoadTexture(
	VideoWeakPtr video,
	const void* pBuffer,
	Color mask,
	const unsigned int width,
	const unsigned int height,
	const unsigned int nMipMaps,
	const unsigned int bufferLength,
	const bool compressed)
{
	if (compressed)
		return LoadETC1Texture(video, pBuffer, mask, width, height, nMipMaps, bufferLength);
	else
		return LoadTexture(video, pBuffer, mask, width, height, nMipMaps, bufferLength);
}

bool GLES2Texture::HasETC1CompressedVersion(str_type::string& fileName)
{
	// not supported on iOS
	if (Application::GetPlatformName() == "ios")
		return false;

	const GLubyte* extensions = glGetString(GL_EXTENSIONS);
	if (strstr((char*)extensions, "GL_IMG_texture_compression_pvrtc") == 0)
		return false;

	const str_type::string pkmFileName = Platform::RemoveExtension(fileName.c_str()).append(".").append(ETC1_FILE_FORMAT);
	const bool etc1VersionExists = (m_fileManager->FileExists(pkmFileName));
	if (etc1VersionExists)
		fileName = pkmFileName;
	return etc1VersionExists;
}

bool GLES2Texture::LoadTexture(
	VideoWeakPtr video,
	const void* pBuffer,
	Color mask,
	const unsigned int width,
	const unsigned int height,
	const unsigned int nMipMaps,
	const unsigned int bufferLength)
{
	int iWidth, iHeight, channels;

	const bool maskingEnabled = (mask != 0x0);
	const int forceChannels = maskingEnabled ? SOIL_LOAD_RGBA : SOIL_LOAD_AUTO;

	unsigned char *ht_map = SOIL_load_image_from_memory((unsigned char*)pBuffer, bufferLength, &iWidth, &iHeight, &channels, forceChannels);

	if (ht_map)
	{
		if (maskingEnabled)
		{
			channels = 4;
			ApplyPixelMask(ht_map, mask, channels, iWidth, iHeight);
		}
		m_textureInfo.m_texture = SOIL_create_OGL_texture(ht_map, iWidth, iHeight, channels, m_textureID++, SOIL_FLAG_POWER_OF_TWO);
	}

	std::stringstream ss;
	ss << m_fileName << " file ID " << m_textureInfo.m_texture;
	m_logger.Log(ss.str(), Platform::FileLogger::INFO);

	if (!m_textureInfo.m_texture)
	{
		m_logger.Log(m_fileName + " couldn't load texture", Platform::FileLogger::ERROR);
		video.lock()->Message(m_fileName + " couldn't load texture", GSMT_ERROR);
		SOIL_free_image_data(ht_map);
		return false;
	}

	m_type = TT_STATIC;
	m_profile.width = static_cast<unsigned int>(iWidth);
	m_profile.height = static_cast<unsigned int>(iHeight);
	m_profile.originalWidth = m_profile.width;
	m_profile.originalHeight = m_profile.height;
	m_logger.Log(m_fileName + " texture loaded", Platform::FileLogger::INFO);
	SOIL_free_image_data(ht_map);

	GLES2UniformParameter::m_boundTexture2D = 0;
	glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}

bool GLES2Texture::LoadETC1Texture(
	VideoWeakPtr video,
	const void* pBuffer,
	Color mask,
	const unsigned int width,
	const unsigned int height,
	const unsigned int nMipMaps,
	const unsigned int bufferLength)
{
	const ETC1Header* header = (ETC1Header*)pBuffer;
	if (strstr(header->tag, "PKM") == 0)
	{
		return false;
	}

	m_type = TT_STATIC;
	m_profile.width = static_cast<unsigned int>(Platform::ShortEndianSwap(header->texWidth));
	m_profile.height = static_cast<unsigned int>(Platform::ShortEndianSwap(header->texHeight));
	m_profile.originalWidth = static_cast<unsigned int>(Platform::ShortEndianSwap(header->origWidth));
	m_profile.originalHeight = static_cast<unsigned int>(Platform::ShortEndianSwap(header->origHeight));
	m_logger.Log(m_fileName + " texture loaded", Platform::FileLogger::INFO);

	const std::size_t size = (m_profile.width / 4) * (m_profile.height / 4) * 8;

	// not supported on iOS, so we'll only use the enum on non-ios devices
	GLenum internalformat = 0;
	#ifndef APPLE_IOS
		internalformat = GL_ETC1_RGB8_OES;
	#endif

	m_textureInfo.m_texture = m_textureID++;

	glBindTexture(GL_TEXTURE_2D, m_textureInfo.m_texture);

	unsigned char *data = (unsigned char*)pBuffer;
	glCompressedTexImage2D(GL_TEXTURE_2D, 0, internalformat, m_profile.width, m_profile.height, 0, size, data + sizeof(ETC1Header));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	GLES2UniformParameter::m_boundTexture2D = 0;
	glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}

math::Vector2 GLES2Texture::GetBitmapSize() const
{
	return math::Vector2(static_cast<float>(m_profile.width), static_cast<float>(m_profile.height));
}

const str_type::string& GLES2Texture::GetFileName() const
{
	return m_fileName;
}

GLuint GLES2Texture::GetTextureID() const
{
	return m_textureInfo.m_texture;
}

GLuint GLES2Texture::GetFrameBufferID() const
{
	return m_textureInfo.m_frameBuffer;
}

} // namespace gs2d
