#ifndef GS2D_TEXTURE_H_
#define GS2D_TEXTURE_H_

#include "Types.h"

#include "Math/GameMath.h"

#include "Math/Color.h"

#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>

namespace gs2d {

/**
 * \brief Stores a texture buffer to be used as source or target
 */
class Texture
{
public:
	enum PIXEL_FORMAT
	{
		PF_16BIT = 0,
		PF_32BIT = 1,
		PF_24BIT = 0,
		PF_UNKNOWN = 2,
		PF_DEFAULT = 3,
	};

	enum BITMAP_FORMAT
	{
		BF_BMP = 0,
		BF_JPG = 1,
		BF_PNG,
		BF_TGA,
		BF_DDS,
		BF_HDR,
	};

	struct PROFILE
	{
		PROFILE()
		{
			width = height = nMipMaps = originalWidth = originalHeight = 0;
		}
		unsigned int width, height, nMipMaps,
			originalWidth, originalHeight;
	};

	static const unsigned int FULL_MIPMAP_CHAIN = 0x00FFFFFF;

	virtual PROFILE GetProfile() const = 0;
	virtual boost::any GetTextureObject() = 0;
	virtual math::Vector2 GetBitmapSize() const = 0;

	virtual bool LoadTexture(
		VideoWeakPtr video,
		const str_type::string& fileName,
		const unsigned int width = 0,
		const unsigned int height = 0,
		const unsigned int nMipMaps = 0) = 0;

	virtual bool LoadTexture(
		VideoWeakPtr video,
		const void* pBuffer,
		const unsigned int width,
		const unsigned int height,
		const unsigned int nMipMaps,
		const unsigned int bufferLength) = 0;
};

} // namespace gs2d

#endif
