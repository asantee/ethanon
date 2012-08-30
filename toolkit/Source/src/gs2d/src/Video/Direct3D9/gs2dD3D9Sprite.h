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

#ifndef GS2D_D3D9_SPRITE_H_
#define GS2D_D3D9_SPRITE_H_

#include "gs2dD3D9.h"
#include <boost/shared_array.hpp>

namespace gs2d {

/**
 * \brief Implements a Direct3D 9 Sprite object
 */
class D3D9Sprite : public Sprite
{
	bool LoadSprite(VideoWeakPtr video, GS_BYTE *pBuffer, const unsigned int bufferLength,
						GS_COLOR mask = GS_ZERO, const unsigned int width = 0, const unsigned int height = 0);
	bool LoadSprite(VideoWeakPtr video, const std::wstring& fileName, GS_COLOR mask = GS_ZERO,
					const unsigned int width = 0, const unsigned int height = 0);
	bool CreateRenderTarget(VideoWeakPtr video,
							const unsigned int width, const unsigned int height,
							const GS_TARGET_FORMAT format = GSTF_DEFAULT);

	VideoWeakPtr m_video;
	TexturePtr m_texture;
	math::Vector2 m_scroll;
	math::Vector2 m_multiply;
	math::Vector2 m_origin;
	math::Vector2 m_size;
	GS_TARGET_FORMAT m_targetFormat;
	unsigned int  m_currentRect;
	boost::shared_array<math::Rect2Df> m_rects;
	math::Rect2Df m_rect;
	unsigned int m_nColumns, m_nRows;
	bool m_flipX, m_flipY;
	GS_RECT_MODE m_rectMode;
	Sprite::TYPE m_type;
	IDirect3DTexture9 *m_pBackupTexture;
	float m_densityValue;

	IDirect3DTexture9 *m_pTexture;
	IDirect3DDevice9 *m_pDevice;
	D3D9VideoInfoPtr m_pVideoInfo;

	void Init();
	bool GetInternalData();
	void OnLostDevice();
	void RecoverFromBackup();

public:
	D3D9Sprite();
	~D3D9Sprite();

	bool Draw(const math::Vector2 &v2Pos,
				const GS_COLOR color = GS_WHITE,
				const float angle = 0.0f,
				const math::Vector2 &v2Scale = math::Vector2(1.0f,1.0f));
	bool DrawShaped(const math::Vector2 &v2Pos, const math::Vector2 &v2Size,
					  const GS_COLOR color0, const GS_COLOR color1,
					  const GS_COLOR color2, const GS_COLOR color3,
					  const float angle = 0.0f);
	bool Stretch(const math::Vector2 &a, const math::Vector2 &b, const float width,
				   const GS_COLOR color0 = GS_WHITE, const GS_COLOR color1 = GS_WHITE);
	bool SaveBitmap(const wchar_t *wcsName, const GS_BITMAP_FORMAT fmt, math::Rect2D *pRect = 0);

	bool DrawShapedFast(const math::Vector2 &v2Pos, const math::Vector2 &v2Size, const GS_COLOR color);
	bool DrawOptimal(const math::Vector2 &v2Pos, const GS_COLOR color = GS_WHITE, const float angle = 0.0f,
					 const math::Vector2 &v2Size = math::Vector2(-1,-1));

	void BeginFastRendering();
	void EndFastRendering();

	TextureWeakPtr GetTexture();
	void SetOrigin(const GS_ENTITY_ORIGIN origin);
	void SetOrigin(const math::Vector2 &v2Custom);
	math::Vector2 GetOrigin() const;

	bool SetupSpriteRects(const unsigned int columns, const unsigned int rows);
	bool SetRect(const unsigned int column, const unsigned int row);
	bool SetRect(const unsigned int rect);
	void SetRect(const math::Rect2Df &rect);
	void UnsetRect();
	unsigned int GetNumRects() const;
	math::Rect2Df GetRect() const;
	math::Rect2Df GetRect(const unsigned int rect) const;
	unsigned int GetRectIndex() const;

	Texture::PROFILE GetProfile() const;
	math::Vector2i GetBitmapSize() const;
	math::Vector2 GetBitmapSizeF() const;

	unsigned int GetNumRows() const;
	unsigned int GetNumColumns() const;

	void FlipX(const bool flip);
	void FlipY(const bool flip);
	void FlipX();
	void FlipY();
	bool GetFlipX() const;
	bool GetFlipY() const;

	void SetScroll(const math::Vector2 &v2Scroll);
	math::Vector2 GetScroll() const;

	void SetMultiply(const math::Vector2 &v2Multiply);
	math::Vector2 GetMultiply() const;

	TYPE GetType() const;
	boost::any GetTextureObject();

	void GenerateBackup();
	bool SetAsTexture(const unsigned int passIdx);
	void SetRectMode(const GS_RECT_MODE mode);
	GS_RECT_MODE GetRectMode() const;

	void SetSpriteDensityValue(const float value);
	float GetSpriteDensityValue() const;

	math::Vector2 GetFrameSize() const;

	math::Vector2 GetTextureSize() const;
};

typedef boost::shared_ptr<D3D9Sprite> D3D9SpritePtr;
typedef boost::weak_ptr<D3D9Sprite> D3D9SpriteWeakPtr;

} // namespace gs2d

#endif