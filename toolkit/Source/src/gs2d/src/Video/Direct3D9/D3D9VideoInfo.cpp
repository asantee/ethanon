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

#include "D3D9VideoInfo.h"

namespace gs2d {

const DWORD D3D9VideoInfo::SPRITE_FVF = (D3DFVF_XYZ|D3DFVF_TEX1);

D3DXIMAGE_FILEFORMAT GetD3DPF(const Texture::BITMAP_FORMAT fmt)
{
	switch (fmt)
	{
	case Texture::BF_BMP:
		return D3DXIFF_BMP;
		break;
	case Texture::BF_JPG:
		return D3DXIFF_JPG;
		break;
	case Texture::BF_PNG:
		return D3DXIFF_PNG;
		break;
	case Texture::BF_TGA:
		return D3DXIFF_TGA;
		break;
	case Texture::BF_DDS:
		return D3DXIFF_DDS;
		break;
	case Texture::BF_HDR:
		return D3DXIFF_HDR;
		break;
	default:
		return D3DXIFF_BMP;
		break;
	}
}

std::wstring GetImageExtension(const Texture::BITMAP_FORMAT fmt)
{
	switch (fmt)
	{
	case Texture::BF_JPG:
		return L".jpg";
		break;
	case Texture::BF_PNG:
		return L".png";
		break;
	case Texture::BF_TGA:
		return L".tga";
		break;
	case Texture::BF_DDS:
		return L".dds";
		break;
	case Texture::BF_HDR:
		return L".hdr";
		break;
	case Texture::BF_BMP:
	default:
		return L".bmp";
		break;
	}
}

bool IsTheExtensionRight(const wchar_t *fileName, const wchar_t *extension)
{
	const unsigned int fileLen = wcslen(fileName);
	const unsigned int extLen = wcslen(extension);
	const unsigned int diff = fileLen-extLen+1;
	for (unsigned int t=0; t<diff; t++)
	{
		if (wcscmp(&fileName[t], extension) == 0)
		{
			if (fileName[t+extLen] == L'\0')
				return true;
		}
	}
	return false;
}

void D3D9VideoInfo::BuildIn()
{
	ZeroMemory(&m_msg, sizeof(MSG));
	className.clear();
	m_pow2Tex = m_scissorSupported = 0x00;
	m_inFocus = m_visible = true;
	m_deviceLost = false;
	m_startTime = 0x0;
	m_quitKeysEnabled = true;
	m_pRect2 = m_pRect4 = m_pRect3 = NULL;
	m_lastVertexProfile = m_lastPixelProfile = Shader::SP_NONE;
	m_pBackBuffer = NULL;
	m_nMaxRTs = 1;
	m_nMaxMultiTex = 0;
	v2Resize = v2RestoredSize = math::Vector2i(0,0);
	m_maximized = false;
}

D3D9VideoInfo::D3D9VideoInfo()
{
	BuildIn();
}

D3D9VideoInfo::~D3D9VideoInfo()
{
	ReleaseVB();
}

void D3D9VideoInfo::ReleaseVB()
{
	if (m_pRect2)
	{
		m_pRect2->Release();
		m_pRect2 = NULL;
	}
	if (m_pRect4)
	{
		m_pRect4->Release();
		m_pRect4 = NULL;
	}
	if (m_pRect3)
	{
		m_pRect3->Release();
		m_pRect3 = NULL;
	}
}

bool D3D9VideoInfo::CreateVB(IDirect3DDevice9* pDevice)
{
	ReleaseVB();
	// Create the vertex buffer for the 2-triangled rect
	const SPRITE_VERTEX vertices2[] =
	{
		{  math::Vector3(0.0f, 1.0f, 0.0f), math::Vector2(0, 1) },
		{  math::Vector3(0.0f, 0.0f, 0.0f), math::Vector2(0, 0) },
		{  math::Vector3(1.0f, 0.0f, 0.0f), math::Vector2(1, 0) },
		{  math::Vector3(1.0f, 1.0f, 0.0f), math::Vector2(1, 1) },
	};

	if (FAILED(pDevice->CreateVertexBuffer(sizeof(vertices2),
		D3DUSAGE_WRITEONLY, SPRITE_FVF,
		D3DPOOL_MANAGED, &m_pRect2, NULL)))
	{
		return false;
	}

	void* pVertices;
	if (FAILED(m_pRect2->Lock(0, 0, &pVertices, 0)))
	{
		return false;
	}
	memcpy(pVertices, vertices2, sizeof(vertices2));
	m_pRect2->Unlock();

	// Create the vertex buffer for the 4-triangled rect
	const SPRITE_VERTEX vertices4[] =
	{
		{  math::Vector3(0.5f, 0.5f, 0.0f), math::Vector2(0.5f, 0.5f) },
		{  math::Vector3(0.0f, 1.0f, 0.0f), math::Vector2(0, 1) },
		{  math::Vector3(0.0f, 0.0f, 0.0f), math::Vector2(0, 0) },
		{  math::Vector3(1.0f, 0.0f, 0.0f), math::Vector2(1, 0) },
		{  math::Vector3(1.0f, 1.0f, 0.0f), math::Vector2(1, 1) },
		{  math::Vector3(0.0f, 1.0f, 0.0f), math::Vector2(0, 1) },
	};
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(vertices4),
		D3DUSAGE_WRITEONLY, SPRITE_FVF,
		D3DPOOL_MANAGED, &m_pRect4, NULL)))
	{
		return false;
	}
	if (FAILED(m_pRect4->Lock(0, 0, &pVertices, 0)))
	{
		return false;
	}
	memcpy(pVertices, vertices4, sizeof(vertices4));
	m_pRect4->Unlock();

	// Create the vertex buffer for the 3-triangled rect
	const SPRITE_VERTEX vertices3[] =
	{
		{  math::Vector3(0.0f, 0.0f, 0.0f), math::Vector2(0, 0) },
		{  math::Vector3(0.0f, 1.0f, 0.0f), math::Vector2(0, 1) },
		{  math::Vector3(0.5f, 0.0f, 0.0f), math::Vector2(0.5f, 0) },
		{  math::Vector3(1.0f, 1.0f, 0.0f), math::Vector2(1, 1) },
		{  math::Vector3(1.0f, 0.0f, 0.0f), math::Vector2(1, 0) },
	};
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(vertices3),
		D3DUSAGE_WRITEONLY, SPRITE_FVF,
		D3DPOOL_MANAGED, &m_pRect3, NULL)))
	{
		return false;
	}
	if (FAILED(m_pRect3->Lock(0, 0, &pVertices, 0)))
	{
		return false;
	}
	memcpy(pVertices, vertices3, sizeof(vertices3));
	m_pRect3->Unlock();

	return true;
}

