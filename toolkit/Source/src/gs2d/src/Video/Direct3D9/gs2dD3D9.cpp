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

#include "gs2dD3D9.h"
#include "gs2dD3D9Texture.h"
#include "gs2dD3D9Sprite.h"
#include "gs2dD3D9CgShader.h"
#include "../cgShaderCode.h"
#include <vector>
#include "../../Unicode/UTF8Converter.h"

namespace gs2d {
using namespace math;

GS2D_API VideoPtr CreateVideo(
	const unsigned int width,
	const unsigned int height,
	const std::wstring& winTitle,
	const bool windowed,
	const bool sync,
	const Platform::FileIOHubPtr& fileIOHub,
	const GS_PIXEL_FORMAT pfBB,
	const bool maximizable)
{
	return D3D9Video::Create(width, height, winTitle, windowed, sync, pfBB, maximizable, fileIOHub);
}

/// Platform specific user message function implementation
void ShowMessage(std::wstringstream &stream, const GS_MESSAGE_TYPE type)
{
	if (type == GSMT_INFO)
	{
		std::wcout << L"GS2D INFO: " << stream.str() << std::endl;
	}
	else if (type == GSMT_WARNING)
	{
		std::wcerr << L"GS2D WARNING: " << stream.str() << std::endl;
	}
	else if (type == GSMT_ERROR)
	{
		std::wcerr << L"GS2D ERROR: " << stream.str() << std::endl;
		MessageBox(NULL, stream.str().c_str(), L"GS2D ERROR", MB_OK|MB_ICONERROR);
	}
}

/// Shows an error, warning or info message to the user
void ShowMessage(const std::wstring& str, const GS_MESSAGE_TYPE type)
{
	std::wstringstream ss;
	ss << str;
	ShowMessage(ss, type);
}

/// Returns a D3DXIMAGE constant for a GS_BITMAP_FORMAT one
D3DXIMAGE_FILEFORMAT GetD3DPF(const GS_BITMAP_FORMAT fmt)
{
	switch (fmt)
	{
	case GSBF_BMP:
		return D3DXIFF_BMP;
		break;
	case GSBF_JPG:
		return D3DXIFF_JPG;
		break;
	case GSBF_PNG:
		return D3DXIFF_PNG;
		break;
	case GSBF_TGA:
		return D3DXIFF_TGA;
		break;
	case GSBF_DDS:
		return D3DXIFF_DDS;
		break;
	case GSBF_HDR:
		return D3DXIFF_HDR;
		break;
	default:
		return D3DXIFF_BMP;
		break;
	}
}

/// Converts a GS_BITMAP_FORMAT constant to an extension string
std::wstring GetImageExtension(const GS_BITMAP_FORMAT fmt)
{
	switch (fmt)
	{
	case GSBF_JPG:
		return L".jpg";
		break;
	case GSBF_PNG:
		return L".png";
		break;
	case GSBF_TGA:
		return L".tga";
		break;
	case GSBF_DDS:
		return L".dds";
		break;
	case GSBF_HDR:
		return L".hdr";
		break;
	case GSBF_BMP:
	default:
		return L".bmp";
		break;
	}
}

// Returns true if the file name has an explicit extension in its string
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

	pCurrentVS->SetMatrixConstant(L"viewMatrix", ortho);
	pCurrentVS->SetConstant(L"screenSize", Vector2(width, height));

	pRectVS->SetMatrixConstant(L"viewMatrix", ortho);
	pRectVS->SetConstant(L"screenSize", Vector2(width, height));

	pFontVS->SetMatrixConstant(L"viewMatrix", ortho);
	pFontVS->SetConstant(L"screenSize", Vector2(width, height));
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
		pVideo->SetBlendMode(t, GSBM_MODULATE);
	}
	return true;
}

