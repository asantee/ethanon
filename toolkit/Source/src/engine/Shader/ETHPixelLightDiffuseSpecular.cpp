#include "ETHPixelLightDiffuseSpecular.h"
#include "ETHShaders.h"

const float ETHPixelLightDiffuseSpecular::LIGHT_PRECISION_DOWNSCALE = 1.0f / 1280.0f;

ETHPixelLightDiffuseSpecular::ETHPixelLightDiffuseSpecular(
	VideoPtr video,
	const str_type::string& shaderPath,
	ETHFakeEyePositionManagerPtr fakeEyeManager)
	: m_fakeEyeManager(fakeEyeManager)
{
	m_video = video;
	
	m_pixelLightShader = m_video->LoadShaderFromString(
		"hPixelLightVS", ETHShaders::PL_VS_Hor_Light(), "",
		"hPixelLightPS", ETHShaders::PL_PS_Hor_Diff(), "");

	m_pixelLightSpecularShader = m_video->LoadShaderFromString(
		"hPixelLightVS", ETHShaders::PL_VS_Hor_Light(), "",
		"hPixelLightSpecularPS", ETHShaders::PL_PS_Hor_Spec(), "");

	m_defaultNM = m_video->CreateSprite(ETHGlobal::GetDataResourceFullPath(shaderPath, GS_L("default_nm.png")));
}

bool ETHPixelLightDiffuseSpecular::BeginLightPass(ETHSpriteEntity *pRender, Vector3 &v3LightPos, const Vector2 &v2Size,
	const ETHLight* light, const float maxHeight, const float minHeight, const float lightIntensity, const bool drawToTarget)
{
	const Vector2 &v2Origin = pRender->ComputeAbsoluteOrigin(v2Size);
	const Vector3 &v3EntityPos = pRender->GetPosition();

	// set the correct light shader
	ShaderPtr lightShader;
	const bool hasGloss = static_cast<bool>(pRender->GetGloss());
	if (hasGloss)
	{
		lightShader = m_pixelLightSpecularShader;
	}
	else
	{
		lightShader = m_pixelLightShader;
	}

	// if it has a gloss map, send specular data to shader
	if (hasGloss)
	{
		lightShader->SetConstant(GS_L("specularPower"), pRender->GetSpecularPower());
		lightShader->SetConstant(GS_L("specularBrightness"), pRender->GetSpecularBrightness());
		lightShader->SetTexture(GS_L("glossMap"), pRender->GetGloss()->GetTexture());
		lightShader->SetConstant(GS_L("fakeEyePos"), m_fakeEyeManager->ComputeFakeEyePosition(m_video, lightShader, drawToTarget, v3LightPos, pRender->GetAngle()));
	}

	// choose which normalmap to use
	if (pRender->GetNormal())
	{
		lightShader->SetTexture(GS_L("normalMap"), pRender->GetNormal()->GetTexture());
	}
	else
	{
		lightShader->SetTexture(GS_L("normalMap"), GetDefaultNormalMap()->GetTexture());
	}

	// sets spatial information to the shader
	lightShader->SetConstant(GS_L("topLeft3DPos"), v3EntityPos-Vector3(v2Origin,0));

	// TO-DO it looks like a mess around here...
	if (pRender->GetAngle() != 0.0f)
	{
		Vector3 newPos = v3LightPos-v3EntityPos;
		Matrix4x4 matRot = Matrix4x4::RotateZ(-Util::DegreeToRadian(pRender->GetAngle()));
		newPos = Matrix4x4::Multiply(newPos, matRot);
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

	lightShader->SetConstant(GS_L("lightPos"), v3LightPos * lightPrecisionDownScale);

	const float scaledRange = (light->range * lightPrecisionDownScale);
	lightShader->SetConstant(GS_L("squaredRange"), scaledRange * scaledRange);
	lightShader->SetConstant(GS_L("lightColor"), Vector4(light->color, 1.0f) * lightIntensity);

	m_video->SetCurrentShader(lightShader);

	return true;
}

bool ETHPixelLightDiffuseSpecular::EndLightPass()
{
	m_video->SetAlphaMode(m_lastAM);
	return true;
}

SpritePtr ETHPixelLightDiffuseSpecular::GetDefaultNormalMap()
{
	return m_defaultNM;
}
