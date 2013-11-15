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

#ifndef GS2D_TEXTURE_H_
#define GS2D_TEXTURE_H_

#include "Math/GameMath.h"

namespace gs2d {

/**
 * \brief Stores a texture buffer to be used as source or target
 */
class Texture
{
public:
	enum TARGET_FORMAT
	{
		TF_DEFAULT = 0,
		TF_ARGB = 1,
		TF_NONE = 2
	};

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
			mask = 0xFF000000;
			width = height = nMipMaps = originalWidth = originalHeight = 0;
		}
		Color mask;
		unsigned int width, height, nMipMaps,
			originalWidth, originalHeight;
	};

	enum TYPE
	{
		TT_STATIC = 0,
		TT_RENDER_TARGET = 1,
		TT_NONE = 2,
	};

	static const unsigned int FULL_MIPMAP_CHAIN = 0x00FFFFFF;

    virtual bool IsAllBlack() const = 0;

	virtual bool SetTexture(const unsigned int passIdx = 0) = 0;
	virtual PROFILE GetProfile() const = 0;
	virtual TYPE GetTextureType() const = 0;
	virtual boost::any GetTextureObject() = 0;
	virtual math::Vector2 GetBitmapSize() const = 0;

	virtual bool CreateRenderTarget(
		VideoWeakPtr video,
		const unsigned int width,
		const unsigned int height,const TARGET_FORMAT fmt) = 0;

	virtual bool LoadTexture(
		VideoWeakPtr video,
		const str_type::string& fileName,
		Color mask,
		const unsigned int width = 0,
		const unsigned int height = 0,
		const unsigned int nMipMaps = 0) = 0;

	virtual bool LoadTexture(
		VideoWeakPtr video,
		const void* pBuffer,
		Color mask,
		const unsigned int width,
		const unsigned int height,
		const unsigned int nMipMaps,
		const unsigned int bufferLength) = 0;
};

} // namespace gs2d

#endif
