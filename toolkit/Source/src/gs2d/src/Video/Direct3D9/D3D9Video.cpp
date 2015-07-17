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

#include "D3D9Video.h"
#include "D3D9Texture.h"
#include "D3D9Sprite.h"
#include "D3D9CgShader.h"

#include "../../Platform/Platform.h"

#include "../../Unicode/utf8/utf8.h"

#include "../cgShaderCode.h"

#include <vector>

namespace gs2d {
using namespace math;

D3D9Video::RENDER_TARGET::RENDER_TARGET() :
	width(0),
	height(0),
	format(Texture::TF_DEFAULT)
{
}

D3D9Video::RENDER_TARGET::RENDER_TARGET(
	Sprite* sprite,
	const unsigned int width,
	const unsigned int height,
	const Texture::TARGET_FORMAT fmt)
{
	pSprite = sprite;
	this->width = width;
	this->height = height;
	format = fmt;
}

GS2D_API VideoPtr CreateVideo(
	const unsigned int width,
	const unsigned int height,
	const str_type::string& winTitle,
	const bool windowed,
	const bool sync,
	const Platform::FileIOHubPtr& fileIOHub,
	const Texture::PIXEL_FORMAT pfBB,
	const bool maximizable)
{
	return D3D9Video::Create(width, height, winTitle, windowed, sync, pfBB, maximizable, fileIOHub);
}

/// Platform specific user message function implementation
void ShowMessage(str_type::stringstream &stream, const GS_MESSAGE_TYPE type)
{
	if (type == GSMT_INFO)
	{
		GS2D_COUT << GS_L("GS2D INFO: ") << stream.str() << std::endl;
	}
	else if (type == GSMT_WARNING)
	{
		GS2D_CERR << GS_L("GS2D WARNING: ") << stream.str() << std::endl;
	}
	else if (type == GSMT_ERROR)
	{
		GS2D_CERR << GS_L("GS2D ERROR: ") << stream.str() << std::endl;
		MessageBoxA(NULL, stream.str().c_str(), GS_L("GS2D ERROR"), MB_OK | MB_ICONERROR);
	}
}

/// Set all view matrices to the shaders
void SetupShaderViewData(IDirect3DDevice9 *pDevice, ShaderPtr pCurrentVS, ShaderPtr pRectVS, ShaderPtr pFontVS)
{
	D3DSURFACE_DESC desc;
	IDirect3DSurface9 *pSurface = NULL;

	pDevice->GetRenderTarget(0, &pSurface);
	assert(pSurface);
	pSurface->GetDesc(&desc);
	const float width = (float)desc.Width;
	const float height = (float)desc.Height;
	Matrix4x4 ortho;
	Orthogonal(ortho, width, height, D3D9Video::ZNEAR, D3D9Video::ZFAR);

	pCurrentVS->SetMatrixConstant(GS_L("viewMatrix"), ortho);
	pCurrentVS->SetConstant(GS_L("screenSize"), Vector2(width, height));

	pRectVS->SetMatrixConstant(GS_L("viewMatrix"), ortho);
	pRectVS->SetConstant(GS_L("screenSize"), Vector2(width, height));

	pFontVS->SetMatrixConstant(GS_L("viewMatrix"), ortho);
	pFontVS->SetConstant(GS_L("screenSize"), Vector2(width, height));
	pSurface->Release();
}

/// Set the default render states
bool SetDefaultRenderStates(IDirect3DDevice9 *pDevice, D3D9Video *pVideo)
{
	//pVideo->SetClamp(true);
	pDevice->SetRenderState(D3DRS_WRAP0, 0);
	//pVideo->SetFilterMode(pVideo->GetFilterMode());
	for (unsigned int t=1; t<pVideo->GetMaxMultiTextures(); t++)
	{
		pDevice->SetTextureStageState(t, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pDevice->SetTextureStageState(t, D3DTSS_COLORARG2, D3DTA_CURRENT);
		//pDevice->SetTextureStageState(t, D3DTSS_TEXCOORDINDEX, 0);
		pVideo->SetBlendMode(t, Video::BM_MODULATE);
	}
	return true;
}

/// Creates an IDirect3D object
IDirect3D9 *CreateAPI()
{
	IDirect3D9* m_pD3D;
	if ((m_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
	{
		str_type::stringstream ss;
		ss << GS_L("Couldn't create the IDirect3D9 object");
		ShowMessage(ss, GSMT_ERROR);
		return NULL;
	}
	return m_pD3D;
}

const float D3D9Video::ZFAR = 5.0f;
const float D3D9Video::ZNEAR = 0.0f;
const DWORD D3D9Video::W32_WINDOWED_STYLE = (WS_CLIPCHILDREN | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE);
const DWORD D3D9Video::W32_FULLSCREEN_STYLE = (WS_CLIPCHILDREN | WS_POPUP);
const DWORD D3D9Video::ALPHAREF = (0x01);
SHORT D3D9Video::m_wheelDelta = 0;
bool D3D9Video::m_inputFocus = false;
std::string D3D9Video::m_currentStringInput;
D3D9Video::RENDER_TARGET_LIST D3D9Video::m_targets;
const unsigned int D3D9Video::TEXTURE_CHANNELS = 8;

boost::shared_ptr<D3D9Video> D3D9Video::Create(
	const unsigned int width,
	const unsigned int height,
	const str_type::string& winTitle,
	const bool windowed,
	const bool sync,
	const Texture::PIXEL_FORMAT pfBB,
	const bool maximizable,
	const Platform::FileIOHubPtr& fileIOHub)
{
	boost::shared_ptr<D3D9Video> p(
		new D3D9Video(width, height, winTitle, windowed, sync, pfBB, maximizable, fileIOHub));
	p->weak_this = p;
	return p;
}

TexturePtr D3D9Video::CreateTextureFromFileInMemory(
	const void* pBuffer,
	const unsigned int bufferLength,
	Color mask,
	const unsigned int width,
	const unsigned int height,
	const unsigned int nMipMaps)
{
	TexturePtr texture(new D3D9Texture);
	if (texture->LoadTexture(weak_this, pBuffer, mask, width, height, nMipMaps, bufferLength))
	{
		return texture;
	}
	return TexturePtr();
}

TexturePtr D3D9Video::LoadTextureFromFile(
	const str_type::string& fileName,
	Color mask,
	const unsigned int width,
	const unsigned int height,
	const unsigned int nMipMaps)
{
	TexturePtr texture(new D3D9Texture);
	if (texture->LoadTexture(weak_this, fileName, mask, width, height, nMipMaps))
	{
		return texture;
	}
	return TexturePtr();
}

TexturePtr D3D9Video::CreateRenderTargetTexture(
	const unsigned int width,
	const unsigned int height,
	const Texture::TARGET_FORMAT fmt)
{
	TexturePtr texture(new D3D9Texture);
	if (texture->CreateRenderTarget(weak_this, width, height, fmt))
	{
		return texture;
	}
	return TexturePtr();
}

SpritePtr D3D9Video::CreateSprite(
	GS_BYTE* pBuffer,
	const unsigned int bufferLength,
	Color mask,
	const unsigned int width,
	const unsigned int height)
{
	SpritePtr sprite(new D3D9Sprite);
	if (sprite->LoadSprite(weak_this, pBuffer, bufferLength, mask, width, height))
	{
		return sprite;
	}
	return SpritePtr();
}

SpritePtr D3D9Video::CreateSprite(
	const str_type::string& fileName,
	Color mask,
	const unsigned int width,
	const unsigned int height)
{
	SpritePtr sprite(new D3D9Sprite);
	if (sprite->LoadSprite(weak_this, fileName, mask, width, height))
	{
		return sprite;
	}
	return SpritePtr();
}

SpritePtr D3D9Video::CreateRenderTarget(
	const unsigned int width,
	const unsigned int height,
	const Texture::TARGET_FORMAT format)
{
	SpritePtr sprite(new D3D9Sprite);
	if (sprite->CreateRenderTarget(weak_this, width, height, format))
	{
		return sprite;
	}
	return SpritePtr();
}

ShaderPtr D3D9Video::LoadShaderFromFile(
	const str_type::string& fileName,
	const Shader::SHADER_FOCUS focus,
	const Shader::SHADER_PROFILE profile,
	const char *entry)
{
	ShaderPtr shader = ShaderPtr(new D3D9CgShader);
	if (shader->LoadShaderFromFile(m_shaderContext, fileName, focus, profile, entry))
	{
		return shader;
	}
	return ShaderPtr();
}

ShaderPtr D3D9Video::LoadShaderFromString(
	const str_type::string& shaderName,
	const std::string& codeAsciiString,
	const Shader::SHADER_FOCUS focus,
	const Shader::SHADER_PROFILE profile,
	const char *entry)
{
	ShaderPtr shader(new D3D9CgShader);
	if (shader->LoadShaderFromString(m_shaderContext, shaderName, codeAsciiString, focus, profile, entry))
	{
		return shader;
	}
	return ShaderPtr();
}

D3D9Video::D3D9Video(
	const unsigned int width,
	const unsigned int height,
	const str_type::string& winTitle,
	const bool windowed,
	const bool sync,
	const Texture::PIXEL_FORMAT pfBB,
	const bool maximizable,
	const Platform::FileIOHubPtr& fileIOHub) :
	m_videoInfo(new D3D9VideoInfo),
	m_blendModes(TEXTURE_CHANNELS),
	m_pDevice(NULL),
	m_pD3D(NULL),
	m_rendering(false),
	m_sync(false),
	m_windowed(true),
	m_scissor(Rect2D(0,0,0,0)),
	m_alphaMode(AM_PIXEL),
	m_textureFilter(TM_ALWAYS),
	m_cursorHidden(false),
	m_quit(false),
	m_windowPos(Vector2i(0,0)),
	m_clamp(true),
	m_fpsRate(60.0f),
	m_maximizable(false),
	m_fileIOHub(fileIOHub)
{
	for (unsigned int t = 1; t < TEXTURE_CHANNELS; t++)
		m_blendModes[t] = BM_MODULATE;

	m_currentStringInput.clear();
	m_wheelDelta = 0;
	m_inputFocus = false;
	StartApplication(width, height, winTitle, windowed, sync, pfBB, maximizable);
}

D3D9Video::~D3D9Video()
{
	m_videoInfo->ReleaseVB();
	if (m_videoInfo->m_pBackBuffer)
	{
		m_videoInfo->m_pBackBuffer->Release();
		m_videoInfo->m_pBackBuffer = NULL;
	}
	if (m_pDevice)
	{
		m_pDevice->Release();
		m_pDevice = NULL;
	}
	if (m_pD3D)
	{
		m_pD3D->Release();
		m_pD3D = NULL;
	}

	//ShowWindow(m_videoInfo->m_hWnd, SW_HIDE);
	SendMessage(m_videoInfo->m_hWnd, WM_CLOSE, 0, 0);
	UnregisterClassA(m_videoInfo->className.c_str(), m_videoInfo->m_hInstance);
}

void D3D9Video::RemoveFromTargetList(Sprite *pSprite)
{
	if (!m_targets.empty())
	{
		for (RENDER_TARGET_LIST::iterator iter = m_targets.begin(); iter != m_targets.end(); )
		{
			if (pSprite == iter->pSprite)
				iter = m_targets.erase(iter);
			else
				++iter;
		}
	}
}

void D3D9Video::AddToTargetList(const RENDER_TARGET &target)
{
	m_targets.push_back(target);
}

/// Windows event handling
LRESULT WINAPI D3D9Video::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	D3D9VideoInfo* pInfo = (D3D9VideoInfo*)(GetWindowLong(hWnd, GWL_USERDATA));

	switch(msg)
	{
		case WM_CREATE:
			{
				CREATESTRUCT* creation = (CREATESTRUCT*)(lParam);
				pInfo = (D3D9VideoInfo*)(creation->lpCreateParams);
				SetWindowLong(hWnd, GWL_USERDATA, (LONG)(pInfo));
			}
			return 0;

		case WM_DESTROY:
			PostQuitMessage(wParam);
			return 0;

		case WM_MOUSEWHEEL:
			D3D9Video::m_wheelDelta = (SHORT)GET_WHEEL_DELTA_WPARAM(wParam);
			return 0;

		case WM_SYSCOMMAND:
			switch (wParam)
			{
				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
				return 0;
			}
			break;

		case WM_SETFOCUS:
			pInfo->m_inFocus = true;
			return 0;

		case WM_KILLFOCUS:
			pInfo->m_inFocus = false;
			return 0;

		case WM_SIZE:
			switch (wParam)
			{
				case SIZE_MINIMIZED:
					pInfo->m_visible = false;
					return 0;

				case SIZE_RESTORED:
					// if it's already visible, probably it's because the user is trying to leave the maximized mode
					if (pInfo->m_maximized)
					{
						pInfo->m_maximized = false;
						pInfo->v2Resize = pInfo->v2RestoredSize;
					}
					pInfo->m_visible = true;
					return 0;

				default:
					pInfo->v2Resize = Vector2i(LOWORD(lParam), HIWORD(lParam));
					pInfo->m_visible = true;
					pInfo->m_maximized = true;
					return 0;
			}
			break;
		case WM_SYSKEYDOWN:
			if (wParam == VK_F4)
				if (!pInfo->m_quitKeysEnabled)
					return 0;
			break;
		case WM_CHAR:
			switch (wParam) 
			{ 
				case 0x0A:  // linefeed 
				case 0x1B:  // escape 
				case 0x0D:
					break;
				case 0x08:  // backspace
					m_currentStringInput.resize(1, 0x08);
					break;
 				default:	// displayable character 
					unsigned short* end = (unsigned short*)(&wParam); // wParam enters as UTF-16 according to docs
					end++;
					utf8::utf16to8((unsigned short*)&wParam, end, back_inserter(m_currentStringInput));
 					break; 
			} 
			break; 

	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

boost::any D3D9Video::GetVideoInfo()
{
	return m_videoInfo;
}

void D3D9Video::EnableQuitShortcuts(const bool enable)
{
	m_videoInfo->m_quitKeysEnabled = enable;
}

bool D3D9Video::QuitShortcutsEnabled()
{
	return m_videoInfo->m_quitKeysEnabled;
}

void D3D9Video::SetBGColor(const Color& backgroundColor)
{
	m_backgroundColor = backgroundColor;
}

Color D3D9Video::GetBGColor() const
{
	return m_backgroundColor;
}

void D3D9Video::Message(const str_type::string& text, const GS_MESSAGE_TYPE type) const
{
	str_type::stringstream ss;
	ss << text;
	ShowMessage(ss, type);
}

bool D3D9Video::BeginScene(const Color& bgColor, const bool clear)
{
	const Color color = (bgColor.color != 0x0) ? bgColor : m_backgroundColor;

	if (m_windowed)
		SetCursor(m_videoInfo->m_cursor);

	if (clear)
	{
		if (FAILED(m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, color, 1.0f, 0)))
		{
			Message(GS_L("IDirect3DDevice9::Clear has failed"), GSMT_ERROR);
			return false;
		}
	}
	if (FAILED(m_pDevice->BeginScene()))
	{
		Message(GS_L("IDirect3DDevice9::BeginScene has failed"), GSMT_ERROR);
		return false;
	}
	m_rendering = true;
	return true;
}

bool D3D9Video::EndScene(const bool swap)
{
	if (FAILED(m_pDevice->EndScene()))
	{
		Message(GS_L("IDirect3DDevice9::EndScene has failed"), GSMT_ERROR);
	}
	if (swap)
	{
		HRESULT hr = m_pDevice->Present(NULL, NULL, NULL, NULL);
		if (hr == D3DERR_DEVICELOST)
			m_videoInfo->m_deviceLost = true;
		else if (FAILED(hr))
		{
			Message(GS_L("IDirect3DDevice9::Present has failed"), GSMT_ERROR);
		}
	}

	m_rendering = false;
	return true;
}

bool D3D9Video::BeginSpriteScene(const Color& bgColor)
{
	if (!(BeginScene(bgColor)))
	{
		Message(GS_L("IDirect3DDevice9::BeginScene has failed"));
	}
	m_pDevice->SetRenderState(D3DRS_AMBIENT, gs2d::constant::WHITE);
	m_pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	for (unsigned int t = 0; t < TEXTURE_CHANNELS; t++)
		m_pDevice->SetSamplerState(t, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	SetAlphaMode(AM_PIXEL);
	return true;
}

bool D3D9Video::EndSpriteScene()
{
	m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	SetAlphaMode(AM_NONE);
	for (unsigned int t=0; t<TEXTURE_CHANNELS; t++)
	{
		m_pDevice->SetSamplerState(t, D3DSAMP_MAXANISOTROPY, 1);
	}
	EndScene();
	ComputeFPSRate();
	return true;
}

bool D3D9Video::BeginTargetScene(const Color& bgColor, const bool clear)
{
	if (FAILED(BeginScene(bgColor, clear)))
	{
		Message(GS_L("IDirect3DDevice9::BeginScene has failed"));
	}
	m_pDevice->SetRenderState(D3DRS_AMBIENT, gs2d::constant::WHITE);
	for (unsigned int t=0; t<TEXTURE_CHANNELS; t++)
		m_pDevice->SetSamplerState(t, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	SetAlphaMode(AM_PIXEL);
	return true;
}

bool D3D9Video::EndTargetScene()
{
	m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	SetAlphaMode(AM_NONE);
	for (unsigned int t=0; t<TEXTURE_CHANNELS; t++)
	{
		m_pDevice->SetSamplerState(t, D3DSAMP_MAXANISOTROPY, 1);
	}
	EndScene(false);
	return true;
}

bool D3D9Video::SetClamp(const bool set)
{
	if (set)
	{
		for (unsigned int t=0; t<TEXTURE_CHANNELS; t++)
		{
			m_pDevice->SetSamplerState(t, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			m_pDevice->SetSamplerState(t, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		}
	}
	else
	{
		for (unsigned int t=0; t<TEXTURE_CHANNELS; t++)
		{
			m_pDevice->SetSamplerState(t, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
			m_pDevice->SetSamplerState(t, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		}
	}
	m_clamp = set;
	return true;
}

bool D3D9Video::GetClamp() const
{
	return m_clamp;
}

void D3D9Video::SetZBuffer(const bool enable)
{
	m_pDevice->SetRenderState(D3DRS_ZENABLE, (enable) ? D3DZB_TRUE : D3DZB_FALSE);
}

bool D3D9Video::GetZBuffer() const
{
	DWORD dwEnable;
	m_pDevice->GetRenderState(D3DRS_ZENABLE, &dwEnable);
	return (dwEnable != D3DZB_FALSE);
}

void D3D9Video::SetZWrite(const bool enable)
{
	m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, (enable) ? TRUE : FALSE);
}
bool D3D9Video::GetZWrite() const
{
	DWORD dwEnable;
	m_pDevice->GetRenderState(D3DRS_ZWRITEENABLE, &dwEnable);
	return (dwEnable != FALSE);
}

bool D3D9Video::SetScissor(const bool& enable)
{
	return (m_pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, (enable) ? TRUE : FALSE) == D3D_OK);
}

bool D3D9Video::SetScissor(const Rect2D& rect)
{
	m_scissor = rect;
	if (!m_videoInfo->m_scissorSupported)
	{
		Message(GS_L("Hardware scissors are not supported - D3D9Video::SetScissor"));
		return false;
	}

	RECT wrect;
	wrect.left   = rect.pos.x;
	wrect.top	 = rect.pos.y;
	wrect.right  = rect.pos.x+rect.size.x;
	wrect.bottom = rect.pos.y+rect.size.y;

	// safety
	{
		const LONG screenWidth  = GetScreenSize().x;
		const LONG screenHeight = GetScreenSize().y;

		wrect.right = Clamp(wrect.right, 0L, screenWidth);
		wrect.left = Clamp(wrect.left, 0L, screenWidth);

		wrect.bottom = Clamp(wrect.bottom, 0L, screenHeight);
		wrect.top = Clamp(wrect.top, 0L, screenHeight);
	}

	if (wrect.right >= wrect.left && wrect.bottom >= wrect.top)
	{
		m_pDevice->SetScissorRect(&wrect);
		SetScissor(true);
	}
	else
	{
		SetScissor(false);
	}
	return true;
}

Rect2D D3D9Video::GetScissor() const
{
	return m_scissor;
}

void D3D9Video::UnsetScissor()
{
	m_pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
	//m_scissor = Rect2D(0,0,0,0);
}

bool D3D9Video::SetAlphaMode(const ALPHA_MODE mode)
{
	m_alphaMode = mode;
	unsigned int t;

	switch(mode)
	{
	case AM_PIXEL:
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		for (t=0; t<1; t++)
		{
			m_pDevice->SetTextureStageState(t, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			m_pDevice->SetTextureStageState(t, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
			m_pDevice->SetTextureStageState(t, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			m_pDevice->SetTextureStageState(t, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			m_pDevice->SetTextureStageState(t, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			m_pDevice->SetTextureStageState(t, D3DTSS_COLOROP, D3DTOP_MODULATE);
		}
		m_pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		m_pDevice->SetRenderState(D3DRS_ALPHAREF, ALPHAREF);
		m_pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
		break;
	case AM_ADD:
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		for (t=0; t<1; t++)
		{
			m_pDevice->SetTextureStageState(t, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			m_pDevice->SetTextureStageState(t, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
			m_pDevice->SetTextureStageState(t, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			m_pDevice->SetTextureStageState(t, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			m_pDevice->SetTextureStageState(t, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			m_pDevice->SetTextureStageState(t, D3DTSS_COLOROP, D3DTOP_MODULATE);
		}
		m_pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

		m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		m_pDevice->SetRenderState(D3DRS_ALPHAREF, ALPHAREF);
		m_pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
		break;
	case AM_MODULATE:
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		for (t=0; t<1; t++)
		{
			m_pDevice->SetTextureStageState(t, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			m_pDevice->SetTextureStageState(t, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
			m_pDevice->SetTextureStageState(t, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			m_pDevice->SetTextureStageState(t, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			m_pDevice->SetTextureStageState(t, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			m_pDevice->SetTextureStageState(t, D3DTSS_COLOROP, D3DTOP_MODULATE);
		}
		m_pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);

		m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		m_pDevice->SetRenderState(D3DRS_ALPHAREF, ALPHAREF);
		m_pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
		break;
	case AM_ALPHA_TEST:
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		m_pDevice->SetRenderState(D3DRS_ALPHAREF, ALPHAREF);
		m_pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
		break;
	case AM_NONE:
	default:
		m_alphaMode = AM_NONE;
		m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		break;
	};

	return true;
}

Video::ALPHA_MODE D3D9Video::GetAlphaMode() const
{
	return m_alphaMode;
}

bool D3D9Video::SetFilterMode(const TEXTUREFILTER_MODE tfm)
{
	m_textureFilter = tfm;
	if (GetFilterMode() != TM_NEVER)
	{
		for (unsigned int t=0; t<TEXTURE_CHANNELS; t++)
		{
			m_pDevice->SetSamplerState(t, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			m_pDevice->SetSamplerState(t, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			//pDevice->SetSamplerState(t, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
		}
	}
	else
	{
		for (unsigned int t=0; t<TEXTURE_CHANNELS; t++)
		{
			m_pDevice->SetSamplerState(t, D3DSAMP_MINFILTER, D3DTEXF_POINT);
			m_pDevice->SetSamplerState(t, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
			//pDevice->SetSamplerState(t, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
		}
	}
	return true;
}

Video::TEXTUREFILTER_MODE D3D9Video::GetFilterMode() const
{
	return m_textureFilter;
}

bool D3D9Video::Rendering() const
{
	return m_rendering;
}

bool D3D9Video::ResetVideoMode(
	const unsigned int width,
	const unsigned int height,
	const Texture::PIXEL_FORMAT pfBB,
	const bool toggleFullscreen)
{
	VIDEO_MODE mode;
	mode.width = width;
	mode.height = height;
	mode.pf = pfBB;
	return ResetVideoMode(mode, toggleFullscreen);
}

bool D3D9Video::ResetVideoMode(const VIDEO_MODE& mode, const bool toggleFullscreen)
{
	UnsetScissor();
	const bool rendering = Rendering();
	const bool zEnable = GetZBuffer();
	const bool zWrite = GetZWrite();
	const bool clamp = GetClamp();
	const ALPHA_MODE alphaMode = GetAlphaMode();
	const TEXTUREFILTER_MODE filterMode = GetFilterMode();
	std::vector<BLEND_MODE> blendModes(GetMaxMultiTextures(), BM_MODULATE);
	for (unsigned int t=1; t<blendModes.size(); t++)
	{
		blendModes[t] = GetBlendMode(t);
	}

	if (rendering)
	{
		Message(GS_L("scene ended"), GSMT_INFO);
		EndSpriteScene();
	}
	Message(GS_L("starting device reset"), GSMT_INFO);

	// Prepare render targets before reseting the device
	if (!m_targets.empty())
	{
		Message(GS_L("deleting render targets..."), GSMT_INFO);
		RENDER_TARGET_LIST::iterator iter;
		for (iter = m_targets.begin(); iter != m_targets.end(); ++iter)
		{
			Sprite* sprite = iter->pSprite;
			if (sprite->GetType() != Sprite::T_NOT_LOADED)
				sprite->OnLostDevice();
		}
		Message(GS_L("render targets deleted"), GSMT_INFO);
	}

	// Destroy the backbuffer
	if (m_videoInfo->m_pBackBuffer)
	{
		m_videoInfo->m_pBackBuffer->Release();
		m_videoInfo->m_pBackBuffer = NULL;
		Message(GS_L("backbuffer released"), GSMT_INFO);
	}

	// if the mode is toggled, do the window properties changing
	D3DPRESENT_PARAMETERS& d3dpp = m_videoInfo->m_d3dPP;
	if (toggleFullscreen)
	{
		Message(GS_L("trying to toggle fullscreen"), GSMT_INFO);
		d3dpp.Windowed = !d3dpp.Windowed;
		m_videoInfo->m_windowStyle = (d3dpp.Windowed) ? W32_WINDOWED_STYLE : W32_FULLSCREEN_STYLE;

		// if the window is maximizable, activate the maximize button
		if (m_maximizable)
		{
			m_videoInfo->m_windowStyle |= WS_MAXIMIZEBOX;
		}

		SetWindowLongPtr(m_videoInfo->m_hWnd, GWL_STYLE, m_videoInfo->m_windowStyle);
		SetWindowPos(m_videoInfo->m_hWnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
		m_windowed = !m_windowed;
	}

	// change sizes if the user wants to resize the backbuffer
	if (mode.width != 0 && mode.height != 0 || toggleFullscreen)
	{
		Message(GS_L("recreating the D3DPP..."), GSMT_INFO);
		d3dpp.BackBufferWidth  = m_screenDim.x = mode.width;
		d3dpp.BackBufferHeight = m_screenDim.y = mode.height;

		if (m_windowed)
		{
			RECT rect;
			rect.left = 0; rect.top = 0; rect.right = m_screenDim.x; rect.bottom = m_screenDim.y;
			AdjustWindowRectEx(&rect, m_videoInfo->m_windowStyle, FALSE, 0);

			m_windowedDim.x = rect.right-rect.left;
			m_windowedDim.y = rect.bottom-rect.top;
			m_topBarSize.x = -rect.left;
			m_topBarSize.y = -rect.top;
			MoveWindow(m_videoInfo->m_hWnd, GetWindowPosition().x, GetWindowPosition().y, m_windowedDim.x, m_windowedDim.y, TRUE);
		}
	}
	else // If it's being reset because the device has been lost, keep the same size
	{
		d3dpp.BackBufferWidth  = m_screenDim.x;
		d3dpp.BackBufferHeight = m_screenDim.y;
	}

	// Reset it
	const HRESULT hr = m_pDevice->Reset(&d3dpp);
	if (FAILED(hr))
	{
		Message(GS_L("Couldn't reset the video mode"));
	}
	else
	{
		Message(GS_L("device successfully reset"), GSMT_INFO);
		SetVertexShader(GetVertexShader());
		SetPixelShader(GetPixelShader());
		SetupShaderViewData(m_pDevice, GetVertexShader(), m_rectVS, m_fastVS);

		m_videoInfo->m_deviceLost = false;

		if (m_windowed)
		{
			m_windowPos = (GetClientScreenSize()/2) - (GetScreenSize()/2);
		}
		else
		{
			m_windowPos = Vector2i(0,0);
		}

	}

	// if the device is no longer lost
	if (!m_videoInfo->m_deviceLost)
	{
		// reset all render targets
		if (!m_targets.empty())
		{
			RENDER_TARGET_LIST::iterator iter;
			for (iter = m_targets.begin(); iter != m_targets.end(); ++iter)
			{
				Sprite* sprite = iter->pSprite;
				if (sprite->GetType() == Sprite::T_RELOAD)
				{
					sprite->CreateRenderTarget(weak_this, iter->width, iter->height, iter->format);
					sprite->RecoverFromBackup();
				}
			}
			Message(GS_L("targets recovered from backup (if possible)"), GSMT_INFO);
		}
		// Get the backbuffer again
		m_pDevice->GetRenderTarget(0, &m_videoInfo->m_pBackBuffer);
	}

	SetDefaultRenderStates(m_pDevice, this);
	SetZBuffer(zEnable);
	SetFilterMode(filterMode);
	SetZWrite(zWrite);
	SetClamp(clamp);
	SetAlphaMode(alphaMode);
	for (unsigned int t=1; t<blendModes.size(); t++)
	{
		SetBlendMode(t, blendModes[t]);
	}

	// if it has been done during the rendering... keep it up
	if (rendering)
	{
		Message(GS_L("begining scene rendering"), GSMT_INFO);
		BeginSpriteScene();
	}
	Message(GS_L("device successfully reset"), GSMT_INFO);

	ScreenSizeChangeListenerPtr listener = m_screenSizeChangeListener.lock();
	if (listener)
		listener->ScreenSizeChanged(GetScreenSizeF());
	return true;
}

bool D3D9Video::IsWindowed() const
{
	return m_windowed;
}

void D3D9Video::ForwardCommand(const str_type::string& cmd)
{
	GS2D_UNUSED_ARGUMENT(cmd);
	// TODO/TO-DO: implement...
}

str_type::string D3D9Video::PullCommands()
{
	// TODO/TO-DO: implement
	return GS_L("");
}

Video::APP_STATUS D3D9Video::HandleEvents()
{
	// handles window resizing
	if (m_maximizable)
	{
		Vector2i& resize = m_videoInfo->v2Resize;
		if (resize.x > 0 && resize.y > 0)
		{
			ResetVideoMode(resize.x, resize.y, Texture::PF_UNKNOWN, false);
			resize = Vector2i(0,0);
		}
	}

	if (m_videoInfo->m_msg.message == WM_QUIT || m_quit)
		return APP_QUIT;

	if(PeekMessage(&m_videoInfo->m_msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&m_videoInfo->m_msg);
		DispatchMessage(&m_videoInfo->m_msg);
		return APP_SKIP;
	}
	else
	{
		// The following piece of code deals with an eventual device loss.
		// As all textures are stored using D3DPOOL_MANAGED, we don't need to
		//erase them before we reset the device. We must do that only with
		//fonts and render targets (if there are any)
		if (m_videoInfo->m_deviceLost)
		{
			HRESULT hr;
			if (FAILED(hr = m_pDevice->TestCooperativeLevel()))
			{
				if (hr == D3DERR_DEVICELOST)
				{
					//gsSetInfoMessage("couldn't reset the device yet...", NULL);
					return APP_SKIP;
				}

				if (hr == D3DERR_DEVICENOTRESET)
				{
					ResetVideoMode(0, 0, Texture::PF_UNKNOWN);
					//Message("Device reset", GSMT_INFO);
				}
			}
		}
		m_inputFocus = (WindowVisible() && WindowInFocus());
		if (m_videoInfo->m_deviceLost)
			return APP_SKIP;
	}
	return APP_OK;
}

ShaderPtr D3D9Video::GetVertexShader()
{
	return m_pCurrentVS;
}

ShaderPtr D3D9Video::GetPixelShader()
{
	return m_pCurrentPS;
}

bool D3D9Video::SetVertexShader(ShaderPtr pShader)
{
	if (!pShader)
	{
		m_pCurrentVS = m_defaultVS;
	}
	else
	{
		if (pShader->GetShaderFocus() != Shader::SF_VERTEX)
		{
			Message(GS_L("The shader set is not a vertex program - D3D9Video::SetVertexShader"));
			return false;
		}
		m_pCurrentVS = pShader;
	}
	SetupShaderViewData(m_pDevice, GetVertexShader(), m_rectVS, m_fastVS);

	return true;
}

bool D3D9Video::SetPixelShader(ShaderPtr pShader)
{
	m_pCurrentPS = pShader;

	if (!pShader)
	{
		m_pDevice->SetPixelShader(NULL);
		for (unsigned int t=0; t<TEXTURE_CHANNELS; t++)
			m_pDevice->SetTexture(t, NULL);
		return true;
	}
	else
	if (pShader->GetShaderFocus() != Shader::SF_PIXEL)
	{
		Message(GS_L("The shader set is not a vertex program - D3D9Video::SetPixelShader"));
		return false;
	}
	return true;
}

Shader::SHADER_PROFILE D3D9Video::GetHighestVertexProfile() const
{
	return m_videoInfo->m_lastVertexProfile;
}

Shader::SHADER_PROFILE D3D9Video::GetHighestPixelProfile() const
{
	return m_videoInfo->m_lastPixelProfile;
}

ShaderContextPtr D3D9Video::GetShaderContext()
{
	return m_shaderContext;
}

void D3D9Video::ComputeFPSRate()
{
	static float counter = 0.0f;
	static DWORD last = GetTickCount();
	const DWORD current = GetTickCount();

	const DWORD elapsed = current-last;
	if (elapsed > 500)
	{
		m_fpsRate = (counter*2);
		counter = 0.0f;
		last = GetTickCount();
	}
	else
	{
		counter++;
	}
}

float D3D9Video::GetFPSRate() const
{
	return Max(1.0f, m_fpsRate);
}

ShaderPtr D3D9Video::GetFontShader()
{
	return m_fastVS;
}

ShaderPtr D3D9Video::GetOptimalVS()
{
	return m_defaultVS;
}

ShaderPtr D3D9Video::GetDefaultVS()
{
	return m_defaultVS;
}

bool D3D9Video::UnsetTexture(const unsigned int passIdx)
{
	m_pDevice->SetTexture(passIdx, NULL);
	return true;
}

bool D3D9Video::SetBlendMode(const unsigned int passIdx, const BLEND_MODE mode)
{
	if (passIdx <= 0)
	{
		Message(GS_L("nPass can't be less or equal to 0 - D3D9Video::SetBlendMode"));
		return false;
	}
	m_pDevice->SetTextureStageState(passIdx, D3DTSS_COLOROP, (mode==BM_ADD) ? D3DTOP_ADD : D3DTOP_MODULATE);
	m_blendModes[passIdx] = mode;
	return true;
}

Video::BLEND_MODE D3D9Video::GetBlendMode(const unsigned int passIdx) const
{
	return m_blendModes[passIdx];
}

bool D3D9Video::DrawLine(const Vector2 &p1, const Vector2 &p2, const Color& color1, const Color& color2)
{
	if (GetLineWidth() <= 1.0f)
	{
		D3D9VideoInfo::LINE_VERTEX line[2];
		line[0].pos.x = p1.x;
		line[0].pos.y = p1.y;
		line[0].pos.z = 1.0f;
		line[0].rhw = 0.0f;
		line[0].color = color1;

		line[1].pos.x = p2.x;
		line[1].pos.y = p2.y;
		line[1].pos.z = 1.0f;
		line[1].rhw = 0.0f;
		line[1].color = color2;

		const DWORD fvf = (D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
		m_pDevice->SetFVF(fvf);

		ShaderPtr vertexShader = GetVertexShader(), pixelShader = GetPixelShader();

		m_fastVS->SetShader();
		SetPixelShader(ShaderPtr());

		for (unsigned int t=0; t<TEXTURE_CHANNELS; t++)
			m_pDevice->SetTexture(t, NULL);

		m_pDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, line, sizeof(D3D9VideoInfo::LINE_VERTEX));
		
		SetPixelShader(pixelShader);
		SetVertexShader(vertexShader);
	}
	else
	{
		Vector2 a = p1, b = p2;
		if (a == b)
		{
			Message(GS_L("The line can't be a point (a can't be == b) - D3D9Video::DrawLine"), GSMT_WARNING);
			return false;
		}
		const Vector2 v2Dir = a - b;
		const float len = Distance(p1, p2);
		const float angle = RadianToDegree(GetAngle(v2Dir));

		DrawRectangle(
			p1,
			Vector2(GetLineWidth(), len),
			color2,
			color2,
			color1,
			color1,
			angle,
			Sprite::EO_CENTER_BOTTOM);
	}

	return true;
}

bool D3D9Video::DrawRectangle(const Vector2 &v2Pos, const Vector2 &v2Size,
						const Color& color, const float angle, const Sprite::ENTITY_ORIGIN origin)
{
	return DrawRectangle(v2Pos, v2Size,	color, color, color, color, angle, origin);
}

bool D3D9Video::DrawRectangle(const Vector2 &v2Pos, const Vector2 &v2Size,
						const Color& color0, const Color& color1, const Color& color2, const Color& color3,
						const float angle, const Sprite::ENTITY_ORIGIN origin)
{
	if (v2Size == Vector2(0,0))
	{
		return true;
	}

	Vector2 v2Center;
	switch (origin)
	{
	case Sprite::EO_CENTER:
	case Sprite::EO_RECT_CENTER:
		v2Center.x = v2Size.x / 2.0f;
		v2Center.y = v2Size.y / 2.0f;
		break;
	case Sprite::EO_RECT_CENTER_BOTTOM:
	case Sprite::EO_CENTER_BOTTOM:
		v2Center.x = v2Size.x / 2.0f;
		v2Center.y = v2Size.y;
		break;
	case Sprite::EO_RECT_CENTER_TOP:
	case Sprite::EO_CENTER_TOP:
		v2Center.x = v2Size.x / 2.0f;
		v2Center.y = 0.0f;
		break;
	case Sprite::EO_DEFAULT:
	default:
		v2Center.x = 0.0f;
		v2Center.y = 0.0f;
		break;
	};

	Matrix4x4 mRot;
	if (angle != 0.0f)
		mRot = RotateZ(DegreeToRadian(angle));
	m_rectVS->SetMatrixConstant(GS_L("rotationMatrix"), mRot);
	m_rectVS->SetConstant(GS_L("size"), v2Size);
	m_rectVS->SetConstant(GS_L("entityPos"), v2Pos);
	m_rectVS->SetConstant(GS_L("center"), v2Center);
	m_rectVS->SetConstant(GS_L("color0"), color0);
	m_rectVS->SetConstant(GS_L("color1"), color1);
	m_rectVS->SetConstant(GS_L("color2"), color2);
	m_rectVS->SetConstant(GS_L("color3"), color3);

	ShaderPtr vertexShader = GetVertexShader(), pixelShader = GetPixelShader();

	m_rectVS->SetShader();
	SetPixelShader(ShaderPtr());

	for (unsigned int t=0; t<TEXTURE_CHANNELS; t++)
		m_pDevice->SetTexture(t, NULL);

	if (!m_videoInfo->DrawSprite(m_pDevice, Sprite::RM_TWO_TRIANGLES))
	{
		Message(GS_L("Rendering failed - D3D9Video::DrawRectangle"));
		return false;
	}

	SetPixelShader(pixelShader);
	SetVertexShader(vertexShader);

	return true;
}

Video::VIDEO_MODE D3D9Video::GetVideoMode(const unsigned int modeIdx) const
{
	if (modeIdx >= m_modes.size())
	{
		VIDEO_MODE mode;
		mode.width = mode.height = 0;
		mode.pf = Texture::PF_UNKNOWN;
		mode.idx = 0x0;

		Message(GS_L("The selected video mode doesn't exist - D3D9Video::GetVideoMode"));
		return mode;
	}
	return m_modes[modeIdx];
}

unsigned int D3D9Video::GetVideoModeCount() const
{
	if (!m_pD3D)
	{
		if ((m_pD3D = CreateAPI()) == NULL)
		{
			Message(GS_L("Couldn't create the IDirect3D9 object - D3D9Video::GetVideoModeCount"));
			return 0;
		}
		SetDisplayModes(m_pD3D);
	}
	return m_modes.size();
}

bool D3D9Video::StartApplication(const unsigned int width, const unsigned int height,
								const str_type::string& winTitle, const bool windowed,
								const bool sync, const Texture::PIXEL_FORMAT pfBB, const bool maximizable)
{
	if (!m_pD3D)
	{
		if ((m_pD3D = CreateAPI()) == NULL)
		{
			Message(GS_L("Couldn't create the IDirect3D9 object - D3D9Video::StartApplication"));
			return false;
		}
		SetDisplayModes(m_pD3D);
		Message(GS_L("creating the API object IDirect3D9"), GSMT_INFO);
	}

	m_sync = sync;

	// open window
	WNDCLASSEXA wc = { sizeof(WNDCLASSEX), 0, MsgProc, 0, 0,
						GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
						winTitle.c_str(), NULL };
	m_videoInfo->m_hInstance = wc.hInstance;
	m_videoInfo->className = winTitle;

	RegisterClassExA(&wc);

	m_windowTitle = winTitle;
	m_screenDim.x = width;
	m_screenDim.y = height;
	m_maximizable = maximizable;

	// if sizes are zero, use the best resolution possible
	if (m_screenDim.x == 0 || m_screenDim.y == 0)
	{
		const int nMode = m_modes.size() - 1;
		m_screenDim.x = m_modes[nMode].width;
		m_screenDim.y = m_modes[nMode].height;

	}

	RECT rect;
	rect.left = 0; rect.top = 0; rect.right = m_screenDim.x; rect.bottom = m_screenDim.y;

	m_videoInfo->m_windowStyle = (windowed) ? W32_WINDOWED_STYLE : W32_FULLSCREEN_STYLE;

	// if the window is maximizable, activate the maximize button
	if (m_maximizable)
	{
		m_videoInfo->m_windowStyle |= WS_MAXIMIZEBOX;
	}

	AdjustWindowRectEx(&rect, m_videoInfo->m_windowStyle, FALSE, 0);

	if (!windowed)
	{
		m_windowedDim = m_screenDim;
		m_topBarSize = Vector2i(0,0);
		m_windowPos = Vector2i(0,0);
	}
	else
	{
		m_windowPos = (GetClientScreenSize()/2) - (GetScreenSize()/2);
		m_windowedDim.x = rect.right-rect.left;
		m_windowedDim.y = rect.bottom-rect.top;
		m_topBarSize.x = -rect.left;
		m_topBarSize.y = -rect.top;
	}

	m_videoInfo->m_hWnd= CreateWindowA(m_videoInfo->className.c_str(), winTitle.c_str(),
							m_videoInfo->m_windowStyle, m_windowPos.x, m_windowPos.y, m_windowedDim.x, m_windowedDim.y,
							GetDesktopWindow(), NULL, wc.hInstance, m_videoInfo.get());

	// convert from GSlib format to D3D_FORMAT
	D3DFORMAT d3dFmt;
	switch (pfBB)
	{
	case Texture::PF_32BIT:
		d3dFmt = D3DFMT_X8R8G8B8;
		break;
	case Texture::PF_16BIT:
		d3dFmt = D3DFMT_R5G6B5;
		break;
	default:
		d3dFmt = D3DFMT_UNKNOWN;
		break;
	};

	// get the current display mode for checking
	D3DDISPLAYMODE currentMode;
	if(FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &currentMode)))
	{
		Message(GS_L("Couldn't get the display mode - D3D9Video::StartApplication"));
		m_pD3D->Release();
		m_pD3D = NULL;
		return false;
	}
	const D3DFORMAT fmtCurrent = currentMode.Format;
	if (d3dFmt == D3DFMT_UNKNOWN)
		d3dFmt = fmtCurrent;

	// check if it'll work
	if(FAILED(m_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
										fmtCurrent, D3DUSAGE_RENDERTARGET,
										D3DRTYPE_SURFACE, d3dFmt)))
	{
		Message(GS_L("The back buffer format had to be changed to default - D3D9Video::StartApplication"));
		d3dFmt = fmtCurrent;
	}

	// Create the present parameters struct
	D3DPRESENT_PARAMETERS& d3dpp = m_videoInfo->m_d3dPP;
	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.Windowed = TRUE;
	d3dpp.BackBufferFormat = d3dFmt;
	d3dpp.PresentationInterval = (sync) ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE;
	d3dpp.BackBufferWidth  = m_screenDim.x;
	d3dpp.BackBufferHeight = m_screenDim.y;

	// Get caps
	D3DCAPS9& caps = m_videoInfo->m_caps;
	m_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
	m_videoInfo->m_pow2Tex = (caps.TextureCaps & D3DPTEXTURECAPS_POW2);
	m_videoInfo->m_nMaxRTs = static_cast<int>(caps.NumSimultaneousRTs);

	if (!m_videoInfo->m_pow2Tex)
		Message(GS_L("non pow-2 textures aren't supported. You may experience lower graphic quality"), GSMT_INFO);

	m_videoInfo->m_scissorSupported = (caps.RasterCaps & D3DPRASTERCAPS_SCISSORTEST);
	if (!m_videoInfo->m_scissorSupported)
		Message(GS_L("Scissors aren't supported"), GSMT_INFO);

	m_videoInfo->m_magAniso = (caps.TextureFilterCaps  & D3DPTFILTERCAPS_MAGFANISOTROPIC);
	m_videoInfo->m_minAniso = (caps.TextureFilterCaps  & D3DPTFILTERCAPS_MINFANISOTROPIC);
	m_videoInfo->m_maxAniso = caps.MaxAnisotropy;
	m_videoInfo->m_nMaxMultiTex = static_cast<unsigned int>(caps.MaxSimultaneousTextures);
	str_type::stringstream ss;
	ss << GS_L("Maximum number of simultaneous textures: ") << m_videoInfo->m_nMaxMultiTex << std::endl;
	ShowMessage(ss, GSMT_INFO);

	m_videoInfo->m_isVSSupported = (caps.VertexShaderVersion >= D3DVS_VERSION(1,1));

	m_videoInfo->SetupShaderProfile();

	if (!windowed)
	{
		d3dpp.Windowed = FALSE;
		d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	}

	// does it support hardware vertex processing?
	DWORD flags = (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) ? D3DCREATE_HARDWARE_VERTEXPROCESSING : D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	// if the video card doesn't support any vertex shader version,
	// it must have software vertex processing
	if (!m_videoInfo->m_isVSSupported)
		flags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	if (flags & D3DCREATE_HARDWARE_VERTEXPROCESSING)
		Message(GS_L("setting up the window with HARDware vertex processing..."), GSMT_INFO);
	else
		Message(GS_L("setting up the window with SOFTware vertex processing..."), GSMT_INFO);

	if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_videoInfo->m_hWnd,
								flags, &d3dpp, &m_pDevice)))
	{
		if (!windowed)
		{
			const int nMode = m_modes.size()- 1;
			d3dpp.BackBufferFormat = (D3DFORMAT)m_modes[nMode].idx;
			d3dpp.BackBufferWidth  = m_screenDim.x = m_modes[nMode].width;
			d3dpp.BackBufferHeight = m_screenDim.y = m_modes[nMode].height;
		}
		else
		{
			Message(GS_L("The window couldn't be created"));
			m_pD3D->Release();
			m_pD3D = NULL;
			return false;
		}

		if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_videoInfo->m_hWnd,
										flags, &m_videoInfo->m_d3dPP, &m_pDevice)))
		{
			Message(GS_L("The window screen parameters are probably invalid"));
			m_pD3D->Release();
			m_pD3D = NULL;
			return false;
		}
	}

	Message(GS_L("the application started successfuly"), GSMT_INFO);

	m_screenDim.x = m_videoInfo->m_d3dPP.BackBufferWidth;
	m_screenDim.y = m_videoInfo->m_d3dPP.BackBufferHeight;

	ShowWindow(m_videoInfo->m_hWnd, SW_SHOW);
	UpdateWindow(m_videoInfo->m_hWnd);

	m_videoInfo->m_cursor = LoadCursor(NULL, IDC_ARROW);
	SetCursor(m_videoInfo->m_cursor);

	m_windowed = windowed;

	m_videoInfo->v2RestoredSize = m_screenDim;

	m_videoInfo->m_startTime = GetTickCount();
	if (!m_videoInfo->CreateVB(m_pDevice))
	{
		Message(GS_L("The main vertex buffer couldn't be created"));
	}
	m_shaderContext = D3D9CgShaderContextPtr(new D3D9CgShaderContext(m_pDevice));
	m_defaultVS = LoadShaderFromString(GS_L("defaultShader"), gs2dglobal::defaultVSCode, Shader::SF_VERTEX, Shader::SP_MODEL_2, "sprite");
	m_rectVS = LoadShaderFromString(GS_L("rectShader"), gs2dglobal::defaultVSCode, Shader::SF_VERTEX, Shader::SP_MODEL_2, "rectangle");
	m_fastVS = LoadShaderFromString(GS_L("fastShader"), gs2dglobal::fastSimpleVSCode, Shader::SF_VERTEX, Shader::SP_MODEL_2, "fast");
	m_pCurrentVS = m_defaultVS;
	m_defaultVS->SetConstant(GS_L("cameraPos"), GetCameraPos());

	Orthogonal(m_videoInfo->m_orthoMatrix, GetScreenSizeF().x, GetScreenSizeF().y, ZNEAR, ZFAR);
	SetBGColor(gs2d::constant::BLACK);

	m_pDevice->GetRenderTarget(0, &m_videoInfo->m_pBackBuffer);
	SetupShaderViewData(m_pDevice, GetVertexShader(), m_rectVS, m_fastVS);

	SetDefaultRenderStates(m_pDevice, this);
	SetFilterMode(TM_ALWAYS);
	SetClamp(true);
	SetAlphaMode(AM_PIXEL);
	SetZBuffer(false);
	SetZWrite(false);
	return true;
}

unsigned int D3D9Video::GetMaxMultiTextures() const
{
	return m_videoInfo->m_nMaxMultiTex;
}

unsigned int D3D9Video::GetMaxRenderTargets() const
{
	return m_videoInfo->m_nMaxRTs;
}

bool D3D9Video::SetRenderTarget(SpritePtr pTarget, const unsigned int target)
{
	if (!pTarget && target == 0)
	{
		m_pDevice->SetRenderTarget(0, m_videoInfo->m_pBackBuffer);
	}
	else
	{
		IDirect3DSurface9 *pSurface = NULL;
		if (pTarget)
		{
			IDirect3DTexture9 *pTexture;
			try
			{
				pTexture = boost::any_cast<IDirect3DTexture9*>(pTarget->GetTextureObject());
			}
			catch (const boost::bad_any_cast &)
			{
				return false;
			}
			if (!pTexture)
			{
				Message(GS_L("Invalid target - D3D9Video::SetRenderTarget"));
			}
			pTexture->GetSurfaceLevel(0, &pSurface);
		}
		if (FAILED(m_pDevice->SetRenderTarget(static_cast<DWORD>(target), pSurface)))
		{
			Message(GS_L("Invalid target - D3D9Video::SetRenderTarget"));
			return false;
		}
		else if (pSurface)
			pSurface->Release();
	}

	if (target == 0)
	{
		SetupShaderViewData(m_pDevice, GetVertexShader(), m_rectVS, m_fastVS);
	}

	return true;
}

void D3D9Video::EnableMediaPlaying(const bool enable)
{
	m_pDevice->SetDialogBoxMode((enable) ? TRUE : FALSE);
}

void D3D9Video::ResetTimer()
{
	m_videoInfo->m_startTime = GetTickCount();
}

void D3D9Video::Quit()
{
	m_quit = true;
}

unsigned long D3D9Video::GetElapsedTime(const TIME_UNITY unity) const
{
	unsigned long elapsedTime = static_cast<unsigned long>(GetTickCount()-m_videoInfo->m_startTime);
	switch (unity)
	{
	case TU_HOURS:
		elapsedTime /= 1000;
		elapsedTime /= 60;
		elapsedTime /= 60;
		break;
	case TU_MINUTES:
		elapsedTime /= 1000;
		elapsedTime /= 60;
		break;
	case TU_SECONDS:
		elapsedTime /= 1000;
		break;
	case TU_MILLISECONDS:
	default:
		break;
	};

	return elapsedTime;
}


float D3D9Video::GetElapsedTimeF(const TIME_UNITY unity) const
{
	const DWORD delta = GetTickCount()-m_videoInfo->m_startTime;
	float elapsedTime = static_cast<float>(delta);
	switch (unity)
	{
	case TU_HOURS:
		elapsedTime /= 1000.0f;
		elapsedTime /= 60.0f;
		elapsedTime /= 60.0f;
		break;
	case TU_MINUTES:
		elapsedTime /= 1000.0f;
		elapsedTime /= 60.0f;
		break;
	case TU_SECONDS:
		elapsedTime /= 1000.0f;
		break;
	case TU_MILLISECONDS:
	default:
		break;
	};

	return elapsedTime;
}

void D3D9Video::SetDisplayModes(IDirect3D9 *pD3D) const
{
	//retrieves all supported video modes
	const unsigned int n16bit = pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, D3DFMT_R5G6B5);
	const unsigned int n32bit = pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8);
	const unsigned int nVideoModes = n16bit + n32bit;
	D3DDISPLAYMODE mode;
	m_modes.resize(nVideoModes);

	unsigned int t;
	for (t = 0; t < n16bit; t++)
	{
		pD3D->EnumAdapterModes(D3DADAPTER_DEFAULT, D3DFMT_R5G6B5, t, &mode);
		m_modes[t].width  = mode.Width;
		m_modes[t].height = mode.Height;
		m_modes[t].idx = mode.Format;
		m_modes[t].pf = Texture::PF_16BIT;
	}
	for (t = n16bit; t < nVideoModes; t++)
	{
		pD3D->EnumAdapterModes(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8, t-n16bit, &mode);
		m_modes[t].width  = mode.Width;
		m_modes[t].height = mode.Height;
		m_modes[t].idx = mode.Format;
		m_modes[t].pf = Texture::PF_32BIT;
	}

	std::sort(m_modes.begin(), m_modes.end());
	std::vector<VIDEO_MODE>::iterator iter = std::unique(m_modes.begin(), m_modes.end());
	m_modes.resize(iter - m_modes.begin());
}

bool D3D9Video::SetWindowTitle(const str_type::string& title)
{
	m_windowTitle = title;
	if (m_videoInfo->m_d3dPP.Windowed)
		SetWindowTextA(m_videoInfo->m_hWnd, title.c_str());
	return true;
}

str_type::string D3D9Video::GetWindowTitle() const
{
	return m_windowTitle;
}

bool D3D9Video::WindowVisible() const
{
	return m_videoInfo->m_visible;
}

bool D3D9Video::WindowInFocus() const
{
	return m_videoInfo->m_inFocus;
}

Vector2i D3D9Video::GetScreenSize() const
{
	return m_screenDim;
}

Vector2 D3D9Video::GetScreenSizeF() const
{
	return Vector2((float)m_screenDim.x, (float)m_screenDim.y);
}

Vector2i D3D9Video::GetClientScreenSize() const
{
	return Vector2i((int)GetSystemMetrics(SM_CXSCREEN), (int)GetSystemMetrics(SM_CYSCREEN));
}

void D3D9Video::SetWindowPosition(const Vector2i &v2)
{
	if (!m_windowed)
		return;
	m_windowPos = v2;
	MoveWindow(m_videoInfo->m_hWnd, v2.x, v2.y, m_windowedDim.x, m_windowedDim.y, TRUE);
}


Vector2i D3D9Video::GetWindowPosition()
{
	if (!m_windowed)
		return Vector2i(0,0);
	RECT rect;
	GetWindowRect(m_videoInfo->m_hWnd, &rect);
	m_windowPos.x = rect.left;
	m_windowPos.y = rect.top;
	return m_windowPos;
}

Vector2i D3D9Video::ScreenToWindow(const Vector2i &v2Point) const
{
	if (!m_windowed)
		return v2Point;

	POINT point;
	point.x = 0;
	point.y = 0;

	ClientToScreen(m_videoInfo->m_hWnd, &point);

	return Vector2i(v2Point.x-point.x, v2Point.y-point.y);
}

bool D3D9Video::HideCursor(const bool hide)
{
	m_cursorHidden = hide;
	ShowCursor(!m_cursorHidden);
	return true;
}

bool D3D9Video::IsCursorHidden() const
{
	return m_cursorHidden;
}

bool D3D9Video::SaveScreenshot(const str_type::char_t* name, const Texture::BITMAP_FORMAT fmt, Rect2D rect)
{
	IDirect3DSurface9 *pd3dsFront = NULL;
	D3DDISPLAYMODE displayMode;
	m_pDevice->GetDisplayMode(0, &displayMode);

	if (FAILED(m_pDevice->CreateOffscreenPlainSurface(displayMode.Width, displayMode.Height,
		D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pd3dsFront, NULL)))
	{
		Message(GS_L("Failed while creating an offscreen surface - D3D9Video::SaveScreenshot"));
		return false;
	}

	if (SUCCEEDED(m_pDevice->GetFrontBufferData(0, pd3dsFront)))
	{
		const bool cut = (rect.size != Vector2i(0,0));
		RECT wRect = { 0,0,0,0 };
		if (m_windowed)
		{
			GetWindowRect(m_videoInfo->m_hWnd, &wRect);
		}
		if (cut)
		{
			wRect.left = rect.pos.x;
			wRect.top = rect.pos.y;
			wRect.right = rect.pos.x+rect.size.x;
			wRect.bottom = rect.pos.y+rect.size.y;
		}
		str_type::string finalName = name;
		str_type::string ext = GetImageExtension(fmt);
		if (!Platform::IsExtensionRight(finalName, ext))
		{
			finalName += ext;
		}
		if (
			FAILED(D3DXSaveSurfaceToFileA(
				finalName.c_str(), 
				GetD3DPF(fmt), pd3dsFront, NULL, (cut || m_windowed) ? &wRect : NULL)
			)
		)
		{
			Message(GS_L("Couldn't save the screenshot to the disc - D3D9Video::SaveScreenshot"));
			if (pd3dsFront)
				pd3dsFront->Release();
			return false;
		}
	}
	else
	{
		Message(GS_L("Couldn't get front buffer data - D3D9Video::SaveScreenshot"));
		if (pd3dsFront)
			pd3dsFront->Release();
		return false;
	}

	if (pd3dsFront)
		pd3dsFront->Release();

	return true;
}

boost::any D3D9Video::GetGraphicContext()
{
	return m_pDevice;
}

Platform::FileIOHubPtr D3D9Video::GetFileIOHub()
{
	return m_fileIOHub;
}

} // namespace gs2d
