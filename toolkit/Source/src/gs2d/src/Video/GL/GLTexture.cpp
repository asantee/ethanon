#include "GLTexture.h"

#include "../../Application.h"

#include "../../Platform/Platform.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcomma"
#define STB_IMAGE_IMPLEMENTATION
#include "../../../vendors/stb/stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "../../../vendors/stb/stb_image_resize.h"
#pragma clang diagnostic pop

namespace gs2d {

boost::shared_ptr<GLTexture> GLTexture::Create(VideoWeakPtr video, Platform::FileManagerPtr fileManager)
{
    GLTexturePtr texture = GLTexturePtr(new GLTexture(video, fileManager));
    texture->weak_this = texture;
    return texture;
}

GLTexture::GLTexture(VideoWeakPtr video, Platform::FileManagerPtr fileManager) :
	m_texture(0),
	m_fileManager(fileManager)
{
}


GLTexture::~GLTexture()
{
	Free();
}

void GLTexture::Free()
{
	if (m_texture != 0)
	{
		glDeleteTextures(1, &m_texture);
		m_texture = 0;
	}
}

GLuint GLTexture::GetTexture() const
{
	return m_texture;
}

math::Vector2 GLTexture::GetBitmapSize() const
{
	return m_resolution;
}

bool GLTexture::LoadTexture(
	VideoWeakPtr video,
	const str_type::string& fileName,
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
	return LoadTexture(video, out->GetAddress(), nMipMaps, static_cast<unsigned int>(out->GetBufferSize()));
}

bool GLTexture::LoadTexture(
	VideoWeakPtr video,
	const void* pBuffer,
	const unsigned int nMipMaps,
	const unsigned int bufferLength)
{
	int width, height, nrChannels;

	stbi_uc* data = stbi_load_from_memory(
		(stbi_uc*)pBuffer,
		bufferLength,
		&width,
		&height,
		&nrChannels,
		0);

	if (data == NULL)
	{
		ShowMessage("GLTexture couldn't load file " + m_fileName, GSMT_ERROR);
		return false;
	}

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	GLint format;
	switch (nrChannels)
	{
		case 1:
			format = GL_RED;
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ONE);
			break;
		case 2:
			format = GL_RG;
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_GREEN);
			break;
		case 3:
			format = GL_RGB;
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_BLUE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ONE);
			break;
		case 4:
			format = GL_RGBA;
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_BLUE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ALPHA);
			break;
		default:
			stbi_image_free(data);
			glDeleteTextures(1, &m_texture);
			ShowMessage("GLTexture invalid format for file " + m_fileName, GSMT_ERROR);
			return false;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	m_resolution.x = (float)width;
	m_resolution.y = (float)height;

	if (math::Util::IsPowerOfTwo(width) && math::Util::IsPowerOfTwo(height))
	{
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	}
	else
	{
		const unsigned int newWidth  = math::Util::FindNextPowerOfTwoValue(width);
		const unsigned int newHeight = math::Util::FindNextPowerOfTwoValue(height);

		unsigned char* output = new unsigned char [newWidth * newHeight * nrChannels];
		stbir_resize_uint8(data, width, height, 0, output, newWidth, newHeight, 0, nrChannels);

		glTexImage2D(GL_TEXTURE_2D, 0, format, newWidth, newHeight, 0, format, GL_UNSIGNED_BYTE, output);

		delete [] output;
	}

	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
	return true;
}

} // namespace gs2d