/// Creates an IDirect3D object
IDirect3D9 *CreateAPI()
{
	IDirect3D9* m_pD3D;
	if ((m_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
	{
		std::wstringstream ss;
		ss << L"Couldn't create the IDirect3D9 object";
		ShowMessage(ss, GSMT_ERROR);
		return NULL;
	}
	return m_pD3D;
}

const float D3D9Video::ZFAR = 5.0f;
const float D3D9Video::ZNEAR = 0.0f;
const DWORD D3D9VideoInfo::SPRITE_FVF = (D3DFVF_XYZ|D3DFVF_TEX1);
const DWORD D3D9Video::W32_WINDOWED_STYLE = (WS_CLIPCHILDREN | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE);
const DWORD D3D9Video::W32_FULLSCREEN_STYLE = (WS_CLIPCHILDREN | WS_POPUP);
const DWORD D3D9Video::ALPHAREF = (0x01);
SHORT D3D9Video::m_wheelDelta = 0;
bool D3D9Video::m_inputFocus = false;
TCHAR D3D9Video::m_currentChar = '\0';
D3D9Video::RENDER_TARGET_LIST D3D9Video::m_targets;
const unsigned int D3D9Video::TEXTURE_CHANNELS = 8;

boost::shared_ptr<D3D9Video> D3D9Video::Create(const unsigned int width, const unsigned int height,
											   const std::wstring& winTitle, const bool windowed, const bool sync,
											   const GS_PIXEL_FORMAT pfBB, const bool maximizable,
											   const Platform::FileIOHubPtr& fileIOHub)
{
	boost::shared_ptr<D3D9Video> p(
		new D3D9Video(width, height, winTitle, windowed, sync, pfBB, maximizable, fileIOHub));
	p->weak_this = p;
	return p;
}

TexturePtr D3D9Video::CreateTextureFromFileInMemory(const void *pBuffer, const unsigned int bufferLength, Color mask,
			 const unsigned int width, const unsigned int height, const unsigned int nMipMaps)
{
	TexturePtr texture(new D3D9Texture);
	if (texture->LoadTexture(weak_this, pBuffer, mask, width, height, nMipMaps, bufferLength))
	{
		return texture;
	}
	return TexturePtr();
}

TexturePtr D3D9Video::LoadTextureFromFile(const std::wstring& fileName, Color mask,
			 const unsigned int width, const unsigned int height, const unsigned int nMipMaps)
{
	TexturePtr texture(new D3D9Texture);
	if (texture->LoadTexture(weak_this, fileName, mask, width, height, nMipMaps))
	{
		return texture;
	}
	return TexturePtr();
}

TexturePtr D3D9Video::CreateRenderTargetTexture(const unsigned int width, const unsigned int height, const GS_TARGET_FORMAT fmt)
{
	TexturePtr texture(new D3D9Texture);
	if (texture->CreateRenderTarget(weak_this, width, height, fmt))
	{
		return texture;
	}
	return TexturePtr();
}

SpritePtr D3D9Video::CreateSprite(GS_BYTE *pBuffer, const unsigned int bufferLength,
					Color mask, const unsigned int width, const unsigned int height)
{
	SpritePtr sprite(new D3D9Sprite);
	if (sprite->LoadSprite(weak_this, pBuffer, bufferLength, mask, width, height))
	{
		return sprite;
	}
	return SpritePtr();
}

SpritePtr D3D9Video::CreateSprite(const std::wstring& fileName,
				Color mask, const unsigned int width, const unsigned int height)
{
	SpritePtr sprite(new D3D9Sprite);
	if (sprite->LoadSprite(weak_this, fileName, mask, width, height))
	{
		return sprite;
	}
	return SpritePtr();
}

SpritePtr D3D9Video::CreateRenderTarget(const unsigned int width, const unsigned int height,
						const GS_TARGET_FORMAT format)
{
	SpritePtr sprite(new D3D9Sprite);
	if (sprite->CreateRenderTarget(weak_this, width, height, format))
	{
		return sprite;
	}
	return SpritePtr();
}

ShaderPtr D3D9Video::LoadShaderFromFile(const std::wstring& fileName, const GS_SHADER_FOCUS focus, const GS_SHADER_PROFILE profile,
								const char *entry)
{
	ShaderPtr shader = ShaderPtr(new D3D9CgShader);
	if (shader->LoadShaderFromFile(m_shaderContext, fileName, focus, profile, entry))
	{
		return shader;
	}
	return ShaderPtr();
}

ShaderPtr D3D9Video::LoadShaderFromString(const str_type::string& shaderName, const std::string& codeAsciiString, const GS_SHADER_FOCUS focus, const GS_SHADER_PROFILE profile, const char *entry)
{
	ShaderPtr shader(new D3D9CgShader);
	if (shader->LoadShaderFromString(m_shaderContext, shaderName, codeAsciiString, focus, profile, entry))
	{
		return shader;
	}
	return ShaderPtr();
}

D3D9Video::D3D9Video(const unsigned int width, const unsigned int height,
			const std::wstring& winTitle, const bool windowed, const bool sync,
			const GS_PIXEL_FORMAT pfBB, const bool maximizable, const Platform::FileIOHubPtr& fileIOHub) :
	m_videoInfo(new D3D9VideoInfo),
	m_blendModes(TEXTURE_CHANNELS),
	m_pDevice(NULL),
	m_pD3D(NULL),
	m_rendering(false),
	m_sync(false),
	m_windowed(true),
	m_scissor(Rect2D(0,0,0,0)),
	m_v2Camera(Vector2(0,0)),
	m_alphaMode(GSAM_PIXEL),
	m_textureFilter(GSTM_ALWAYS),
	m_lineWidth(1.0f),
	m_nVideoModes(0),
	m_cursorHidden(false),
	m_quit(false),
	m_windowPos(Vector2i(0,0)),
	m_clamp(true),
	m_depth(0.0f),
	m_fpsRate(60.0f),
	m_roundUpPosition(false),
	m_maximizable(false),
	m_fileIOHub(fileIOHub)
{
	for (unsigned int t = 1; t < TEXTURE_CHANNELS; t++)
		m_blendModes[t] = GSBM_MODULATE;

	m_currentChar = '\0';
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
	UnregisterClassW(m_videoInfo->className.c_str(), m_videoInfo->m_hInstance);
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
				case 0x08:  // backspace 
				case 0x0A:  // linefeed 
				case 0x1B:  // escape 
				case 0x0D:
					break;
				case 0x09:  // tab 
					// Convert tabs to four consecutive spaces. 
					for (int i=0; i<4; i++) 
						SendMessage(hWnd, WM_CHAR, 0x20, 0); 
					return 0; 
 				default:	// displayable character 
					m_currentChar = (TCHAR)wParam;
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

void D3D9Video::RoundUpPosition(const bool roundUp)
{
	m_roundUpPosition = roundUp;
}
bool D3D9Video::IsRoundingUpPosition() const
{
	return m_roundUpPosition;
}

void D3D9Video::Message(const std::wstring& text, const GS_MESSAGE_TYPE type) const
{
	std::wstringstream ss;
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
			Message(L"IDirect3DDevice9::Clear has failed", GSMT_ERROR);
			return false;
		}
	}
	if (FAILED(m_pDevice->BeginScene()))
	{
		Message(L"IDirect3DDevice9::BeginScene has failed", GSMT_ERROR);
		return false;
	}
	m_rendering = true;
	return true;
}

bool D3D9Video::EndScene(const bool swap)
{
	if (FAILED(m_pDevice->EndScene()))
	{
		Message(L"IDirect3DDevice9::EndScene has failed", GSMT_ERROR);
	}
	if (swap)
	{
		HRESULT hr = m_pDevice->Present(NULL, NULL, NULL, NULL);
		if (hr == D3DERR_DEVICELOST)
			m_videoInfo->m_deviceLost = true;
		else if (FAILED(hr))
		{
			Message(L"IDirect3DDevice9::Present has failed", GSMT_ERROR);
		}
	}

	m_rendering = false;
	return true;
}

bool D3D9Video::BeginSpriteScene(const Color& bgColor)
{
	if (!(BeginScene(bgColor)))
	{
		Message(L"IDirect3DDevice9::BeginScene has failed");
	}
	m_pDevice->SetRenderState(D3DRS_AMBIENT, gs2d::constant::WHITE);
	m_pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	for (unsigned int t=0; t<TEXTURE_CHANNELS; t++)
		m_pDevice->SetSamplerState(t, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	SetAlphaMode(GSAM_PIXEL);
	return true;
}

bool D3D9Video::EndSpriteScene()
{
	m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	SetAlphaMode(GSAM_NONE);
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
		Message(L"IDirect3DDevice9::BeginScene has failed");
	}
	m_pDevice->SetRenderState(D3DRS_AMBIENT, gs2d::constant::WHITE);
	for (unsigned int t=0; t<TEXTURE_CHANNELS; t++)
		m_pDevice->SetSamplerState(t, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	SetAlphaMode(GSAM_PIXEL);
	return true;
}

bool D3D9Video::EndTargetScene()
{
	m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	SetAlphaMode(GSAM_NONE);
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

bool D3D9Video::ManageLoop()
{
	if (Rendering())
		EndSpriteScene();

	APP_STATUS status = APP_SKIP;
	while (status == APP_SKIP)
	{
		status = HandleEvents();
		if (status == APP_QUIT)
			return false;
	}

	if (!Rendering())
		BeginSpriteScene();

	return true;
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
		Message(L"Hardware scissors are not supported - D3D9Video::SetScissor");
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

bool D3D9Video::SetAlphaMode(const GS_ALPHA_MODE mode)
{
	m_alphaMode = mode;
	unsigned int t;

	switch(mode)
	{
	case GSAM_PIXEL:
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
	case GSAM_ADD:
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
	case GSAM_MODULATE:
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
	case GSAM_ALPHA_TEST:
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		m_pDevice->SetRenderState(D3DRS_ALPHAREF, ALPHAREF);
		m_pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
		break;
	case GSAM_NONE:
	default:
		m_alphaMode = GSAM_NONE;
		m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		break;
	};

	return true;
}

GS_ALPHA_MODE D3D9Video::GetAlphaMode() const
{
	return m_alphaMode;
}

bool D3D9Video::SetSpriteDepth(const float depth)
{
	m_depth = depth;
	if (m_depth > 1.0f || m_depth < 0.0f)
	{
		Message(L"Warning: the depth must range 0.0f - 1.0f - D3D9Video::SetSpriteDepth", GSMT_WARNING);
		m_depth = Max(m_depth, 0.0f);
		m_depth = Min(m_depth, 1.0f);
	}
	return true;
}
float D3D9Video::GetSpriteDepth() const
{
	return m_depth;
}

bool D3D9Video::SetFilterMode(const GS_TEXTUREFILTER_MODE tfm)
{
	m_textureFilter = tfm;
	if (GetFilterMode() != GSTM_NEVER)
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

GS_TEXTUREFILTER_MODE D3D9Video::GetFilterMode() const
{
	return m_textureFilter;
}

bool D3D9Video::Rendering() const
{
	return m_rendering;
}

bool D3D9Video::ResetVideoMode(const unsigned int width, const unsigned int height, const GS_PIXEL_FORMAT pfBB, const bool toggleFullscreen)
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
	const GS_ALPHA_MODE alphaMode = GetAlphaMode();
	const GS_TEXTUREFILTER_MODE filterMode = GetFilterMode();
	std::vector<GS_BLEND_MODE> blendModes(GetMaxMultiTextures(), GSBM_MODULATE);
	for (unsigned int t=1; t<blendModes.size(); t++)
	{
		blendModes[t] = GetBlendMode(t);
	}

	if (rendering)
	{
		Message(L"scene ended", GSMT_INFO);
		EndSpriteScene();
	}
	Message(L"starting device reset", GSMT_INFO);

	// Prepare render targets before reseting the device
	if (!m_targets.empty())
	{
		Message(L"deleting render targets...", GSMT_INFO);
		RENDER_TARGET_LIST::iterator iter;
		for (iter = m_targets.begin(); iter != m_targets.end(); ++iter)
		{
			Sprite* sprite = iter->pSprite;
			if (sprite->GetType() != Sprite::T_NOT_LOADED)
				sprite->OnLostDevice();
		}
		Message(L"render targets deleted", GSMT_INFO);
	}

	// Destroy the backbuffer
	if (m_videoInfo->m_pBackBuffer)
	{
		m_videoInfo->m_pBackBuffer->Release();
		m_videoInfo->m_pBackBuffer = NULL;
		Message(L"backbuffer released", GSMT_INFO);
	}

	// if the mode is toggled, do the window properties changing
	D3DPRESENT_PARAMETERS& d3dpp = m_videoInfo->m_d3dPP;
	if (toggleFullscreen)
	{
		Message(L"trying to toggle fullscreen", GSMT_INFO);
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
		Message(L"recreating the D3DPP...", GSMT_INFO);
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
		Message(L"Couldn't reset the video mode");
	}
	else
	{
		Message(L"device successfully reset", GSMT_INFO);
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
			Message(L"targets recovered from backup (if possible)", GSMT_INFO);
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
		Message(L"begining scene rendering", GSMT_INFO);
		BeginSpriteScene();
	}
	Message(L"device successfully reset", GSMT_INFO);

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
			ResetVideoMode(resize.x, resize.y, GSPF_UNKNOWN, false);
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
		// As all textures are sotre using D3DPOOL_MANAGED, we don't need to
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
					ResetVideoMode(0, 0, GSPF_UNKNOWN);
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
		if (pShader->GetShaderFocus() != GSSF_VERTEX)
		{
			Message(L"The shader set is not a vertex program - D3D9Video::SetVertexShader");
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
	if (pShader->GetShaderFocus() != GSSF_PIXEL)
	{
		Message(L"The shader set is not a vertex program - D3D9Video::SetPixelShader");
		return false;
	}
	return true;
}

GS_SHADER_PROFILE D3D9Video::GetHighestVertexProfile() const
{
	return m_videoInfo->m_lastVertexProfile;
}

GS_SHADER_PROFILE D3D9Video::GetHighestPixelProfile() const
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

std::wstring GetFileName(const std::wstring& source)
{
	std::wstring::size_type length = source.length();
	std::wstring::size_type t = length-1;
	for (; t > 0; --t)
	{
		if (source[t] == L'\\' || source[t] == L'/')
			break;
	}
	return source.substr(++t);
}

BitmapFontPtr D3D9Video::LoadBitmapFont(const std::wstring& fullFilePath)
{
	str_type::ifstream stream(fullFilePath.c_str(), str_type::ifstream::in);
	if (stream.is_open())
	{
		str_type::ostringstream out;
		out << stream.rdbuf();

		BitmapFontPtr newFont = BitmapFontPtr(new BitmapFont(weak_this, fullFilePath, out.str()));
		if (newFont->IsLoaded())
		{
			m_fonts[GetFileName(fullFilePath)] = newFont;
			return newFont;
		}
		else
		{
			std::wstring errorMsg = L"Invalid font file ";
			errorMsg += fullFilePath;
			Message(errorMsg, GSMT_WARNING);
			return BitmapFontPtr();
		}
	}
	else
	{
		std::wstring errorMsg = L"Font file not found ";
		errorMsg += fullFilePath;
		Message(errorMsg, GSMT_WARNING);
		return BitmapFontPtr();
	}
}

BitmapFontPtr D3D9Video::SeekBitmapFont(const str_type::string& font)
{
	std::map<str_type::string, BitmapFontPtr>::iterator iter = m_fonts.find(font);
	BitmapFontPtr bitmapFont;
	if (iter == m_fonts.end())
	{
		bitmapFont = LoadBitmapFont(m_fileIOHub->GenerateBitmapFontFilePath(font));
		if (!bitmapFont)
		{
			Message(font + GS_L(": couldn't create bitmap font"), GSMT_ERROR);
		}
	}
	else
	{
		bitmapFont = iter->second;
	}
	return bitmapFont;	
}

Vector2 D3D9Video::ComputeCarretPosition(const std::wstring& font, const std::wstring& text, const unsigned int pos)
{
	BitmapFontPtr bitmapFont = SeekBitmapFont(font);
	if (bitmapFont)
		return bitmapFont->ComputeCarretPosition(text, pos);
	else
		return Vector2(0,0);
}

Vector2 D3D9Video::ComputeTextBoxSize(const std::wstring& font, const std::wstring& text)
{
	BitmapFontPtr bitmapFont = SeekBitmapFont(font);
	if (bitmapFont)
		return bitmapFont->ComputeTextBoxSize(text);
	else
		return Vector2(0,0);
}

unsigned int D3D9Video::FindClosestCarretPosition(const std::wstring& font, const std::wstring &text, const Vector2 &textPos, const Vector2 &reference)
{
	BitmapFontPtr bitmapFont = SeekBitmapFont(font);
	if (bitmapFont)
		return bitmapFont->FindClosestCarretPosition(text, textPos, reference);
	else
		return 0;
}

bool D3D9Video::DrawBitmapText(const Vector2 &v2Pos, const std::wstring& text, const std::wstring& font, const Color& color, const float scale)
{
	BitmapFontPtr bitmapFont = SeekBitmapFont(font);
	if (bitmapFont)
	{
		bitmapFont->DrawBitmapText(v2Pos, text, color, scale);
		return true;
	}
	else
	{
		return false;
	}
}

bool D3D9Video::UnsetTexture(const unsigned int passIdx)
{
	m_pDevice->SetTexture(passIdx, NULL);
	return true;
}

bool D3D9Video::SetBlendMode(const unsigned int passIdx, const GS_BLEND_MODE mode)
{
	if (passIdx <= 0)
	{
		Message(L"nPass can't be less or equal to 0 - D3D9Video::SetBlendMode");
		return false;
	}
	m_pDevice->SetTextureStageState(passIdx, D3DTSS_COLOROP, (mode==GSBM_ADD) ? D3DTOP_ADD : D3DTOP_MODULATE);
	m_blendModes[passIdx] = mode;
	return true;
}

GS_BLEND_MODE D3D9Video::GetBlendMode(const unsigned int passIdx) const
{
	return m_blendModes[passIdx];
}

bool D3D9Video::DrawLine(const Vector2 &p1, const Vector2 &p2, const Color& color1, const Color& color2)
{
	if (m_lineWidth <= 1.0f)
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
			Message(L"The line can't be a point (a can't be == b) - D3D9Video::DrawLine", GSMT_WARNING);
			return false;
		}
		const Vector2 v2Dir = a - b;
		const float len = Distance(p1, p2);
		const float angle = RadianToDegree(GetAngle(v2Dir));

		DrawRectangle(p1, Vector2(m_lineWidth, len),
					  color2, color2, color1, color1,
					  angle, GSEO_CENTER_BOTTOM);
	}

	return true;
}

void D3D9Video::SetLineWidth(const float width)
{
	m_lineWidth = (width < 1.0f) ? 1.0f : width;
}


float D3D9Video::GetLineWidth() const
{
	return m_lineWidth;
}

bool D3D9Video::DrawRectangle(const Vector2 &v2Pos, const Vector2 &v2Size,
						const Color& color, const float angle, const GS_ENTITY_ORIGIN origin)
{
	return DrawRectangle(v2Pos, v2Size,	color, color, color, color, angle, origin);
}

bool D3D9Video::DrawRectangle(const Vector2 &v2Pos, const Vector2 &v2Size,
						const Color& color0, const Color& color1, const Color& color2, const Color& color3,
						const float angle, const GS_ENTITY_ORIGIN origin)
{
	if (v2Size == Vector2(0,0))
	{
		Message(L"Invalid size argument - D3D9Video::DrawRectangle");
		return false;
	}

	Vector2 v2Center;
	switch (origin)
	{
	case GSEO_CENTER:
	case GSEO_RECT_CENTER:
		v2Center.x = v2Size.x/2.0f;
		v2Center.y = v2Size.y/2.0f;
		break;
	case GSEO_RECT_CENTER_BOTTOM:
	case GSEO_CENTER_BOTTOM:
		v2Center.x = v2Size.x/2.0f;
		v2Center.y = v2Size.y;
		break;
	case GSEO_RECT_CENTER_TOP:
	case GSEO_CENTER_TOP:
		v2Center.x = v2Size.x/2.0f;
		v2Center.y = 0.0f;
		break;
	case GSEO_DEFAULT:
	default:
		v2Center.x = 0.0f;
		v2Center.y = 0.0f;
		break;
	};

	Matrix4x4 mRot;
	if (angle != 0.0f)
		mRot = RotateZ(DegreeToRadian(angle));
	m_rectVS->SetMatrixConstant(L"rotationMatrix", mRot);
	m_rectVS->SetConstant(L"size", v2Size);
	m_rectVS->SetConstant(L"entityPos", v2Pos);
	m_rectVS->SetConstant(L"center", v2Center);
	m_rectVS->SetConstant(L"color0", color0);
	m_rectVS->SetConstant(L"color1", color1);
	m_rectVS->SetConstant(L"color2", color2);
	m_rectVS->SetConstant(L"color3", color3);

	ShaderPtr vertexShader = GetVertexShader(), pixelShader = GetPixelShader();

	m_rectVS->SetShader();
	SetPixelShader(ShaderPtr());

	for (unsigned int t=0; t<TEXTURE_CHANNELS; t++)
		m_pDevice->SetTexture(t, NULL);

	if (!m_videoInfo->DrawSprite(m_pDevice, GSRM_TWO_TRIANGLES))
	{
		Message(L"Rendering failed - D3D9Video::DrawRectangle");
		return false;
	}

	SetPixelShader(pixelShader);
	SetVertexShader(vertexShader);

	return true;
}

Video::VIDEO_MODE D3D9Video::GetVideoMode(const unsigned int modeIdx) const
{
	if (modeIdx >= m_nVideoModes)
	{
		VIDEO_MODE mode;
		mode.width = mode.height = 0;
		mode.pf = GSPF_UNKNOWN;
		mode.idx = 0x0;

		Message(L"The selected video mode doesn't exist - D3D9Video::GetVideoMode");
		return mode;
	}
	return m_modes[modeIdx];
}

unsigned int D3D9Video::GetVideoModeCount()
{
	if (!m_pD3D)
	{
		if ((m_pD3D = CreateAPI()) == NULL)
		{
			Message(L"Couldn't create the IDirect3D9 object - D3D9Video::GetVideoModeCount");
			return 0;
		}
		SetDisplayModes(m_pD3D);
	}
	return m_nVideoModes;
}

bool D3D9Video::StartApplication(const unsigned int width, const unsigned int height,
								const std::wstring& winTitle, const bool windowed,
								const bool sync, const GS_PIXEL_FORMAT pfBB, const bool maximizable)
{
	m_sync = sync;

	// open window
	WNDCLASSEXW wc = { sizeof(WNDCLASSEX), 0, MsgProc, 0, 0,
						GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
						winTitle.c_str(), NULL };
	m_videoInfo->m_hInstance = wc.hInstance;
	m_videoInfo->className = winTitle;

	RegisterClassExW(&wc);

	m_windowTitle = winTitle;
	m_screenDim.x = width;
	m_screenDim.y = height;
	m_maximizable = maximizable;

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

	m_videoInfo->m_hWnd= CreateWindowW(m_videoInfo->className.c_str(), winTitle.c_str(),
							m_videoInfo->m_windowStyle, m_windowPos.x, m_windowPos.y, m_windowedDim.x, m_windowedDim.y,
							GetDesktopWindow(), NULL, wc.hInstance, m_videoInfo.get());

	if (!m_pD3D)
	{
		if ((m_pD3D = CreateAPI()) == NULL)
		{
			Message(L"Couldn't create the IDirect3D9 object - D3D9Video::StartApplication");
			return false;
		}
		SetDisplayModes(m_pD3D);
		Message(L"creating the API object IDirect3D9", GSMT_INFO);
	}

	// convert from GSlib format to D3D_FORMAT
	D3DFORMAT d3dFmt;
	switch (pfBB)
	{
	case GSPF_32BIT:
		d3dFmt = D3DFMT_X8R8G8B8;
		break;
	case GSPF_16BIT:
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
		Message(L"Couldn't get the display mode - D3D9Video::StartApplication");
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
		Message(L"The back buffer format had to be changed to default - D3D9Video::StartApplication");
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
		Message(L"non pow-2 textures aren't supported. You may experience lower graphic quality", GSMT_INFO);

	m_videoInfo->m_scissorSupported = (caps.RasterCaps & D3DPRASTERCAPS_SCISSORTEST);
	if (!m_videoInfo->m_scissorSupported)
		Message(L"Scissors aren't supported", GSMT_INFO);

	m_videoInfo->m_magAniso = (caps.TextureFilterCaps  & D3DPTFILTERCAPS_MAGFANISOTROPIC);
	m_videoInfo->m_minAniso = (caps.TextureFilterCaps  & D3DPTFILTERCAPS_MINFANISOTROPIC);
	m_videoInfo->m_maxAniso = caps.MaxAnisotropy;
	m_videoInfo->m_nMaxMultiTex = static_cast<unsigned int>(caps.MaxSimultaneousTextures);
	std::wstringstream ss;
	ss << L"Maximum number of simultaneous textures: " << m_videoInfo->m_nMaxMultiTex << std::endl;
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
		Message(L"setting up the window with HARDware vertex processing...", GSMT_INFO);
	else
		Message(L"setting up the window with SOFTware vertex processing...", GSMT_INFO);

	if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_videoInfo->m_hWnd,
								flags, &d3dpp, &m_pDevice)))
	{
		if (!windowed)
		{
			const int nMode = m_nVideoModes/3;
			d3dpp.BackBufferFormat = (D3DFORMAT)m_modes[nMode].idx;
			d3dpp.BackBufferWidth  = m_screenDim.x = m_modes[nMode].width;
			d3dpp.BackBufferHeight = m_screenDim.y = m_modes[nMode].height;
		}
		else
		{
			Message(L"The window couldn't be created");
			m_pD3D->Release();
			m_pD3D = NULL;
			return false;
		}

		if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_videoInfo->m_hWnd,
										flags, &m_videoInfo->m_d3dPP, &m_pDevice)))
		{
			Message(L"The window screen parameters are probably invalid");
			m_pD3D->Release();
			m_pD3D = NULL;
			return false;
		}
	}

	Message(L"the application started successfuly", GSMT_INFO);

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
		Message(L"The main vertex buffer couldn't be created");
	}
	m_shaderContext = D3D9CgShaderContextPtr(new D3D9CgShaderContext(m_pDevice));
	m_defaultVS = LoadShaderFromString(L"defaultShader", gs2dglobal::defaultVSCode, GSSF_VERTEX, GSSP_MODEL_2, "sprite");
	m_rectVS = LoadShaderFromString(L"rectShader", gs2dglobal::defaultVSCode, GSSF_VERTEX, GSSP_MODEL_2, "rectangle");
	m_fastVS = LoadShaderFromString(L"fastShader", gs2dglobal::fastSimpleVSCode, GSSF_VERTEX, GSSP_MODEL_2, "fast");
	m_pCurrentVS = m_defaultVS;
	m_defaultVS->SetConstant(L"cameraPos", GetCameraPos());

	Orthogonal(m_videoInfo->m_orthoMatrix, GetScreenSizeF().x, GetScreenSizeF().y, ZNEAR, ZFAR);
	SetBGColor(gs2d::constant::BLACK);

	m_pDevice->GetRenderTarget(0, &m_videoInfo->m_pBackBuffer);
	SetupShaderViewData(m_pDevice, GetVertexShader(), m_rectVS, m_fastVS);

	SetDefaultRenderStates(m_pDevice, this);
	SetFilterMode(GSTM_ALWAYS);
	SetClamp(true);
	SetAlphaMode(GSAM_PIXEL);
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
				Message(L"Invalid target - D3D9Video::SetRenderTarget");
			}
			pTexture->GetSurfaceLevel(0, &pSurface);
		}
		if (FAILED(m_pDevice->SetRenderTarget(static_cast<DWORD>(target), pSurface)))
		{
			Message(L"Invalid target - D3D9Video::SetRenderTarget");
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

void D3D9Video::SetDisplayModes(IDirect3D9 *pD3D)
{
	//retrieves all supported vidoe modes
	const unsigned int n16bit = pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, D3DFMT_R5G6B5);
	const unsigned int n32bit = pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8);
	m_nVideoModes = n16bit+n32bit;
	D3DDISPLAYMODE mode;
	m_modes.resize(m_nVideoModes);

	unsigned int t;
	for (t = 0; t < n16bit; t++)
	{
		pD3D->EnumAdapterModes(D3DADAPTER_DEFAULT, D3DFMT_R5G6B5, t, &mode);
		m_modes[t].width  = mode.Width;
		m_modes[t].height = mode.Height;
		m_modes[t].idx = mode.Format;
		m_modes[t].pf = GSPF_16BIT;
	}
	for (t=n16bit; t<m_nVideoModes; t++)
	{
		pD3D->EnumAdapterModes(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8, t-n16bit, &mode);
		m_modes[t].width  = mode.Width;
		m_modes[t].height = mode.Height;
		m_modes[t].idx = mode.Format;
		m_modes[t].pf = GSPF_32BIT;
	}

	std::sort(m_modes.begin(), m_modes.end());
	std::vector<VIDEO_MODE>::iterator iter = std::unique(m_modes.begin(), m_modes.end());
	m_modes.resize(iter - m_modes.begin());
}

