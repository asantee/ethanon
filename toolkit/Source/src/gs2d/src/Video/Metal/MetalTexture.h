#ifndef GS2D_METAL_TEXTURE_H_
#define GS2D_METAL_TEXTURE_H_

#include "../../Video.h"

#include <Metal/Metal.h>

namespace gs2d {

class MetalVideo;

class MetalTexture : public Texture
{
	std::string m_fileName;
	Platform::FileManagerPtr m_fileManager;
	math::Vector2 m_bitmapSize;

	id<MTLTexture> m_texture;

	MetalVideo* m_video;

public:
	MetalTexture(MetalVideo *video, Platform::FileManagerPtr fileManager);
	~MetalTexture();

	id<MTLTexture> GetTexture();
	
	bool LoadTexture(
		const std::string& fileName,
		const unsigned int nMipMaps = 0) override;

	bool LoadTexture(
		const void* pBuffer,
		const unsigned int nMipMaps,
		const unsigned int bufferLength) override;
	
	void Free() override;

	math::Vector2 GetBitmapSize() const override;
	const std::string& GetFileName() const;
};

typedef boost::shared_ptr<MetalTexture> MetalTexturePtr;

} // namespace gs2d

#endif
