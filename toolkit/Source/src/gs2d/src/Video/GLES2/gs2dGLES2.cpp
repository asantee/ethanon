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

#include "gs2dGLES2.h"

#ifdef APPLE_IOS
  #include <OpenGLES/ES2/gl.h>
  #include <OpenGLES/ES2/glext.h>
  #include "../../Platform/ios/Platform.ios.h"
#endif

#ifdef ANDROID
  #include <GLES2/gl2.h>
  #include <GLES2/gl2ext.h>
  #include "../../Platform/android/Platform.android.h"
#endif

#include "../../Shader.h"
#include "gs2dGLES2Shader.h"
#include "gs2dGLES2Sprite.h"

#include <sstream>

namespace gs2d {

using namespace math;

const float GLES2Video::ZFAR = 5.0f;
const float GLES2Video::ZNEAR = 0.0f;
const str_type::string GLES2Video::VIDEO_LOG_FILE("GLES2Video.log.txt");
const unsigned long GLES2Video::ALPHAREF = (0x01);

void UnbindFrameBuffer()
{
	GLuint idx;
#	ifdef APPLE_IOS
		idx = 1;
#	else
		idx = 0;
#	endif
	glBindFramebuffer(GL_FRAMEBUFFER, idx);
}

GLES2ShaderPtr LoadInternalShader(GLES2Video* video, const str_type::string& str, const GS_SHADER_FOCUS focus)
{
	GLES2ShaderPtr shader = video->LoadGLES2ShaderFromFile(str, focus);
	if (shader)
	{
		video->Message(str + " default shader created successfully", GSMT_INFO);
	}
	else
	{
		video->Message(str + " couldn't create default shader", GSMT_ERROR);
	}
	return shader;
}

GS2D_API VideoPtr CreateVideo(const unsigned int width, const unsigned int height, const Platform::FileIOHubPtr& fileIOHub)
{
	return GLES2Video::Create(width, height, GS_L("GS2D"), fileIOHub);
}

GLES2Video::GLES2Video(
	const unsigned int width,
	const unsigned int height,
	const str_type::string& winTitle,
	const Platform::FileIOHubPtr& fileIOHub) :
	m_backgroundColor(gs2d::constant::BLACK),
	m_screenSize(width, height),
	m_windowTitle(winTitle),
	m_quit(false),
	m_rendering(false),
	m_logger(Platform::FileLogger::GetLogDirectory() + VIDEO_LOG_FILE),
	m_fpsRate(30.0f),
	m_roundUpPosition(false),
	m_scissor(Vector2i(0, 0), Vector2i(0, 0)),
	m_textureFilterMode(GSTM_IFNEEDED),
	m_blend(false),
	m_zBuffer(true),
	m_zWrite(true),
	m_fileIOHub(fileIOHub)
{
	for (std::size_t t = 0; t < _GS2D_GLES2_MAX_MULTI_TEXTURES; t++)
	{
		m_blendModes[t] = GSBM_MODULATE;
	}

	ResetTimer();
	m_logger.Log("Creating shader context...", Platform::FileLogger::INFO);
	m_shaderContext = GLES2ShaderContextPtr(new GLES2ShaderContext(this));
	m_logger.Log("StartApplication...", Platform::FileLogger::INFO);
	StartApplication(width, height, winTitle, false, false, GSPF_DEFAULT, false);
}

boost::shared_ptr<GLES2Video> GLES2Video::Create(
	const unsigned int width,
	const unsigned int height,
	const str_type::string& winTitle,
	const Platform::FileIOHubPtr& fileIOHub)
{
	boost::shared_ptr<GLES2Video> p(new GLES2Video(width, height, winTitle, fileIOHub));
	p->weak_this = p;
	return p;
}

static bool HasFragmentShaderMaximumPrecision()
{
	GLint range[2], precision[1];
	glGetShaderPrecisionFormat(GL_FRAGMENT_SHADER, GL_HIGH_FLOAT, range, precision);
	return (range[0] != 0 && range[1] != 0 && precision[0] != 0);
}

static void LogFragmentShaderMaximumPrecision(const Platform::FileLogger& logger)
{
	const bool precisionResult = HasFragmentShaderMaximumPrecision();
	const str_type::string logStr = (precisionResult)
		? GS_L("High floating point fragment shader precision supported")
		: GS_L("High floating point fragment shader precision is not supported");
	logger.Log(logStr, (precisionResult) ? Platform::FileLogger::INFO : Platform::FileLogger::WARNING);
}

bool GLES2Video::StartApplication(
	const unsigned int width,
	const unsigned int height,
	const str_type::string& winTitle,
	const bool windowed,
	const bool sync,
	const GS_PIXEL_FORMAT pfBB,
	const bool maximizable)
{
	glDisable(GL_DITHER);
	glHint(GL_GENERATE_MIPMAP_HINT, GL_FASTEST);

	m_defaultVS = LoadInternalShader(this, "assets/shaders/default/default.vs", GSSF_VERTEX);
	m_defaultPS = LoadInternalShader(this, "assets/shaders/default/default.ps", GSSF_PIXEL);
	m_fastRenderVS = LoadInternalShader(this, "assets/shaders/default/fastRender.vs", GSSF_VERTEX);
	m_optimalVS = LoadInternalShader(this, "assets/shaders/default/optimal.vs", GSSF_VERTEX);
	m_modulate1 = LoadInternalShader(this, "assets/shaders/default/modulate1.ps", GSSF_PIXEL);
	m_add1 = LoadInternalShader(this, "assets/shaders/default/add1.ps", GSSF_PIXEL);

	// forces shader pre-load to avoid runtime lag
	m_shaderContext->SetShader(m_defaultVS,		m_defaultPS, m_orthoMatrix, GetScreenSizeF());
	m_shaderContext->SetShader(m_defaultVS,		m_modulate1, m_orthoMatrix, GetScreenSizeF());
	m_shaderContext->SetShader(m_defaultVS,		m_add1,      m_orthoMatrix, GetScreenSizeF());
	m_shaderContext->SetShader(m_fastRenderVS,	m_defaultPS, m_orthoMatrix, GetScreenSizeF());
	m_shaderContext->SetShader(m_optimalVS,		m_modulate1, m_orthoMatrix, GetScreenSizeF());
	m_shaderContext->SetShader(m_optimalVS,		m_add1,      m_orthoMatrix, GetScreenSizeF());
	m_shaderContext->SetShader(m_optimalVS,		m_defaultPS, m_orthoMatrix, GetScreenSizeF());

	LogFragmentShaderMaximumPrecision(m_logger);

	SetZBuffer(false);
	SetZWrite(false);
	SetFilterMode(GSTM_IFNEEDED);
	UnsetScissor();

	ResetVideoMode(width, height, pfBB, false);
	m_logger.Log("Application started...", Platform::FileLogger::INFO);
	return true;
}

void GLES2Video::Enable2D(const int width, const int height, const bool flipY)
{
	glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
	glDisable(GL_CULL_FACE);
	Orthogonal(m_orthoMatrix, static_cast<float>(width), static_cast<float>(height) * ((flipY) ? -1.0f : 1.0f), ZNEAR, ZFAR);

	glDepthFunc(GL_LEQUAL);
	glDepthRangef(0.0f, 1.0f);
}

void GLES2Video::CheckGLError(const str_type::string& op, const Platform::FileLogger& logger)
{
	for (GLint error = glGetError(); error; error = glGetError())
	{
		std::stringstream ss;
		ss << "ERROR: after " << op << ". Error code " << error;
		logger.Log(ss.str(), Platform::FileLogger::ERROR);
	}
}

TexturePtr GLES2Video::CreateTextureFromFileInMemory(
	const void *pBuffer,
	const unsigned int bufferLength,
	Color mask,
	const unsigned int width,
	const unsigned int height,
	const unsigned int nMipMaps)
{
	TexturePtr texture(new GLES2Texture(weak_this, GS_L("from_memory"), m_fileIOHub->GetFileManager()));
	if (texture->LoadTexture(weak_this, pBuffer, bufferLength, mask, width, height, nMipMaps))
	{
		return texture;
	}
	return TexturePtr();
}

TexturePtr GLES2Video::LoadTextureFromFile(
	const str_type::string& fileName,
	Color mask,
	const unsigned int width,
	const unsigned int height,
	const unsigned int nMipMaps)
{
	TexturePtr texture(new GLES2Texture(weak_this, fileName, m_fileIOHub->GetFileManager()));
	if (texture->LoadTexture(weak_this, fileName, mask, width, height, nMipMaps))
	{
		return texture;
	}
	return TexturePtr();
}

TexturePtr GLES2Video::CreateRenderTargetTexture(const unsigned int width, const unsigned int height, const GS_TARGET_FORMAT fmt)
{
	TexturePtr texture(new GLES2Texture(weak_this, GS_L("render_target"), m_fileIOHub->GetFileManager()));
	if (texture->CreateRenderTarget(weak_this, width, height, fmt))
	{
		return texture;
	}
	return TexturePtr();
}

SpritePtr GLES2Video::CreateSprite(
	GS_BYTE *pBuffer,
	const unsigned int bufferLength,
	Color mask,
	const unsigned int width,
	const unsigned int height)
{
	// TODO
	return SpritePtr();
}

SpritePtr GLES2Video::CreateSprite(
	const str_type::string& fileName,
	Color mask,
	const unsigned int width,
	const unsigned int height)
{
	SpritePtr sprite(new GLES2Sprite(m_shaderContext));
	if (sprite->LoadSprite(weak_this, fileName, mask, width, height))
	{
		return sprite;
	}
	return SpritePtr();
}

SpritePtr GLES2Video::CreateRenderTarget(
	const unsigned int width,
	const unsigned int height,
	const GS_TARGET_FORMAT format)
{
	SpritePtr sprite(new GLES2Sprite(m_shaderContext));
	if (sprite->CreateRenderTarget(weak_this, width, height, format))
	{
		return sprite;
	}
	return SpritePtr();
}

ShaderPtr GLES2Video::LoadShaderFromFile(
	const str_type::string& fileName,
	const GS_SHADER_FOCUS focus,
	const GS_SHADER_PROFILE profile,
	const char *entry)
{
	GLES2ShaderPtr shader(new GLES2Shader(m_fileIOHub->GetFileManager(), m_shaderContext));
	if (shader->LoadShaderFromFile(m_shaderContext, fileName, focus, profile, entry))
	{
		return shader;
	}
	return ShaderPtr();
}

ShaderPtr GLES2Video::LoadShaderFromString(
	const str_type::string& shaderName,
	const std::string& codeAsciiString,
	const GS_SHADER_FOCUS focus,
	const GS_SHADER_PROFILE profile,
	const char *entry)
{
	GLES2ShaderPtr shader(new GLES2Shader(m_fileIOHub->GetFileManager(), m_shaderContext));
	if (shader->LoadShaderFromString(m_shaderContext, shaderName, codeAsciiString, focus, profile, entry))
	{
		return shader;
	}
	return ShaderPtr();
}

GLES2ShaderPtr GLES2Video::LoadGLES2ShaderFromFile(const str_type::string& fileName, const GS_SHADER_FOCUS focus)
{
	GLES2ShaderPtr shader(new GLES2Shader(m_fileIOHub->GetFileManager(), m_shaderContext));
	if (shader->LoadShaderFromFile(m_shaderContext, fileName, focus))
	{
		return shader;
	}
	return GLES2ShaderPtr();
}

GLES2ShaderPtr GLES2Video::LoadGLES2ShaderFromString(
	const str_type::string& shaderName,
	const std::string& codeAsciiString,
	const GS_SHADER_FOCUS focus)
{
	GLES2ShaderPtr shader(new GLES2Shader(m_fileIOHub->GetFileManager(), m_shaderContext));
	if (shader->LoadShaderFromString(m_shaderContext, shaderName, codeAsciiString, focus))
	{
		return shader;
	}
	return GLES2ShaderPtr();
}

boost::any GLES2Video::GetVideoInfo()
{
	// TODO
	return 0;
}

ShaderPtr GLES2Video::GetFontShader()
{
	return m_fastRenderVS;
}

ShaderPtr GLES2Video::GetOptimalVS()
{
	return m_optimalVS;
}

ShaderPtr GLES2Video::GetDefaultVS()
{
	return m_defaultVS;
}

ShaderPtr GLES2Video::GetVertexShader()
{
	return m_shaderContext->GetCurrentVS();
}

GLES2ShaderPtr GLES2Video::GetDefaultPS()
{
	return m_defaultPS;
}

ShaderPtr GLES2Video::GetPixelShader()
{
	return m_shaderContext->GetCurrentPS();
}

ShaderContextPtr GLES2Video::GetShaderContext()
{
	return m_shaderContext;
}

bool GLES2Video::SetVertexShader(ShaderPtr pShader)
{
	if (pShader)
	{
		m_shaderContext->SetShader(
			boost::dynamic_pointer_cast<GLES2Shader>(pShader),
			boost::dynamic_pointer_cast<GLES2Shader>(m_shaderContext->GetCurrentPS()),
			m_orthoMatrix,
			GetCurrentTargetSize());
	}
	else
	{
		m_shaderContext->SetShader(m_defaultVS, boost::dynamic_pointer_cast<GLES2Shader>(m_shaderContext->GetCurrentPS()),
				m_orthoMatrix, GetCurrentTargetSize());
	}
	return true;
}

bool GLES2Video::SetPixelShader(ShaderPtr pShader)
{
	if (pShader)
	{
		m_shaderContext->SetShader(
			boost::dynamic_pointer_cast<GLES2Shader>(m_shaderContext->GetCurrentVS()),
			boost::dynamic_pointer_cast<GLES2Shader>(pShader),
			m_orthoMatrix, GetCurrentTargetSize());
	}
	else
	{
		m_shaderContext->SetShader(boost::dynamic_pointer_cast<GLES2Shader>(m_shaderContext->GetCurrentVS()), m_defaultPS,
				m_orthoMatrix, GetCurrentTargetSize());
	}
	return true;
}

GS_SHADER_PROFILE GLES2Video::GetHighestVertexProfile() const
{
	return GSSP_MODEL_2;
}

GS_SHADER_PROFILE GLES2Video::GetHighestPixelProfile() const
{
	return GSSP_MODEL_2;
}

boost::any GLES2Video::GetGraphicContext()
{
	// it has actually two command forwarders at the moment...
	return PullCommands(); // TO-DO/TODO: use only one!
}

Video::VIDEO_MODE GLES2Video::GetVideoMode(const unsigned int modeIdx) const
{
	VIDEO_MODE vm;
	vm.width = m_screenSize.x;
	vm.height = m_screenSize.y;
	vm.pf = GSPF_DEFAULT;
	vm.idx = 0x01;
	return vm;
}

unsigned int GLES2Video::GetVideoModeCount()
{
	return 1;
}

bool GLES2Video::ResetVideoMode(const VIDEO_MODE& mode, const bool toggleFullscreen)
{
	return ResetVideoMode(mode.width, mode.height, mode.pf, false);
}

bool GLES2Video::ResetVideoMode(
	const unsigned int width,
	const unsigned int height,
	const GS_PIXEL_FORMAT pfBB,
	const bool toggleFullscreen)
{
	m_screenSize.x = width;
	m_screenSize.y = height;

	Enable2D(width, height);
	m_shaderContext->ResetViewConstants(m_orthoMatrix, GetScreenSizeF());

	ScreenSizeChangeListenerPtr listener = m_screenSizeChangeListener.lock();
	if (listener)
		listener->ScreenSizeChanged(GetScreenSizeF());

	return true;
}

bool GLES2Video::SetRenderTarget(SpritePtr pTarget, const unsigned int target)
{
	if (!pTarget)
	{
		m_currentTarget.reset();
		UnbindFrameBuffer();
	}
	else
	{
		if (pTarget->GetType() == Sprite::T_TARGET)
		{
			m_currentTarget = pTarget->GetTexture();
		}
		else
		{
			Message(GS_L("The current sprite has no render target texture"), GSMT_ERROR);
		}
	}
	return true;
}

unsigned int GLES2Video::GetMaxRenderTargets() const
{
	return 1;
}

unsigned int GLES2Video::GetMaxMultiTextures() const
{
	return _GS2D_GLES2_MAX_MULTI_TEXTURES;
}

void GLES2Video::SetupMultitextureShader()
{
	if (m_blendTextures[1])
	{
		ShaderPtr pixelShader = m_blendModes[1] == GSBM_ADD ? m_add1 : m_modulate1;
		pixelShader->SetTexture("t1", m_blendTextures[1]);
		SetPixelShader(pixelShader);
	}
}

void GLES2Video::DisableMultitextureShader()
{
	SetPixelShader(m_defaultPS);
}

bool GLES2Video::SetBlendMode(const unsigned int passIdx, const GS_BLEND_MODE mode)
{
	if (passIdx == 0 || passIdx >= _GS2D_GLES2_MAX_MULTI_TEXTURES)
	{
		Message(GS_L("Invalid pass index set on SetBlendMode"), GSMT_ERROR);
		return false;
	}
	else
	{
		m_blendModes[passIdx] = mode;
		return true;
	}
}

bool GLES2Video::SetBlendTexture(const unsigned int passIdx, GLES2TexturePtr texture)
{
	if (passIdx == 0 || passIdx >= _GS2D_GLES2_MAX_MULTI_TEXTURES)
	{
		Message(GS_L("Invalid pass index set on SetBlendTexture"), GSMT_ERROR);
		return false;
	}
	else
	{
		m_blendTextures[passIdx] = texture;
		return true;
	}
}

GS_BLEND_MODE GLES2Video::GetBlendMode(const unsigned int passIdx) const
{
	return m_blendModes[passIdx];
}

bool GLES2Video::UnsetTexture(const unsigned int passIdx)
{
	if (passIdx == 0 || passIdx >= _GS2D_GLES2_MAX_MULTI_TEXTURES)
	{
		Message(GS_L("Invalid pass index set on UnsetTexture"), GSMT_ERROR);
		return false;
	}
	else
	{
		m_blendTextures[passIdx].reset();
		DisableMultitextureShader();
		return true;
	}
}

void GLES2Video::SetZBuffer(const bool enable)
{
	if (m_zBuffer)
	{
		if (!enable)
			glDisable(GL_DEPTH_TEST);
	}
	else
	{
		if (enable)
			glEnable(GL_DEPTH_TEST);
	}
	m_zBuffer = enable;
}

bool GLES2Video::GetZBuffer() const
{
	/*GLboolean enabled;
	glGetBooleanv(GL_DEPTH_TEST, &enabled);
	return (enabled == GL_TRUE) ? true : false;*/
	return m_zBuffer;
}

void GLES2Video::SetZWrite(const bool enable)
{
	if (m_zWrite)
	{
		if (!enable)
			glDepthMask(GL_FALSE);
	}
	else
	{
		if (enable)
			glDepthMask(GL_TRUE);
	}
	m_zWrite = enable;
}

bool GLES2Video::GetZWrite() const
{
	/*GLboolean enabled;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &enabled);
	return (enabled == GL_TRUE) ? true : false;*/
	return m_zWrite;
}

bool GLES2Video::SetClamp(const bool set)
{
	// TODO
	return false;
}

bool GLES2Video::GetClamp() const
{
	// TODO
	return false;
}

bool GLES2Video::SetSpriteDepth(const float depth)
{
	return m_shaderContext->SetSpriteDepth(depth);
}

float GLES2Video::GetSpriteDepth() const
{
	return m_shaderContext->GetSpriteDepth();
}

void GLES2Video::SetLineWidth(const float width)
{
	// TODO
}

float GLES2Video::GetLineWidth() const
{
	// TODO
	return 0.0f;
}

bool GLES2Video::SetCameraPos(const Vector2 &pos)
{
	m_v2Camera = pos;
	return true;
}

bool GLES2Video::MoveCamera(const Vector2 &dir)
{
	SetCameraPos(m_v2Camera + dir);
	return true;
}

Vector2 GLES2Video::GetCameraPos() const
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

void GLES2Video::RoundUpPosition(const bool roundUp)
{
	m_roundUpPosition = roundUp;
}

bool GLES2Video::IsRoundingUpPosition() const
{
	return m_roundUpPosition;
}

bool GLES2Video::SetScissor(const bool& enable)
{
	if (enable)
	{
		glEnable(GL_SCISSOR_TEST);
	}
	else
	{
		glDisable(GL_SCISSOR_TEST);
	}
	return true;
}

bool GLES2Video::SetScissor(const Rect2D& rect)
{
	SetScissor(true);
	GLint posY;
	TexturePtr target = m_currentTarget.lock();
	if (target)
	{
		posY = static_cast<GLint>(rect.pos.y);
	}
	else
	{
		posY = static_cast<GLint>(m_screenSize.y) - static_cast<GLint>(rect.pos.y + rect.size.y);
	}

	if (m_scissor != rect)
	{
		m_scissor = rect;
		glScissor(static_cast<GLint>(rect.pos.x), posY,
				  static_cast<GLsizei>(rect.size.x), static_cast<GLsizei>(rect.size.y));
	}
	return true;
}

Rect2D GLES2Video::GetScissor() const
{
	return m_scissor;
}

void GLES2Video::UnsetScissor()
{
	SetScissor(false);
}

bool GLES2Video::DrawLine(const Vector2& p1, const Vector2& p2, const Color& color1, const Color& color2)
{
	// TODO
	return false;
}

bool GLES2Video::DrawRectangle(
	const Vector2& v2Pos,
	const Vector2& v2Size,
	const Color& color,
	const float angle,
	const GS_ENTITY_ORIGIN origin)
{
	// TODO
	return false;
}

bool GLES2Video::DrawRectangle(
	const Vector2& v2Pos,
	const Vector2& v2Size,
	const Color& color0,
	const Color& color1,
	const Color& color2,
	const Color& color3,
	const float angle,
	const GS_ENTITY_ORIGIN origin)
{
	// TODO
	return false;
}

void GLES2Video::SetBGColor(const Color& backgroundColor)
{
	m_backgroundColor = backgroundColor;
}

Color GLES2Video::GetBGColor() const
{
	return m_backgroundColor;
}


bool GLES2Video::BeginSpriteScene(const Color& dwBGColor)
{
	UnbindFrameBuffer();
	if (dwBGColor != gs2d::constant::ZERO)
	{
		m_backgroundColor = dwBGColor;
	}
	Vector4 color;
	color.SetColor(m_backgroundColor);
	glClearColor(color.x, color.y, color.z, color.w);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	SetAlphaMode(GSAM_PIXEL);
	m_rendering = true;
	return true;
}

bool GLES2Video::EndSpriteScene()
{
	m_rendering = false;
	ComputeFPSRate();
	return true;
}

bool GLES2Video::BeginTargetScene(const Color& dwBGColor, const bool clear)
{
	// explicit static cast for better performance
	TexturePtr texturePtr = m_currentTarget.lock();
	if (texturePtr)
	{
		Texture *pTexture = texturePtr.get(); // safety compile-time error checking
		GLES2Texture *pGLES2Texture = static_cast<GLES2Texture*>(pTexture); // safer direct cast
		const GLuint target = pGLES2Texture->GetFrameBufferID();
		glBindFramebuffer(GL_FRAMEBUFFER, target);

		CheckFrameBufferStatus(m_logger, target, pGLES2Texture->GetTextureID(), false);

		if (clear)
		{
			Vector4 color;
			color.SetColor(dwBGColor);
			glClearColor(color.x, color.y, color.z, color.w);
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		}

		const Texture::PROFILE& profile = pGLES2Texture->GetProfile();
		Enable2D(static_cast<int>(profile.width), static_cast<int>(profile.height), true);
		m_shaderContext->ResetViewConstants(m_orthoMatrix, GetCurrentTargetSize());
	}
	else
	{
		Message(GS_L("There's no render target"), GSMT_ERROR);
	}
	m_rendering = true;
	return true;
}

bool GLES2Video::EndTargetScene()
{
	SetRenderTarget(SpritePtr());
	m_rendering = false;
	UnbindFrameBuffer();
	Enable2D(m_screenSize.x, m_screenSize.y);
	m_shaderContext->ResetViewConstants(m_orthoMatrix, GetScreenSizeF());
	return true;
}

bool GLES2Video::IsTrue(const GLboolean& enabled)
{
	return (enabled == GL_TRUE) ? true : false;
}

void GLES2Video::SetBlend(const bool enable)
{
	if (m_blend)
	{
		if (!enable)
			glDisable(GL_BLEND);
	}
	else
	{
		if (enable)
			glEnable(GL_BLEND);
	}
	m_blend = enable;
}	

bool GLES2Video::SetAlphaMode(const GS_ALPHA_MODE mode)
{
	m_alphaMode = mode;
	switch(mode)
	{
	case GSAM_PIXEL:
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		SetBlend(true);
		return true;
	case GSAM_ADD:
		glBlendFunc(GL_ONE, GL_ONE);
		SetBlend(true);
		return true;
	case GSAM_MODULATE:
		glBlendFunc(GL_ZERO, GL_SRC_COLOR);
		SetBlend(true);
		return true;
	case GSAM_NONE:
	case GSAM_ALPHA_TEST: // alpha test not supported
	default:
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		SetBlend(false);
		return true;
	};
}

GS_ALPHA_MODE GLES2Video::GetAlphaMode() const
{
	return m_alphaMode;
}

bool GLES2Video::SetFilterMode(const GS_TEXTUREFILTER_MODE tfm)
{
	// NOTE: it won't work on OpenGL ES exactly like in the D3D9 implementation since
	// the texture has to be previously bound before we reset filter mode
	m_textureFilterMode = tfm;
	switch (tfm)
	{
	case GSTM_IFNEEDED:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		break;
	case GSTM_ALWAYS:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		break;
	case GSTM_NEVER:
	default:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		break;
	};
	return true;
}

GS_TEXTUREFILTER_MODE GLES2Video::GetFilterMode() const
{
	return m_textureFilterMode;
}

bool GLES2Video::Rendering() const
{
	return m_rendering;
}

bool GLES2Video::SaveScreenshot(const wchar_t *wcsName,
		const GS_BITMAP_FORMAT fmt, Rect2D rect)
{
	// TODO
	return false;
}

bool GLES2Video::ManageLoop()
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

Vector2i GLES2Video::GetClientScreenSize() const
{
	return m_screenSize;
}

Application::APP_STATUS GLES2Video::HandleEvents()
{
	if (m_quit)
	{
		Command(Platform::NativeCommandAssembler::QuitApplication());
	}
	return m_quit ? APP_QUIT : APP_OK;
}

float GLES2Video::GetFPSRate() const
{
	return Max(1.0f, m_fpsRate);
}

void GLES2Video::ComputeFPSRate()
{
	static float counter = 0.0f;
	const clock_t current = GetElapsedTime(TU_MILLISECONDS);
	static clock_t last = current;

	const clock_t elapsed = current - last;
	if (elapsed > 500)
	{
		m_fpsRate = (counter * 2);
		counter = 0.0f;
		last = current;
	}
	else
	{
		counter++;
	}
}

void GLES2Video::Message(const str_type::string& text, const GS_MESSAGE_TYPE type) const
{
	str_type::string str;
	Platform::FileLogger::TYPE logType = Platform::FileLogger::ERROR;
	switch (type)
	{
	case GSMT_WARNING:
		str = "WARNING: ";
		logType = Platform::FileLogger::WARNING;
		break;
	case GSMT_INFO:
		str = "Info: ";
		logType = Platform::FileLogger::INFO;
		break;
	case GSMT_ERROR:
		str = "ERROR: ";
		logType = Platform::FileLogger::ERROR;
		break;
	}
	m_logger.Log(str + text, logType);
}

// the boost timer is behaving very strange on iOS, so this method will be implemented outside
#ifndef APPLE_IOS
float GLES2Video::GetElapsedTimeF(const TIME_UNITY unity) const
{
	return static_cast<float>(GetElapsedTimeD(unity));
}

unsigned long GLES2Video::GetElapsedTime(const TIME_UNITY unity) const
{
	return static_cast<unsigned long>(GetElapsedTimeD(unity));
}

double GLES2Video::GetElapsedTimeD(const TIME_UNITY unity) const
{
	timeval current;
	gettimeofday(&current, NULL);
	const double curr = current.tv_sec    + (current.tv_usec    / 1000000.0);
	const double last = m_lastTime.tv_sec + (m_lastTime.tv_usec / 1000000.0);
	double elapsedTimeS = curr - last;
	switch (unity)
	{
	case TU_HOURS:
		elapsedTimeS /= 60.0;
		elapsedTimeS /= 60.0;
		break;
	case TU_MINUTES:
		elapsedTimeS /= 60.0;
		break;
	case TU_MILLISECONDS:
		elapsedTimeS *= 1000.0;
	case TU_SECONDS:
	default:
		break;
	};
	return elapsedTimeS;
}
#endif

void GLES2Video::ResetTimer()
{
	gettimeofday(&m_lastTime, NULL);
}

void GLES2Video::Quit()
{
	m_quit = true;
	Command(Platform::NativeCommandAssembler::QuitApplication());
}

void GLES2Video::EnableQuitShortcuts(const bool enable)
{
	// TODO
}

bool GLES2Video::QuitShortcutsEnabled()
{
	// TODO
	return false;
}

bool GLES2Video::SetWindowTitle(const str_type::string& title)
{
	m_windowTitle = title;
	return true;
}

str_type::string GLES2Video::GetWindowTitle() const
{
	return m_windowTitle;
}

void GLES2Video::EnableMediaPlaying(const bool enable)
{
	// TODO
}

Vector2 GLES2Video::GetCurrentTargetSize() const
{
	TexturePtr currentTarget = m_currentTarget.lock();
	return (currentTarget) ? currentTarget->GetBitmapSize() : GetScreenSizeF();
}

bool GLES2Video::IsWindowed() const
{
	return false;
}

Vector2i GLES2Video::GetScreenSize() const
{
	return m_screenSize;
}

Vector2 GLES2Video::GetScreenSizeF() const
{
	return Vector2(static_cast<float>(m_screenSize.x), static_cast<float>(m_screenSize.y));
}

Vector2i GLES2Video::GetWindowPosition()
{
	return Vector2i(0, 0);
}

void GLES2Video::SetWindowPosition(const Vector2i &v2)
{
	// dummy
}

Vector2i GLES2Video::ScreenToWindow(const Vector2i &v2Point) const
{
	return v2Point;
}

bool GLES2Video::WindowVisible() const
{
	return true;
}

bool GLES2Video::WindowInFocus() const
{
	return true;
}

bool GLES2Video::HideCursor(const bool hide)
{
	return true;
}

bool GLES2Video::IsCursorHidden() const
{
	return true;
}

const Platform::FileLogger& GLES2Video::GetLogger() const
{
	return m_logger;
}

str_type::string GLES2Video::PullCommands()
{
	str_type::string out;
	ForwardCommands(out);
	out += m_commands;
	m_commands = GS_L("");
	return out;
}

void GLES2Video::ForwardCommand(const str_type::string& cmd)
{
#	ifdef DEBUG
	std::cout << cmd << std::endl;
#	endif
	m_commands += (GS_L("\n") + cmd);
}

Platform::FileIOHubPtr GLES2Video::GetFileIOHub()
{
	return m_fileIOHub;
}

} // namespace gs2d