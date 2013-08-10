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

#include "GLTexture.h"
#include "GLVideo.h"

#include "../../Application.h"

#include "../../Platform/Platform.h"

#include <SOIL.h>

namespace gs2d {

boost::shared_ptr<GLTexture> GLTexture::Create(VideoWeakPtr video, Platform::FileManagerPtr fileManager)
{
    GLTexturePtr texture = GLTexturePtr(new GLTexture(video, fileManager));
    texture->weak_this = texture;
    return texture;
}

static void ApplyPixelMask(unsigned char *ht_map, const Color mask, const int channels, const int width, const int height);

GLuint GLTexture::m_textureID(1000);

GLTexture::TEXTURE_INFO::TEXTURE_INFO() :
	m_frameBuffer(0),
	m_renderBuffer(0),
	m_texture(0)
{
}

GLTexture::GLTexture(VideoWeakPtr video, Platform::FileManagerPtr fileManager) :
	m_fileManager(fileManager),
	m_bitmap(0),
	m_channels(0)
{
	m_video = boost::dynamic_pointer_cast<GLVideo>(video.lock());
}

GLTexture::~GLTexture()
{
	GLVideoPtr video = m_video.lock();

	if (video)
		video->RemoveRecoverableResource(this);

	FreeBitmap();
	DeleteGLTexture();

	if (m_textureInfo.m_frameBuffer != 0)
	{
		GLuint buffers[1] = { m_textureInfo.m_frameBuffer };
		glDeleteFramebuffers(1, buffers);
	}
	if (m_textureInfo.m_renderBuffer != 0)
	{
		GLuint buffers[1] = { m_textureInfo.m_renderBuffer };
		glDeleteRenderbuffers(1, buffers);
	}
}

void GLTexture::DeleteGLTexture()
{
	if (m_textureInfo.m_texture != 0)
	{
		GLuint textures[1] = { m_textureInfo.m_texture };
		glDeleteTextures(1, textures);
		m_textureInfo.m_texture = 0;
	}
}

bool GLTexture::SetTexture(const unsigned int passIdx)
{
	return true;
}

Texture::PROFILE GLTexture::GetProfile() const
{
	return m_profile;
}

Texture::TYPE GLTexture::GetTextureType() const
{
	return m_type;
}

boost::any GLTexture::GetTextureObject()
{
	return m_textureInfo.m_texture;
}

const GLTexture::TEXTURE_INFO& GLTexture::GetTextureInfo() const
{
	return m_textureInfo;
}

math::Vector2 GLTexture::GetBitmapSize() const
{
	return math::Vector2(static_cast<float>(m_profile.width), static_cast<float>(m_profile.height));
}

bool GLTexture::LoadTexture(
	VideoWeakPtr video,
	const str_type::string& fileName,
	Color mask,
	const unsigned int width,
	const unsigned int height,
	const unsigned int nMipMaps)
{
	m_fileName = fileName;
	Platform::FileBuffer out;
	m_fileManager->GetFileBuffer(fileName, out);
	if (!out)
	{
		ShowMessage(fileName + " could not load buffer", GSMT_ERROR);
		return false;
	}
	return LoadTexture(video, out->GetAddress(), mask, width, height, nMipMaps, static_cast<unsigned int>(out->GetBufferSize()));
}

bool GLTexture::LoadTexture(
	VideoWeakPtr video,
	const void* pBuffer,
	Color mask,
	const unsigned int width,
	const unsigned int height,
	const unsigned int nMipMaps,
	const unsigned int bufferLength)
{
	const bool maskingEnabled = (mask != 0x0);
	int iWidth, iHeight;
	const int forceChannels = maskingEnabled ? SOIL_LOAD_RGBA : SOIL_LOAD_AUTO;
	m_bitmap = SOIL_load_image_from_memory((unsigned char*)pBuffer, bufferLength, &iWidth, &iHeight, &m_channels, forceChannels);

	if (!m_bitmap)
	{
		ShowMessage(m_fileName + " couldn't create texture from file", GSMT_ERROR);
		return false;
	}

	if (maskingEnabled)
	{
		m_channels = 4;
		ApplyPixelMask(m_bitmap, mask, m_channels, iWidth, iHeight);
	}

	CreateTextureFromBitmap(m_bitmap, iWidth, iHeight, m_channels, true);

	if (!m_textureInfo.m_texture)
	{
		ShowMessage(m_fileName + " couldn't load texture", GSMT_ERROR);
		FreeBitmap();
		return false;
	}
	else
	{
		m_type = TT_STATIC;
		m_profile.width = static_cast<unsigned int>(iWidth);
		m_profile.height = static_cast<unsigned int>(iHeight);
		m_profile.originalWidth = m_profile.width;
		m_profile.originalHeight = m_profile.height;
		m_profile.mask = mask;
		ShowMessage(Platform::GetFileName(m_fileName) + " texture loaded", GSMT_INFO);
		m_video.lock()->InsertRecoverableResource(this);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}

bool GLTexture::CreateRenderTarget(
	VideoWeakPtr video,
	const unsigned int width,
	const unsigned int height,
	const TARGET_FORMAT fmt)
{
	m_textureInfo.m_texture = m_textureID++;

	glGenTextures(1, &m_textureInfo.m_texture);
	glBindTexture(GL_TEXTURE_2D, m_textureInfo.m_texture);

	// we'll ignore the user format and force using ARGB format
	GS2D_UNUSED_ARGUMENT(fmt);

	m_channels = 4;
	m_textureInfo.glTargetFmt = GL_RGBA;
	m_textureInfo.glPixelType = GL_UNSIGNED_BYTE;
	m_textureInfo.gsTargetFmt = TF_ARGB;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		m_textureInfo.glTargetFmt,
		static_cast<GLsizei>(width),
		static_cast<GLsizei>(height),
		0,
		static_cast<GLenum>(m_textureInfo.glTargetFmt),
		m_textureInfo.glPixelType,
		NULL);

	// attach 2D texture
	glGenFramebuffers(1, &m_textureInfo.m_frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_textureInfo.m_frameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureInfo.m_texture, 0);

	// create depth buffer
	glGenRenderbuffers(1, &m_textureInfo.m_renderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_textureInfo.m_renderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_textureInfo.m_renderBuffer);

	CheckFrameBufferStatus(m_textureInfo.m_frameBuffer, m_textureInfo.m_texture, true);
	GLVideo::UnbindFrameBuffer();

	m_type = TT_RENDER_TARGET;
	m_profile.width = width;
	m_profile.height = height;
	m_profile.originalWidth = m_profile.width;
	m_profile.originalHeight = m_profile.height;

	m_video.lock()->InsertRecoverableResource(this);

	glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}

void GLTexture::CreateTextureFromBitmap(GS_BYTE* data, const int width, const int height, const int channels, const bool pow2)
{
	DeleteGLTexture();
	if (data)
	{
		m_textureInfo.m_texture = SOIL_create_OGL_texture(data, width, height, channels, m_textureID++, pow2 ? SOIL_FLAG_POWER_OF_TWO : 0);
	}
}

void GLTexture::FreeBitmap()
{
	if (m_bitmap)
	{
		SOIL_free_image_data(m_bitmap);
		m_bitmap = 0;
	}
}

void GLTexture::Recover()
{
	if (m_type == TT_STATIC)
	{
		CreateTextureFromBitmap(m_bitmap, m_profile.originalWidth, m_profile.originalHeight, m_channels, true);
		ShowMessage("Texture recovered: " + Platform::GetFileName(m_fileName), GSMT_INFO);
	}
	else if (m_type == TT_RENDER_TARGET)
	{
		CreateTextureFromBitmap(m_textureInfo.renderTargetBackup.get(), m_profile.originalWidth, m_profile.originalHeight, m_channels, false);
	}
}

bool GLTexture::IsAllBlack() const
{
	const GS_BYTE maxTolerance = 0xF;
	const GS_BYTE* bitmap = (m_type == TT_RENDER_TARGET) ? m_textureInfo.renderTargetBackup.get() : m_bitmap;
	const size_t size = m_profile.originalWidth * m_profile.originalHeight * m_channels;
	for (std::size_t t = 0; t < size; t++)
	{
		if (m_channels == 4)
		{
			if (t % 4 == 3)
			{
				continue;
			}
		}

		if (bitmap[t] > maxTolerance)
		{
			return false;
		}
	}
	return true;
}

bool GLTexture::SaveTargetSurfaceBackup()
{
	if (m_type != Texture::TT_RENDER_TARGET)
	{
		ShowMessage("Failed while trying to save backup of a non target surface", GSMT_ERROR);
		return false;
	}
	glBindTexture(GL_TEXTURE_2D, m_textureInfo.m_texture);
	const unsigned int bytes = 4;
	m_textureInfo.renderTargetBackup = boost::shared_array<GS_BYTE>(new GS_BYTE [m_profile.originalWidth * m_profile.originalHeight * bytes]);
	glGetTexImage(GL_TEXTURE_2D, 0, m_textureInfo.glTargetFmt, m_textureInfo.glPixelType, m_textureInfo.renderTargetBackup.get());
	return true;
}

bool GLTexture::SaveBitmap(const str_type::char_t* name, const Texture::BITMAP_FORMAT fmt)
{
	str_type::string fileName = name, ext;
	const int type = GetSOILTexType(fmt, ext);
	
	if (!Platform::IsExtensionRight(fileName, ext))
		fileName.append(ext);

	unsigned char* data = 0;
	if (m_bitmap)
	{
		data = m_bitmap;
	}
	else
	{
		if (!m_textureInfo.renderTargetBackup.get())
			SaveTargetSurfaceBackup();
		data = m_textureInfo.renderTargetBackup.get();
	}

	const bool r = (SOIL_save_image(
		fileName.c_str(),
		type,
		m_profile.originalWidth,
		m_profile.originalHeight,
		m_channels,
		data) != 0);

	if (!r)
		ShowMessage(str_type::string("Couldn't save texture ") + fileName, GSMT_ERROR);

	return r;
}

void CheckFrameBufferStatus(const GLuint fbo, const GLuint tex, const bool showSuccessMessage)
{
	const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	str_type::stringstream ss;
	ss << GS_L("fboID ") << fbo << GS_L(" (texture id") << tex << GS_L("): ");
	switch (status)
	{
	case GL_FRAMEBUFFER_COMPLETE:
		#ifdef DEBUG
		 if (showSuccessMessage)
		 {
			 ss << GS_L(" render target texture created successfully");
			 ShowMessage(ss.str(), GSMT_INFO);
		 }
		#endif
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		ss << GS_L(" incomplete attachment");
		ShowMessage(ss.str(), GSMT_ERROR);
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		ss << GS_L(" incomplete missing attachment");
		ShowMessage(ss.str(), GSMT_ERROR);
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED:
		ss << GS_L(" unsupported");
		ShowMessage(ss.str(), GSMT_ERROR);
		break;
	default:
		ss << GS_L(" unknown status");
		ShowMessage(ss.str(), GSMT_ERROR);
	}
}

static void ApplyPixelMask(unsigned char *ht_map, const Color mask, const int channels, const int width, const int height)
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

int GetSOILTexType(const Texture::BITMAP_FORMAT fmt, str_type::string& ext)
{
	switch (fmt)
	{
	case Texture::BF_BMP:
		ext = ".bmp";
		return SOIL_SAVE_TYPE_BMP;
		break;
	case Texture::BF_JPG:
		ext = ".bmp";
		return SOIL_SAVE_TYPE_BMP;
		break;
	case Texture::BF_PNG:
		ext = ".bmp";
		return SOIL_SAVE_TYPE_BMP;
		break;
	case Texture::BF_TGA:
		ext = ".tga";
		return SOIL_SAVE_TYPE_TGA;
		break;
	case Texture::BF_DDS:
		ext = ".dds";
		return SOIL_SAVE_TYPE_DDS;
		break;
	case Texture::BF_HDR:
		ext = ".bmp";
		return SOIL_SAVE_TYPE_BMP;
		break;
	default:
		ext = ".bmp";
		return SOIL_SAVE_TYPE_BMP;
		break;
	}
	return SOIL_SAVE_TYPE_BMP;
}

} // namespace gs2d
