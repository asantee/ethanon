#ifndef GS2D_TEXTURE_H_
#define GS2D_TEXTURE_H_

#include "Math/Vector2.h"

#include <boost/shared_ptr.hpp>
#include <string>

namespace gs2d {

class Video;
typedef boost::weak_ptr<Video> VideoWeakPtr;

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

	virtual math::Vector2 GetBitmapSize() const = 0;

	virtual bool LoadTexture(
		VideoWeakPtr video,
		const std::string& fileName,
		const unsigned int nMipMaps = 0) = 0;

	virtual bool LoadTexture(
		VideoWeakPtr video,
		const void* pBuffer,
		const unsigned int nMipMaps,
		const unsigned int bufferLength) = 0;
	
	virtual void Free() = 0;
};

typedef boost::shared_ptr<Texture> TexturePtr;

} // namespace gs2d

#endif
