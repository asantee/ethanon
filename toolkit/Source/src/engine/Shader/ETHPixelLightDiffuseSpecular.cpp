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

#include "ETHPixelLightDiffuseSpecular.h"
#include "ETHShaders.h"

#ifdef OPENGL
 const Shader::SHADER_PROFILE ETHPixelLightDiffuseSpecular::m_profile = Shader::SP_MODEL_1;
#else
 const Shader::SHADER_PROFILE ETHPixelLightDiffuseSpecular::m_profile = Shader::SP_MODEL_2;
#endif

ETHPixelLightDiffuseSpecular::ETHPixelLightDiffuseSpecular(
	VideoPtr video,
	const str_type::string& shaderPath,
	ETHFakeEyePositionManagerPtr fakeEyeManager)
	: m_fakeEyeManager(fakeEyeManager)
{
	m_video = video;
	if (IsSupportedByHardware())
	{
		m_hPixelLightPS = m_video->LoadShaderFromFile(ETHGlobal::GetDataResourceFullPath(shaderPath, ETHShaders::PL_PS_Hor_Diff()).c_str(), Shader::SF_PIXEL, m_profile);
		m_vPixelLightPS = m_video->LoadShaderFromFile(ETHGlobal::GetDataResourceFullPath(shaderPath, ETHShaders::PL_PS_Ver_Diff()).c_str(), Shader::SF_PIXEL, m_profile);
		m_hPixelLightVS = m_video->LoadShaderFromFile(ETHGlobal::GetDataResourceFullPath(shaderPath, ETHShaders::PL_VS_Hor_Light()).c_str(), Shader::SF_VERTEX, Shader::SP_MODEL_2);
		m_vPixelLightVS = m_video->LoadShaderFromFile(ETHGlobal::GetDataResourceFullPath(shaderPath, ETHShaders::PL_VS_Ver_Light()).c_str(), Shader::SF_VERTEX, Shader::SP_MODEL_2);
		m_hPixelLightSpecularPS = m_video->LoadShaderFromFile(ETHGlobal::GetDataResourceFullPath(shaderPath, ETHShaders::PL_PS_Hor_Spec()).c_str(), Shader::SF_PIXEL, m_profile);
		m_vPixelLightSpecularPS = m_video->LoadShaderFromFile(ETHGlobal::GetDataResourceFullPath(shaderPath, ETHShaders::PL_PS_Ver_Spec()).c_str(), Shader::SF_PIXEL, m_profile);
		m_defaultNM = m_video->CreateSprite(ETHGlobal::GetDataResourceFullPath(shaderPath, GS_L("default_nm.png")));
	}
}

bool ETHPixelLightDiffuseSpecular::BeginLightPass(ETHSpriteEntity *pRender, Vector3 &v3LightPos, const Vector2 &v2Size,
	const ETHLight* light, const float maxHeight, const float minHeight, const float lightIntensity, const bool drawToTarget)
{
	const Vector2 &v2Origin = pRender->ComputeAbsoluteOrigin(v2Size);
	const Vector3 &v3EntityPos = pRender->GetPosition();

	// set the correct light shader
	ShaderPtr pLightShader;
	const bool hasGloss = pRender->GetGloss();
	if (pRender->GetType() == ETHEntityProperties::ET_VERTICAL)
	{
		if (hasGloss)
		{
			pLightShader = m_vPixelLightSpecularPS;
		}
		else
		{
			pLightShader = m_vPixelLightPS;
		}
	}
	else
	{
		if (hasGloss)
		{
			pLightShader = m_hPixelLightSpecularPS;
		}
		else
		{
			pLightShader = m_hPixelLightPS;
		}
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
	if (pRender->GetType() == ETHEntityProperties::ET_VERTICAL)
	{
		m_vPixelLightVS->SetConstant(GS_L("spaceLength"), (maxHeight-minHeight));
		m_vPixelLightVS->SetConstant(GS_L("topLeft3DPos"), v3EntityPos-(Vector3(v2Origin.x,0,-v2Origin.y)));
		m_video->SetVertexShader(m_vPixelLightVS);
	}
	else
	{
		m_hPixelLightVS->SetConstant(GS_L("topLeft3DPos"), v3EntityPos-Vector3(v2Origin,0));
		m_video->SetVertexShader(m_hPixelLightVS);
	}

	// TO-DO it looks like a mess around here...
	if (pRender->GetType() != ETHEntityProperties::ET_VERTICAL)
	{
		if (pRender->GetAngle() != 0.0f)
		{
			Vector3 newPos = v3LightPos-v3EntityPos;
			Matrix4x4 matRot = RotateZ(-DegreeToRadian(pRender->GetAngle()));
			newPos = Multiply(newPos, matRot);
			v3LightPos = newPos + v3EntityPos;
		}
	}

	m_lastAM = m_video->GetAlphaMode();
	m_video->SetAlphaMode(Video::AM_ADD);

	// Set a depth value depending on the entity type
	pRender->SetDepth(maxHeight, minHeight);
 
	pLightShader->SetConstant(GS_L("lightPos"), v3LightPos);
	pLightShader->SetConstant(GS_L("squaredRange"), light->range * light->range);
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

bool ETHPixelLightDiffuseSpecular::IsSupportedByHardware() const
{
	return (m_video->GetHighestPixelProfile() >= m_profile);
}

bool ETHPixelLightDiffuseSpecular::IsUsingPixelShader() const
{
	return true;
}

SpritePtr ETHPixelLightDiffuseSpecular::GetDefaultNormalMap()
{
	return m_defaultNM;
}
