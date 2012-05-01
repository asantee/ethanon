/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2012 Andre Santee
 http://www.asantee.net/ethanon/

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

#ifndef GS2D_D3D9_TEXTURE_H_
#define GS2D_D3D9_TEXTURE_H_

#include "../../gs2d.h"
#include <d3dx9.h>

namespace gs2d {

/**
 * \brief Implements a Direct3D 9 Texture object
 */
class D3D9Texture : public Texture
{
	IDirect3DTexture9 *m_pTexture;
	IDirect3DDevice9 *m_pDevice;
	VideoWeakPtr m_video;
	PROFILE m_profile;
	TYPE  m_texType;

	bool GetInternalData();

public:
	D3D9Texture();
	~D3D9Texture();

	bool SetTexture(const unsigned int passIdx = 0);
	PROFILE GetProfile() const;
	TYPE GetTextureType() const;
	boost::any GetTextureObject();
	math::Vector2 GetBitmapSize() const;

protected:
	bool CreateRenderTarget(
		VideoWeakPtr video, const unsigned int width, const unsigned int height, const GS_TARGET_FORMAT fmt
	);
	bool LoadTexture(
		VideoWeakPtr video,
		const std::wstring& fileName, GS_COLOR mask,
			const unsigned int width = 0, const unsigned int height = 0,
			const unsigned int nMipMaps = 0
	);
	virtual bool LoadTexture(
		VideoWeakPtr video,
		const void * pBuffer, GS_COLOR mask,
			const unsigned int width, const unsigned int height,
			const unsigned int nMipMaps,
			const unsigned int bufferLength
	);

};

} // namespace gs2d

#endif