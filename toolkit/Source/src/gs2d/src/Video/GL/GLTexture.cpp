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

Texture::PROFILE GLTexture::GetProfile() const
{
	return m_profile;
}

GLuint GLTexture::GetTexture() const
{
	return m_textureInfo.m_texture;
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
	return LoadTexture(video, out->GetAddress(), width, height, nMipMaps, static_cast<unsigned int>(out->GetBufferSize()));
}

bool GLTexture::LoadTexture(
	VideoWeakPtr video,
	const void* pBuffer,
	const unsigned int width,
	const unsigned int height,
	const unsigned int nMipMaps,
	const unsigned int bufferLength)
{
	int iWidth, iHeight;
	m_bitmap = SOIL_load_image_from_memory((unsigned char*)pBuffer, bufferLength, &iWidth, &iHeight, &m_channels, SOIL_LOAD_AUTO);

	if (!m_bitmap)
	{
		ShowMessage(m_fileName + " couldn't create texture from file", GSMT_ERROR);
		return false;
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
		m_profile.width = static_cast<unsigned int>(iWidth);
		m_profile.height = static_cast<unsigned int>(iHeight);
		m_profile.originalWidth = m_profile.width;
		m_profile.originalHeight = m_profile.height;
		ShowMessage(Platform::GetFileName(m_fileName) + " texture loaded", GSMT_INFO);
		m_video.lock()->InsertRecoverableResource(this);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}

void GLTexture::CreateTextureFromBitmap(unsigned char* data, const int width, const int height, const int channels, const bool pow2)
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
	CreateTextureFromBitmap(m_bitmap, m_profile.originalWidth, m_profile.originalHeight, m_channels, true);
	ShowMessage("Texture recovered: " + Platform::GetFileName(m_fileName), GSMT_INFO);
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
