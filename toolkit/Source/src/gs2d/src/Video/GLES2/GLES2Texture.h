#ifndef GS2D_GLES2_TEXTURE_H_
#define GS2D_GLES2_TEXTURE_H_

#include "../../Video.h"

#include "GLES2Include.h"

namespace gs2d {

void CheckFrameBufferStatus(const GLuint fbo, const GLuint tex, const bool showSuccessMessage);

class GLES2Texture : public Texture
{
	enum COMPRESSION_FORMAT
	{
		NO_COMPRESSION = 0,
		ETC1 = 1,
		PVRTC = 2
	};

	static bool CheckTextureVersion(
		str_type::string& fileName,
		const str_type::string& format,
		Platform::FileManagerPtr fileManager);

	COMPRESSION_FORMAT FindCompressionFormat(str_type::string& fileName);
	bool MayUseETC1CompressedVersion(str_type::string& fileName);
	bool MayUsePVRCompressedVersion(str_type::string& fileName);

	bool LoadTexture(
		VideoWeakPtr video,
		const void* pBuffer,
		const unsigned int nMipMaps,
		const unsigned int bufferLength,
		const COMPRESSION_FORMAT format);

	bool LoadETC1Texture(
		VideoWeakPtr video,
		const void* pBuffer,
		const unsigned int nMipMaps,
		const unsigned int bufferLength);

	bool LoadPVRTexture(
		VideoWeakPtr video,
		const void* pBuffer,
		const unsigned int nMipMaps,
		const unsigned int bufferLength);

	GLuint m_texture;

	str_type::string m_fileName;
	Platform::FileManagerPtr m_fileManager;
	math::Vector2 m_resolution;

public:
	GLES2Texture(VideoWeakPtr video, const str_type::string& fileName, Platform::FileManagerPtr fileManager);
	~GLES2Texture();

	GLuint GetTexture() const;

	bool LoadTexture(
		VideoWeakPtr video,
		const str_type::string& fileName,
		const unsigned int nMipMaps = 0) override;

	bool LoadTexture(
		VideoWeakPtr video,
		const void* pBuffer,
		const unsigned int nMipMaps,
		const unsigned int bufferLength) override;
	
	void Free() override;

	math::Vector2 GetBitmapSize() const override;
	const str_type::string& GetFileName() const;

	static const str_type::string TEXTURE_LOG_FILE;
	static const str_type::string ETC1_FILE_FORMAT;
	static const str_type::string PVR_FILE_FORMAT;

	// ETC1 compressed pkm texture file header
	struct ETC1Header
	{
		char tag[6];        // "PKM 10"
		short format;         // Format == number of mips (== zero)
		short texWidth;       // Texture dimensions, multiple of 4 (big-endian)
		short texHeight;
		short origWidth;      // Original dimensions (big-endian)
		short origHeight;
	};

	// PowerVR compressed pvr texture file header
	struct PVRHeader
	{
		uint32_t version;
		uint32_t flags;
		uint32_t pixelFormat[2];
		uint32_t colourSpace;
		uint32_t channelType;
		uint32_t height;
		uint32_t width;
		uint32_t depth;
		uint32_t numSurfaces;
		uint32_t numFaces;
		uint32_t mipMapCount;
		uint32_t metaDataSize;
	};
};

typedef boost::shared_ptr<GLES2Texture> GLES2TexturePtr;

} // namespace gs2d

#endif