IDirect3DVertexBuffer9* D3D9VideoInfo::GetVertexBuffer(const Sprite::RECT_MODE mode)
{
	switch (mode)
	{
	case Sprite::RM_FOUR_TRIANGLES:
		return m_pRect4;
	case Sprite::RM_THREE_TRIANGLES:
		return m_pRect3;
	case Sprite::RM_TWO_TRIANGLES:
	default:
		return m_pRect2;
	}
}

unsigned int D3D9VideoInfo::GetPolyCount(const Sprite::RECT_MODE mode)
{
	switch (mode)
	{
	case Sprite::RM_FOUR_TRIANGLES:
		return 4;
	case Sprite::RM_THREE_TRIANGLES:
		return 3;
	case Sprite::RM_TWO_TRIANGLES:
	default:
		return 2;
	}
}

D3DPRIMITIVETYPE D3D9VideoInfo::GetPrimitiveType(const Sprite::RECT_MODE mode)
{
	switch (mode)
	{
	case Sprite::RM_FOUR_TRIANGLES:
		return D3DPT_TRIANGLEFAN;
		break;
	case Sprite::RM_THREE_TRIANGLES:
		return D3DPT_TRIANGLESTRIP;
	case Sprite::RM_TWO_TRIANGLES:
	default:
		return D3DPT_TRIANGLEFAN;
	}
}

bool D3D9VideoInfo::DrawSprite(IDirect3DDevice9 *pDevice, const Sprite::RECT_MODE mode)
{
	IDirect3DVertexBuffer9* pSprite = GetVertexBuffer(mode);
	unsigned int polyCount = GetPolyCount(mode);
	D3DPRIMITIVETYPE primType = GetPrimitiveType(mode);

	if (FAILED(pDevice->SetStreamSource(0, pSprite, 0, sizeof(SPRITE_VERTEX))))
		return false;
	pDevice->SetFVF(SPRITE_FVF);
	if (FAILED(pDevice->DrawPrimitive(primType, 0, polyCount)))
		return false;
	return true;
}

void D3D9VideoInfo::BeginFastDraw(IDirect3DDevice9* pDevice, const Sprite::RECT_MODE mode)
{
	IDirect3DVertexBuffer9 *pSprite = GetVertexBuffer(mode);
	if (FAILED(pDevice->SetStreamSource(0, pSprite, 0, sizeof(SPRITE_VERTEX))))
		return;
	pDevice->SetFVF(SPRITE_FVF);
}

bool D3D9VideoInfo::DrawSpriteFast(IDirect3DDevice9* pDevice, const Sprite::RECT_MODE mode)
{
	const unsigned int polyCount = GetPolyCount(mode);
	const D3DPRIMITIVETYPE primType = GetPrimitiveType(mode);
	if (FAILED(pDevice->DrawPrimitive(primType, 0, polyCount)))
		return false;
	return true;
}

void D3D9VideoInfo::SetupShaderProfile()
{
	if (m_caps.VertexShaderVersion >= D3DVS_VERSION(1,1))
		m_lastVertexProfile = Shader::SP_MODEL_1;
	if (m_caps.VertexShaderVersion >= D3DVS_VERSION(2,0))
		m_lastVertexProfile = Shader::SP_MODEL_2;
	if (m_caps.VertexShaderVersion >= D3DVS_VERSION(3,0))
		m_lastVertexProfile = Shader::SP_MODEL_3;

	if (m_caps.PixelShaderVersion >= D3DPS_VERSION(1,0))
		m_lastPixelProfile = Shader::SP_MODEL_1;
	if (m_caps.PixelShaderVersion >= D3DPS_VERSION(2,0))
		m_lastPixelProfile = Shader::SP_MODEL_2;
	if (m_caps.PixelShaderVersion >= D3DPS_VERSION(3,0))
		m_lastPixelProfile = Shader::SP_MODEL_3;
}

} // namespace gs2d