bool D3D9Video::SetWindowTitle(const std::wstring& title)
{
	m_windowTitle = title;
	if (m_videoInfo->m_d3dPP.Windowed)
		SetWindowTextW(m_videoInfo->m_hWnd, title.c_str());
	return true;
}

std::wstring D3D9Video::GetWindowTitle() const
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

bool D3D9Video::SetCameraPos(const Vector2 &pos)
{
	m_v2Camera = pos;
	return true;
}

bool D3D9Video::MoveCamera(const Vector2 &dir)
{
	SetCameraPos(m_v2Camera + dir);
	return true;
}

Vector2 D3D9Video::GetCameraPos() const
{
	if (IsRoundingUpPosition())
	{
		return Vector2(floor(m_v2Camera.x), floor(m_v2Camera.y));
	}
	else
	{
		return m_v2Camera;
	}
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

bool D3D9Video::SaveScreenshot(const wchar_t *wcsName, const GS_BITMAP_FORMAT fmt, Rect2D rect)
{
	IDirect3DSurface9 *pd3dsFront = NULL;
	D3DDISPLAYMODE displayMode;
	m_pDevice->GetDisplayMode(0, &displayMode);

	if (FAILED(m_pDevice->CreateOffscreenPlainSurface(displayMode.Width, displayMode.Height,
		D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pd3dsFront, NULL)))
	{
		Message(L"Failed while creating an offscreen surface - D3D9Video::SaveScreenshot");
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
		std::wstring finalName = wcsName;
		std::wstring ext = GetImageExtension(fmt);
		if (!IsTheExtensionRight(finalName.c_str(), ext.c_str()))
		{
			finalName += ext;
		}
		if (
			FAILED(D3DXSaveSurfaceToFile(
				finalName.c_str(), 
				GetD3DPF(fmt), pd3dsFront, NULL, (cut || m_windowed) ? &wRect : NULL)
			)
		)
		{
			Message(L"Couldn't save the screenshot to the disc - D3D9Video::SaveScreenshot");
			if (pd3dsFront)
				pd3dsFront->Release();
			return false;
		}
	}
	else
	{
		Message(L"Couldn't get front buffer data - D3D9Video::SaveScreenshot");
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

str_type::string D3D9Video::GetPlatformName() const
{
	return GS_L("windows");
}

Platform::FileIOHubPtr D3D9Video::GetFileIOHub()
{
	return m_fileIOHub;
}

} // namespace gs2d
