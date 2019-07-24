#include "GLES2Sprite.h"

#include "GLES2Video.h"

namespace gs2d {

using namespace math;

const str_type::string GLES2Sprite::SPRITE_LOG_FILE("GLES2Sprite.log.txt");
Platform::FileLogger GLES2Sprite::m_logger(Platform::FileLogger::GetLogDirectory() + GLES2Sprite::SPRITE_LOG_FILE);
std::vector<Vector2> GLES2Sprite::m_attachedParameters;

GLES2Sprite::GLES2Sprite(GLES2ShaderContextPtr shaderContext) :
		m_type(T_NOT_LOADED),
		m_densityValue(1.0f)
{
	m_shaderContext = shaderContext.get();
}

bool GLES2Sprite::LoadSprite(
	VideoWeakPtr video,
	unsigned char* pBuffer,
	const unsigned int bufferLength,
	Color mask,
	const unsigned int width,
	const unsigned int height)
{
	m_video = video.lock().get();
	// TODO 
	// SetupSpriteRects(1, 1);
	return false;
}

bool GLES2Sprite::LoadSprite(
	VideoWeakPtr video,
	const str_type::string& fileName,
	Color mask,
	const unsigned int width,
	const unsigned int height)
{
	m_video = video.lock().get();
	if (width == 0 || height == 0)
	{
		m_texture = boost::dynamic_pointer_cast<GLES2Texture>(m_video->LoadTextureFromFile(fileName, 0, width, height, 1));
		if (!m_texture)
		{
			m_logger.Log(fileName + " could not load sprite", Platform::FileLogger::ERROR);
			m_video->Message(fileName + " could not load sprite", GSMT_ERROR);
			return false;
		}
		else
		{
			const Texture::PROFILE profile = m_texture->GetProfile();
			m_bitmapSize.x = static_cast<float>(profile.width);
			m_bitmapSize.y = static_cast<float>(profile.height);
		}
	}
	else
	{
		m_bitmapSize.x = static_cast<float>(width);
		m_bitmapSize.y = static_cast<float>(height);
	}
	m_type = T_BITMAP;

	SetupSpriteRects(1, 1);
	return true;
}

bool GLES2Sprite::CreateRenderTarget(
	VideoWeakPtr video,
	const unsigned int width,
	const unsigned int height,
	const Texture::TARGET_FORMAT format)
{
	m_video = video.lock().get();
	m_texture = boost::dynamic_pointer_cast<GLES2Texture>(m_video->CreateRenderTargetTexture(width, height, format));
	m_bitmapSize = Vector2(static_cast<float>(width), static_cast<float>(height));
	m_type = T_TARGET;
	SetupSpriteRects(1, 1);
	return true;
}

bool GLES2Sprite::Draw(
	const Vector2 &v2Pos,
	const Vector4& color,
	const float angle,
	const Vector2 &v2Scale)
{
	Vector2 v2Size;
	if (m_rect.size.x != 0)
	{
		v2Size = m_rect.size;
	}
	else
	{
		v2Size = m_bitmapSize;
	}
	v2Size = v2Size * v2Scale;

	return DrawShaped(v2Pos, v2Size, color, color, color, color, angle);
}

bool GLES2Sprite::DrawShaped(
	const Vector2 &v2Pos,
	const Vector2 &v2Size,
	const Vector4& color0,
	const Vector4& color1,
	const Vector4& color2,
	const Vector4& color3,
	const float angle)
{
	DrawOptimal(v2Pos, color0, angle, v2Size);
	return true;
	ShaderPtr
		optimal = static_cast<GLES2Video*>(m_video)->GetOptimalShader(),
		current = m_video->GetCurrentShader(),
		defaultS = m_video->GetDefaultShader();
	if (current == optimal || current == defaultS)
	{
		if (color0 == color1 && color0 == color2 && color0 == color3)
		{
			if (current != optimal)
			{
				m_video->SetCurrentShader(optimal);
			}
			DrawOptimal(v2Pos, color0, angle, v2Size);
			return true;
		}
	}

	if (current == optimal)
	{
		m_video->SetCurrentShader(defaultS);
	}

	#ifdef GS2D_WARN_SLOW_DRAWING
	if (current == optimal)
		m_logger.Log(m_texture->GetFileName() + ": slow drawing!", Platform::FileLogger::WARNING);
	#endif

	ShaderPtr currentShaderSmartPtr = m_video->GetCurrentShader();
	Shader* currentShader = currentShaderSmartPtr.get();

	Vector2 pos(v2Pos), camPos(m_video->GetCameraPos()), center(m_normalizedOrigin*v2Size);

	Matrix4x4 mRot;
	if (angle != 0.0f)
	{
		mRot = Matrix4x4::RotateZ(math::Util::DegreeToRadian(-angle));
	}
	currentShader->SetMatrixConstant("rotationMatrix", mRot);
	currentShader->SetTexture("diffuse", m_texture, 0);

	if (m_rect.size.x == 0 || m_rect.size.y == 0)
	{
		currentShader->SetConstant("rectSize", GetBitmapSizeF());
		currentShader->SetConstant("rectPos", Vector2(0, 0));
	}
	else
	{
		currentShader->SetConstant("rectSize", m_rect.size);
		currentShader->SetConstant("rectPos", m_rect.pos);
	}

	Vector2 flipAdd, flipMul;
	GetFlipShaderParameters(flipAdd, flipMul);

	currentShader->SetConstant("screenSize", m_video->GetScreenSizeF());
	currentShader->SetConstant("bitmapSize", m_bitmapSize);
	currentShader->SetConstant("entityPos", pos);
	currentShader->SetConstant("center", center);
	currentShader->SetConstant("size", v2Size);
	currentShader->SetConstant("color0", color0);
	currentShader->SetConstant("color1", color1);
	currentShader->SetConstant("color2", color2);
	currentShader->SetConstant("color3", color3);
	currentShader->SetConstant("flipAdd", flipAdd);
	currentShader->SetConstant("flipMul", flipMul);
	currentShader->SetConstant("cameraPos", camPos);
	currentShader->SetConstant("depth", m_video->GetSpriteDepth());

	GLES2Video::m_polygonRenderer->BeginRendering(currentShaderSmartPtr);
	GLES2Video::m_polygonRenderer->Render();
	GLES2Video::m_polygonRenderer->EndRendering();
	return true;
}

bool GLES2Sprite::DrawOptimal(const math::Vector2 &v2Pos, const Vector4& color, const float angle, const Vector2 &v2Size)
{
	ShaderPtr currentShaderSmartPtr = m_video->GetCurrentShader();
	Shader* currentShader = currentShaderSmartPtr.get();

	currentShaderSmartPtr->SetShader();

	Vector2 size((v2Size != Vector2(-1, -1)) ? v2Size : m_bitmapSize);
	Vector2 pos(v2Pos), camPos(m_video->GetCameraPos()), center(m_normalizedOrigin*size);

	Matrix4x4 mRot;
	if (angle != 0.0f)
	{
		mRot = Matrix4x4::RotateZ(Util::DegreeToRadian(-angle));
	}

	currentShader->SetTexture("diffuse", m_texture, 0);

	Vector2 rectPos, rectSize;
	if (m_rect.size.x == 0 || m_rect.size.y == 0)
	{
		rectPos = Vector2(0, 0);
		rectSize = GetBitmapSizeF();
	}
	else
	{
		rectPos = m_rect.pos;
		rectSize = m_rect.size;
	}

	Vector2 flipAdd, flipMul;
	GetFlipShaderParameters(flipAdd, flipMul);

	const std::size_t numBasicParams = 13;
	const std::size_t numParams = numBasicParams + m_attachedParameters.size();
	Vector2 *params = new Vector2 [numParams];
	params[0] = rectPos;
	params[1] = rectSize;
	params[2] = center;
	params[3] = size;
	params[4] = pos;
	params[5] = camPos;
	params[6] = m_bitmapSize;
	params[7] = Vector2(color.x, color.y);
	params[8] = Vector2(color.z, color.w);
	params[9] = Vector2(m_video->GetSpriteDepth(), m_video->GetSpriteDepth());
	params[10] = m_video->GetScreenSizeF();
	params[11] = flipAdd;
	params[12] = flipMul;
	const std::size_t first = numBasicParams; // to-do make it safer

	for (std::size_t t = 0; t < m_attachedParameters.size(); t++)
	{
		params[t + first] = m_attachedParameters[t];
	}

	currentShader->SetMatrixConstant("rotationMatrix", mRot);

	currentShader->SetConstantArray(
		"params",
		static_cast<unsigned int>(numParams),
		boost::shared_array<const math::Vector2>(params));

	GLES2Video::m_polygonRenderer->BeginRendering(currentShaderSmartPtr);
	GLES2Video::m_polygonRenderer->Render();
	GLES2Video::m_polygonRenderer->EndRendering();

	m_attachedParameters.clear();
	return true;
}

void GLES2Sprite::AttachParametersToOptimalRenderer(const std::vector<math::Vector2>& params)
{
	m_attachedParameters = params;
}

bool GLES2Sprite::SaveBitmap(const str_type::char_t* wcsName, const Texture::BITMAP_FORMAT fmt, Rect2Di *pRect)
{
	// TODO 
	return false;
}

bool GLES2Sprite::DrawShapedFast(const Vector2 &v2Pos, const Vector2 &v2Size, const math::Vector4& color)
{
	Shader* currentShader = m_video->GetCurrentShader().get();

	Vector2 rectSize, rectPos;
	if (m_rect.size.x == 0 || m_rect.size.y == 0)
	{
		rectSize = GetBitmapSizeF();
		rectPos = Vector2(0, 0);
	}
	else
	{
		rectSize = m_rect.size;
		rectPos = m_rect.pos;
	}	

	static const unsigned int numParams = 8;
	Vector2 *params = new Vector2 [numParams];
	params[0] = rectPos;
	params[1] = rectSize;
	params[2] = v2Size;
	params[3] = v2Pos;
	params[4] = m_bitmapSize;
	params[5] = m_video->GetScreenSizeF();
	params[6] = Vector2(color.x, color.y);
	params[7] = Vector2(color.z, color.w);

	currentShader->SetConstantArray("params", numParams, boost::shared_array<const math::Vector2>(params));

	GLES2Video::m_polygonRenderer->Render();
	return true;
}

void GLES2Sprite::BeginFastRendering()
{
	ShaderPtr fastShader = m_video->GetFastShader();
	m_video->SetCurrentShader(fastShader);
	fastShader->SetTexture("diffuse", m_texture, 0);
	GLES2Video::m_polygonRenderer->BeginRendering(fastShader);
}

void GLES2Sprite::EndFastRendering()
{
	GLES2Video::m_polygonRenderer->EndRendering();
}

TextureWeakPtr GLES2Sprite::GetTexture()
{
	return m_texture;
}

Texture::PROFILE GLES2Sprite::GetProfile() const
{
	return m_texture->GetProfile();
}

Vector2i GLES2Sprite::GetBitmapSize() const
{
	return Vector2i(static_cast<int>(m_bitmapSize.x), static_cast<int>(m_bitmapSize.y));
}

Vector2 GLES2Sprite::GetBitmapSizeF() const
{
	return m_bitmapSize;
}

Sprite::TYPE GLES2Sprite::GetType() const
{
	return m_type;;
}

boost::any GLES2Sprite::GetTextureObject()
{
	return m_texture->GetTextureID();
}

void GLES2Sprite::GenerateBackup()
{
	// TODO 
}

bool GLES2Sprite::SetAsTexture(const unsigned int passIdx)
{
	m_video->GetCurrentShader()->SetTexture("t1", m_texture, passIdx);
	return true;
}

void GLES2Sprite::OnLostDevice()
{
	// TODO 
}

void GLES2Sprite::RecoverFromBackup()
{
	// TODO 
}

void GLES2Sprite::SetSpriteDensityValue(const float value)
{
	if (m_type == T_TARGET)
		return;

	Texture::PROFILE profile = m_texture->GetProfile();
	m_bitmapSize = Vector2(static_cast<float>(profile.width) / value, static_cast<float>(profile.height) / value);
	m_densityValue = value;
	SetupSpriteRects(1, 1);
}

float GLES2Sprite::GetSpriteDensityValue() const
{
	return m_densityValue;
}

} // namespace gs2d
