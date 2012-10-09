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

#ifndef GS2D_D3D9_VIDEO_INFO_H_
#define GS2D_D3D9_VIDEO_INFO_H_

#include "../../Video.h"
#include "D3D9CgShader.h"

namespace gs2d {

class D3D9VideoInfo
{
	void BuildIn();

	IDirect3DVertexBuffer9* GetVertexBuffer(const Sprite::RECT_MODE mode);
	unsigned int GetPolyCount(const Sprite::RECT_MODE mode);
	D3DPRIMITIVETYPE GetPrimitiveType(const Sprite::RECT_MODE mode);

public:
	static const DWORD SPRITE_FVF;

	struct LINE_VERTEX
	{
		math::Vector3 pos;
		float rhw;
		GS_DWORD color;
	};

	struct VERTEX
	{
		math::Vector3 pos;
		float rhw;
		GS_DWORD color;
		math::Vector2 t0;
	};

	struct SPRITE_VERTEX
	{
		math::Vector3 pos;
		math::Vector2 t0;
	};

	D3D9VideoInfo();
	~D3D9VideoInfo();
	void ReleaseVB();
	bool CreateVB(IDirect3DDevice9 *pDevice);
	bool DrawSprite(IDirect3DDevice9 *pDevice, const Sprite::RECT_MODE mode);
	void BeginFastDraw(IDirect3DDevice9 *pDevice, const Sprite::RECT_MODE mode);
	bool DrawSpriteFast(IDirect3DDevice9 *pDevice, const Sprite::RECT_MODE mode);
	void SetupShaderProfile();

	D3DCAPS9	m_caps;
	HWND		m_hWnd;
	HINSTANCE	m_hInstance;
	MSG			m_msg;
	HCURSOR   m_cursor;
	std::wstring className;
	DWORD m_pow2Tex;
	DWORD m_magAniso;
	DWORD m_minAniso;
	DWORD m_maxAniso;
	DWORD m_scissorSupported;
	unsigned int m_nMaxRTs;
	unsigned int m_nMaxMultiTex;
	bool m_isVSSupported;
	DWORD	m_windowStyle;
	D3DPRESENT_PARAMETERS m_d3dPP;
	bool m_visible;
	bool m_inFocus;
	bool m_deviceLost;
	bool m_quitKeysEnabled;
	bool m_maximized;
	DWORD m_startTime;
	IDirect3DVertexBuffer9 *m_pRect2;
	IDirect3DVertexBuffer9 *m_pRect4;
	IDirect3DVertexBuffer9 *m_pRect3;
	math::Matrix4x4 m_orthoMatrix;
	Shader::SHADER_PROFILE m_lastVertexProfile;
	Shader::SHADER_PROFILE m_lastPixelProfile;
	IDirect3DSurface9 *m_pBackBuffer;
	math::Vector2i v2Resize, v2RestoredSize;
};

/// Converts a GS_BITMAP_FORMAT constant to an extension string
std::wstring GetImageExtension(const Texture::BITMAP_FORMAT fmt);

/// Returns true if the file name has an explicit extension in its string
bool IsTheExtensionRight(const wchar_t* fileName, const wchar_t* extension);

/// Returns a D3DXIMAGE constant for a GS_BITMAP_FORMAT one
D3DXIMAGE_FILEFORMAT GetD3DPF(const Texture::BITMAP_FORMAT fmt);

typedef boost::shared_ptr<D3D9VideoInfo> D3D9VideoInfoPtr;

} // namespace gs2d

#endif
