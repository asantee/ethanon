#include "ETHPixelLightDiffuseSpecular.h"
#include "ETHShaders.h"

#ifdef OPENGL
 const Shader::SHADER_PROFILE ETHPixelLightDiffuseSpecular::m_profile = Shader::SP_MODEL_1;
#else
 const Shader::SHADER_PROFILE ETHPixelLightDiffuseSpecular::m_profile = Shader::SP_MODEL_2;
#endif

const float ETHPixelLightDiffuseSpecular::LIGHT_PRECISION_DOWNSCALE = 1.0f / 1280.0f;

ETHPixelLightDiffuseSpecular::ETHPixelLightDiffuseSpecular(
	VideoPtr video,
	const str_type::string& shaderPath,
	ETHFakeEyePositionManagerPtr fakeEyeManager)
	: m_fakeEyeManager(fakeEyeManager)
{
	m_video = video;
	m_hPixelLightPS = m_video->LoadShaderFromString(GS_L("hPixelLightPS"), ETHShaders::PL_PS_Hor_Diff(), Shader::SF_PIXEL, m_profile);
	m_hPixelLightVS = m_video->LoadShaderFromString(GS_L("hPixelLightVS"), ETHShaders::PL_VS_Hor_Light(), Shader::SF_VERTEX, Shader::SP_MODEL_2);
	m_hPixelLightSpecularPS = m_video->LoadShaderFromString(GS_L("hPixelLightSpecularPS"), ETHShaders::PL_PS_Hor_Spec(), Shader::SF_PIXEL, m_profile);
	m_defaultNM = m_video->CreateSprite(ETHGlobal::GetDataResourceFullPath(shaderPath, GS_L("default_nm.png")));
}

bool ETHPixelLightDiffuseSpecular::BeginLightPass(ETHSpriteEntity *pRender, Vector3 &v3LightPos, const Vector2 &v2Size,
	const ETHLight* light, const float maxHeight, const float minHeight, const float lightIntensity, const bool drawToTarget)
{
	const Vector2 &v2Origin = pRender->ComputeAbsoluteOrigin(v2Size);
	const Vector3 &v3EntityPos = pRender->GetPosition();

	// set the correct light shader
	ShaderPtr pLightShader;
	const bool hasGloss = static_cast<bool>(pRender->GetGloss());
	if (hasGloss)
	{
		pLightShader = m_hPixelLightSpecularPS;
	}
	else
	{
		pLightShader = m_hPixelLightPS;
	}

	// if it has a gloss map, send specular data to shader
	if (hasGloss)
	{
		pLightShader->SetConstant(GS_L("specularPower"), pRender->GetSpecularPower());
		pLightShader->SetConstant(GS_L("specularBrightness"), pRender->GetSpecularBrightness());
		pLightShader->SetTexture(GS_L("glossMap"), pRender->GetGloss()->GetTexture());
		pLightShader->SetConstant(GS_L("fakeEyePos"), m_fakeEyeManager->ComputeFakeEyePosition(m_video, pLightShader, drawToTarget, v3LightPos, pRender->GetAngle()));
	}

	// choose which normalmap to use
	m_video->SetPixelShader(pLightShader);
	if (pRender->GetNormal())
	{
		pLightShader->SetTexture(GS_L("normalMap"), pRender->GetNormal()->GetTexture());
	}
	else
	{
		pLightShader->SetTexture(GS_L("normalMap"), GetDefaultNormalMap()->GetTexture());
	}

	// sets spatial information to the shader
	m_hPixelLightVS->SetConstant(GS_L("topLeft3DPos"), v3EntityPos-Vector3(v2Origin,0));
	m_video->SetVertexShader(m_hPixelLightVS);

	// TO-DO it looks like a mess around here...
	if (pRender->GetAngle() != 0.0f)
	{
		Vector3 newPos = v3LightPos-v3EntityPos;
		Matrix4x4 matRot = RotateZ(-DegreeToRadian(pRender->GetAngle()));
		newPos = Multiply(newPos, matRot);
		v3LightPos = newPos + v3EntityPos;
	}

	m_lastAM = m_video->GetAlphaMode();
	m_video->SetAlphaMode(Video::AM_ADD);

	// Set a depth value depending on the entity type
	pRender->SetDepth(maxHeight, minHeight);
 
 	// downscales pixel shader ranges on android to prevent from lower precision glitches
 	float lightPrecisionDownScale = 1.0f;
 	#ifdef GLES2
	 	lightPrecisionDownScale = LIGHT_PRECISION_DOWNSCALE;
 	#endif

	pLightShader->SetConstant(GS_L("lightPos"), v3LightPos * lightPrecisionDownScale);

	const float scaledRange = (light->range * lightPrecisionDownScale);
	pLightShader->SetConstant(GS_L("squaredRange"), scaledRange * scaledRange);
	pLightShader->SetConstant(GS_L("lightColor"), Vector4(light->color, 1.0f) * lightIntensity);

	return true;
}

bool ETHPixelLightDiffuseSpecular::EndLightPass()
{
	m_video->SetPixelShader(ShaderPtr());
	m_video->SetVertexShader(ShaderPtr());
	m_video->SetAlphaMode(m_lastAM);
	return true;
}

SpritePtr ETHPixelLightDiffuseSpecular::GetDefaultNormalMap()
{
	return m_defaultNM;
}
