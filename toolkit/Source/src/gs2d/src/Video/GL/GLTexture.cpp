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

#ifdef WEBP
#include <webp/decode.h>
#endif
#include<iostream>

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
	const std::string& fileName,
	const unsigned int nMipMaps)
{
	m_fileName = fileName;

	Platform::FileBuffer out;
	// try loading from WebP
#ifdef WEBP
	std::string webPFileName = Platform::RemoveExtension(fileName) + ".webp";

	if(m_fileManager->FileExists(webPFileName))
	{
		//std::cout << "Loading texture: " << webPFileName << std::endl;
		m_fileManager->GetFileBuffer(webPFileName, out);
	}
	// if not, try original image with STB
	else
	{
		//std::cout << "Loading texture: " << fileName << std::endl;
		m_fileManager->GetFileBuffer(fileName, out);
	}
#else
	m_fileManager->GetFileBuffer(fileName, out);
#endif
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
	int width, height;
	int nrChannels = 4;
	uint8_t* data = NULL;
	bool isWebP = false;
	/* Try to load webp using libwebp
	*/
#ifdef WEBP
	if (isWebP = WebPGetInfo((uint8_t*)pBuffer, bufferLength, &width, &height))
	{
		data = WebPDecodeRGBA((uint8_t*)pBuffer, bufferLength, &width, &height);
	}
#endif
	/* Try to load png using STB_image
	*/
	if (data == NULL)
	{
		data = (stbi_uc*)stbi_load_from_memory(
			(stbi_uc*)pBuffer,
			bufferLength,
			&width,
			&height,
			&nrChannels,
			0);
	}

	if (data == NULL)
	{
		ShowMessage("GLTexture couldn't load file " + m_fileName, GSMT_ERROR);
		return false;
	}

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	GLint format = GL_RGBA;
	GLint swizzleMask[] = { GL_RED, GL_RED, GL_RED, GL_GREEN };
	switch (nrChannels)
	{
		case 1:
			format = GL_RED;
			swizzleMask[3] = GL_ONE;
			glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
			break;
		case 2:
			format = GL_RG;
			glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
			break;
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
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

	if(isWebP)
	{
#ifdef WEBP
		WebPFree(data);
#endif
	}
	else 
	{
		stbi_image_free(data);
	}
	return true;
}

} // namespace gs2d
