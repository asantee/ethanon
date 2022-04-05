#include "MetalTexture.h"

#include "../../Platform/Platform.h"

#include "MetalVideo.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcomma"
#define STB_IMAGE_IMPLEMENTATION
#include "../../../vendors/stb/stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "../../../vendors/stb/stb_image_resize.h"
#pragma clang diagnostic pop

#include <iostream>

namespace gs2d {

MetalTexture::MetalTexture(MetalVideo *video, Platform::FileManagerPtr fileManager) :
	m_video(video),
	m_fileManager(fileManager)
{
}

MetalTexture::~MetalTexture()
{
	Free();
}

void MetalTexture::Free()
{
	m_texture = nil;
}

bool MetalTexture::LoadTexture(
	const std::string& fileName,
	const unsigned int nMipMaps)
{
	m_fileName = fileName;

	Platform::FileBuffer fileBuffer;
	m_fileManager->GetFileBuffer(m_fileName, fileBuffer);
	if (!fileBuffer)
	{
		std::cerr << m_fileName << " could not load buffer" << std::endl;
		return false;
	}

	return LoadTexture(
		fileBuffer->GetAddress(),
		nMipMaps,
		static_cast<unsigned int>(fileBuffer->GetBufferSize()));
}

bool MetalTexture::LoadTexture(
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

	if (data == NULL || width == 0 || height == 0)
	{
		ShowMessage("MetalTexture couldn't load buffer " + m_fileName, GSMT_ERROR);
		return false;
	}

	m_bitmapSize.x = (float)width;
	m_bitmapSize.y = (float)height;

	const uint32_t destDataSize = width * height * 4;
	uint8_t* destData = new uint8_t [destDataSize];

	for (uint32_t y = 0; y < height; y++)
	{
		for (uint32_t x = 0; x < width; x++)
		{
			const uint32_t srcPixelIdx  = nrChannels * (y * width + x);
			const uint32_t destPixelIdx = 4 * (y * width + x);

			if (nrChannels == 4)
			{
				destData[destPixelIdx + 0] = data[srcPixelIdx + 2];
				destData[destPixelIdx + 1] = data[srcPixelIdx + 1];
				destData[destPixelIdx + 2] = data[srcPixelIdx + 0];
				destData[destPixelIdx + 3] = data[srcPixelIdx + 3];
			}
			else if (nrChannels == 3)
			{
				destData[destPixelIdx + 0] = data[srcPixelIdx + 2];
				destData[destPixelIdx + 1] = data[srcPixelIdx + 1];
				destData[destPixelIdx + 2] = data[srcPixelIdx + 0];
				destData[destPixelIdx + 3] = 255;
			}
			else if (nrChannels == 2)
			{
				destData[destPixelIdx + 0] = data[srcPixelIdx + 0];
				destData[destPixelIdx + 1] = data[srcPixelIdx + 0];
				destData[destPixelIdx + 2] = data[srcPixelIdx + 0];
				destData[destPixelIdx + 3] = data[srcPixelIdx + 1];
			}
			else if (nrChannels == 1)
			{
				destData[destPixelIdx + 0] = data[srcPixelIdx + 0];
				destData[destPixelIdx + 1] = data[srcPixelIdx + 0];
				destData[destPixelIdx + 2] = data[srcPixelIdx + 0];
				destData[destPixelIdx + 3] = 255;
			}
		}
	}

	MTLTextureDescriptor *textureDescriptor = [[MTLTextureDescriptor alloc] init];

	textureDescriptor.pixelFormat = MTLPixelFormatBGRA8Unorm;

	textureDescriptor.width = width;
	textureDescriptor.height = height;

	m_texture = [m_video->GetDevice() newTextureWithDescriptor:textureDescriptor];

	const MTLRegion region =
	{
		{ 0, 0, 0 },
		{ (NSUInteger)width, (NSUInteger)height, 1 }
	};

	const NSUInteger bytesPerRow = 4 * (NSUInteger)width;
	[m_texture replaceRegion:region mipmapLevel:0 withBytes:destData bytesPerRow:bytesPerRow];

	stbi_image_free(data);
	delete [] destData;

	return true;
}

math::Vector2 MetalTexture::GetBitmapSize() const
{
	return m_bitmapSize;
}

const std::string& MetalTexture::GetFileName() const
{
	return m_fileName;
}

id<MTLTexture> MetalTexture::GetTexture()
{
	return m_texture;
}

} // namespace gs2d
