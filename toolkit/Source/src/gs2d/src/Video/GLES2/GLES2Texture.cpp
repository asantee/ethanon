#include "GLES2Texture.h"

#include "../../Platform/Platform.h"

#include "GLES2Video.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcomma"
#define STB_IMAGE_IMPLEMENTATION
#include "../../../vendors/stb/stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "../../../vendors/stb/stb_image_resize.h"
#pragma clang diagnostic pop

#include <iostream>

namespace gs2d {

const str_type::string GLES2Texture::TEXTURE_LOG_FILE("GLES2Texture.log.txt");
const str_type::string GLES2Texture::ETC1_FILE_FORMAT("pkm");
const str_type::string GLES2Texture::PVR_FILE_FORMAT("pvr");
	
void CheckFrameBufferStatus(const GLuint fbo, const GLuint tex, const bool showSuccessMessage)
{
	const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	std::cerr << GS_L("fboID ") << fbo << GS_L(" (") << tex << GS_L("): ");
	switch (status)
	{
	case GL_FRAMEBUFFER_COMPLETE:
		if (showSuccessMessage)
		{
			std::cout << GS_L(" render target texture created successfully");
		}
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		std::cout << GS_L(" incomplete attachment");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
		std::cout << GS_L(" incomplete dimensions");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		std::cout << GS_L(" incomplete missing attachment");
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED:
		std::cout << GS_L(" unsupported");
		break;
	default:
		std::cout << GS_L(" unknown status");
	}
	std::cout << std::endl;
}

GLES2Texture::GLES2Texture(VideoWeakPtr video, const str_type::string& fileName, Platform::FileManagerPtr fileManager) :
		m_fileManager(fileManager),
		m_fileName(fileName),
		m_texture(0)
{
}

GLES2Texture::~GLES2Texture()
{
	Free();
}

void GLES2Texture::Free()
{
	if (m_texture != 0)
	{
		glDeleteTextures(1, &m_texture);
		m_texture = 0;
		std::cout << m_fileName << ": texture deleted ID " << m_texture << std::endl;
	}
}

bool GLES2Texture::LoadTexture(
	VideoWeakPtr video,
	const str_type::string& fileName,
	const unsigned int nMipMaps)
{
	m_fileName = fileName;
	const COMPRESSION_FORMAT format = FindCompressionFormat(m_fileName);

	Platform::FileBuffer out;
	m_fileManager->GetFileBuffer(m_fileName, out);
	if (!out)
	{
		std::cerr << m_fileName << " could not load buffer" << std::endl;
		return false;
	}
	return LoadTexture(
		video,
		out->GetAddress(),
		nMipMaps,
		static_cast<unsigned int>(out->GetBufferSize()),
		format);
}

bool GLES2Texture::LoadTexture(
	VideoWeakPtr video,
	const void* pBuffer,
	const unsigned int nMipMaps,
	const unsigned int bufferLength,
	const COMPRESSION_FORMAT format)
{
	if (format == NO_COMPRESSION)
	{
		return LoadTexture(video, pBuffer, nMipMaps, bufferLength);
	}
	else
	{
		if (format == PVRTC)
			return LoadPVRTexture(video, pBuffer, nMipMaps, bufferLength);
		else
			return LoadETC1Texture(video, pBuffer, nMipMaps, bufferLength);
	}
}

GLES2Texture::COMPRESSION_FORMAT GLES2Texture::FindCompressionFormat(str_type::string& fileName)
{
	if (MayUsePVRCompressedVersion(fileName))
		return PVRTC;
	else if (MayUseETC1CompressedVersion(fileName))
		return ETC1;
	else
		return NO_COMPRESSION;
}

bool GLES2Texture::CheckTextureVersion(
	str_type::string& fileName,
	const str_type::string& format,
	Platform::FileManagerPtr fileManager)
{
	const str_type::string alternativeFileName = Platform::RemoveExtension(fileName.c_str()).append(".").append(format);
	const bool versionExists = (fileManager->FileExists(alternativeFileName));
	if (versionExists)
		fileName = alternativeFileName;
	return versionExists;
}

bool GLES2Texture::MayUseETC1CompressedVersion(str_type::string& fileName)
{
	// not supported on iOS
	if (Application::GetPlatformName() == "ios")
		return false;

	const bool etc1VersionExists = CheckTextureVersion(fileName, ETC1_FILE_FORMAT, m_fileManager);
	return etc1VersionExists;
}

bool GLES2Texture::MayUsePVRCompressedVersion(str_type::string& fileName)
{
	const GLubyte* extensions = glGetString(GL_EXTENSIONS);
	if (strstr((char*)extensions, "GL_IMG_texture_compression_pvrtc") == 0)
		return false;

	const bool pvrVersionExists = CheckTextureVersion(fileName, PVR_FILE_FORMAT, m_fileManager);
	return pvrVersionExists;
}

bool GLES2Texture::LoadTexture(
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
		case 1: format = GL_LUMINANCE; break;
		case 2: format = GL_LUMINANCE_ALPHA; break;
		case 3: format = GL_RGB; break;
		case 4: format = GL_RGBA; break;
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

	std::cout << m_fileName << " texture loaded" << std::endl;

	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
	return true;
}

bool GLES2Texture::LoadETC1Texture(
	VideoWeakPtr video,
	const void* pBuffer,
	const unsigned int nMipMaps,
	const unsigned int bufferLength)
{
	const ETC1Header* header = (ETC1Header*)pBuffer;
	if (strstr(header->tag, "PKM") == 0)
	{
		std::cerr << m_fileName << " invalid header" << std::endl;
		return false;
	}

	const short width  = Platform::ShortEndianSwap(header->texWidth);
	const short height = Platform::ShortEndianSwap(header->texHeight);
	m_resolution.x = static_cast<float>(width);
	m_resolution.y = static_cast<float>(height);
	//m_profile.originalWidth = static_cast<unsigned int>(Platform::ShortEndianSwap(header->origWidth));
	//m_profile.originalHeight = static_cast<unsigned int>(Platform::ShortEndianSwap(header->origHeight));
	std::cout << m_fileName << " texture loaded" ;

	const std::size_t size = (width / 4) * (height / 4) * 8;

	// not supported on iOS, so we'll only use the enum on non-ios devices
	GLenum internalformat = 0;
	#ifndef APPLE_IOS
		internalformat = GL_ETC1_RGB8_OES;
	#endif

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	unsigned char *data = (unsigned char*)pBuffer;
	glCompressedTexImage2D(
		GL_TEXTURE_2D,
		0,
		internalformat,
		width,
		height,
		0,
		static_cast<GLsizei>(size),
		data + sizeof(ETC1Header));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}

static GLenum FindPVRTCFormatFromPVRHeaderData(const uint32_t* pixelFormat)
{
	const uint32_t format = (pixelFormat[0] != 0x0) ? pixelFormat[0] : pixelFormat[1];
	switch (format)
	{
	case 0: // PVRTC 2bpp RGB
		return GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
	case 1: // PVRTC 2bpp RGBA
		return GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
	case 2: // PVRTC 4bpp RGB
		return GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
	case 3: // PVRTC 4bpp RGBA
	default:
		return GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
	};
}

bool GLES2Texture::LoadPVRTexture(
	VideoWeakPtr video,
	const void* pBuffer,
	const unsigned int nMipMaps,
	const unsigned int bufferLength)
{
	const PVRHeader* header = (PVRHeader*)pBuffer;
	const uint32_t littleEndianVersionId = 0x03525650;
	const uint32_t bigEndianVersionId    = 0x50565203;
	if (header->version != littleEndianVersionId && header->version != bigEndianVersionId)
	{
		std::cerr << m_fileName << " invalid header" << std::endl;
		return false;
	}

	m_resolution.x = static_cast<float>(header->width);
	m_resolution.y = static_cast<float>(header->height);
	//m_profile.originalWidth = static_cast<unsigned int>(m_profile.width);
	//m_profile.originalHeight = static_cast<unsigned int>(m_profile.height);
	std::cout << m_fileName + " texture loaded" << std::endl;

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	unsigned char *data = (unsigned char*)pBuffer;
	data += sizeof(PVRHeader);
	data += header->metaDataSize;

	const std::size_t size = bufferLength - (sizeof(PVRHeader) + header->metaDataSize);

	const GLenum format = FindPVRTCFormatFromPVRHeaderData(header->pixelFormat);
	glCompressedTexImage2D(
		GL_TEXTURE_2D,
		0,
		format,
		header->width,
		header->height,
		0,
		static_cast<GLsizei>(size),
		data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}

math::Vector2 GLES2Texture::GetBitmapSize() const
{
	return m_resolution;
}

const str_type::string& GLES2Texture::GetFileName() const
{
	return m_fileName;
}

GLuint GLES2Texture::GetTexture() const
{
	return m_texture;
}

} // namespace gs2d
