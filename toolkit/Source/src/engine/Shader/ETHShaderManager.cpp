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

#include "ETHShaderManager.h"
#include "../Scene/ETHScene.h"
#include "ETHPixelLightDiffuseSpecular.h"
#include "ETHVertexLightDiffuse.h"
#include "ETHShaders.h"

ETHShaderManager::ETHShaderManager(VideoPtr video, const str_type::string& shaderPath, const bool richLighting) :
	m_lastAM(Video::AM_PIXEL),
	m_fakeEyeManager(new ETHFakeEyePositionManager),
	m_richLighting(richLighting)
{
	m_video = video;

	m_defaultVS = m_video->LoadShaderFromFile(ETHGlobal::GetDataResourceFullPath(shaderPath, ETHShaders::DefaultVS()).c_str(), GSSF_VERTEX, GSSP_MODEL_2);
	m_particle  = m_video->LoadShaderFromFile(ETHGlobal::GetDataResourceFullPath(shaderPath, ETHShaders::Particle_VS()).c_str(), GSSF_VERTEX, GSSP_MODEL_2);
	m_defaultStaticAmbientVS  = m_video->LoadShaderFromFile(ETHGlobal::GetDataResourceFullPath(shaderPath, ETHShaders::Ambient_VS_Hor()).c_str(), GSSF_VERTEX, GSSP_MODEL_2);
	m_verticalStaticAmbientVS = m_video->LoadShaderFromFile(ETHGlobal::GetDataResourceFullPath(shaderPath, ETHShaders::Ambient_VS_Ver()).c_str(), GSSF_VERTEX, GSSP_MODEL_2);
	m_shadowVS = m_video->LoadShaderFromFile(ETHGlobal::GetDataResourceFullPath(shaderPath, ETHShaders::Shadow_VS_Ver()).c_str(), GSSF_VERTEX, GSSP_MODEL_2);

	#ifndef GLES2
		m_projShadow = m_video->CreateSprite(ETHGlobal::GetDataResourceFullPath(shaderPath, GS_L("shadow.dds")));
	#else
		m_projShadow = m_video->CreateSprite(ETHGlobal::GetDataResourceFullPath(shaderPath, GS_L("shadow.png")));
	#endif

	if (m_richLighting)
	{
		// Not yet implemented on GLES2
		#ifndef GLES2
		{
			ETHLightingProfilePtr profile(new ETHVertexLightDiffuse(m_video, shaderPath));
			if (profile->IsSupportedByHardware())
			{
				m_lightingProfiles[VERTEX_LIGHTING_DIFFUSE] = profile;
			}
		}
		#endif
		{
			ETHLightingProfilePtr profile(new ETHPixelLightDiffuseSpecular(m_video, shaderPath, m_fakeEyeManager));
			if (profile->IsSupportedByHardware())
			{
				m_lightingProfiles[PIXEL_LIGHTING_DIFFUSE_SPECULAR] = profile;
			}
		}
	}

	if (m_lightingProfiles.empty())
	{
		video->Message(GS_L("ETHShaderManager::ETHShaderManager: no lighting profile"), GSMT_WARNING);
	}
	else
	{
		m_currentProfile = FindHighestLightingProfile();
	}
}

ETHLightingProfilePtr ETHShaderManager::FindHighestLightingProfile()
{
	if (m_lightingProfiles.empty())
	{
		m_video->Message(GS_L("ETHShaderManager::FindHighestLightingProfile: no lighting profile"), GSMT_WARNING);
		return ETHLightingProfilePtr();
	}
	return m_lightingProfiles.rbegin()->second;
}

SpritePtr ETHShaderManager::GetProjShadow()
{
	return m_projShadow;
}

bool ETHShaderManager::BeginAmbientPass(const ETHSpriteEntity *pRender, const float maxHeight, const float minHeight)
{
	m_video->SetPixelShader(ShaderPtr());

	if (pRender->GetType() == ETHEntityProperties::ET_VERTICAL)
	{
		m_verticalStaticAmbientVS->SetConstant(GS_L("spaceLength"), (maxHeight-minHeight));
		m_video->SetVertexShader(m_verticalStaticAmbientVS);
	}
	else
	{
		m_video->SetVertexShader(m_defaultStaticAmbientVS);
	}
	m_parallaxManager.SetShaderParameters(m_video, m_video->GetVertexShader(), pRender->GetPosition(), pRender->GetParallaxIndividualIntensity(), false);

	m_lastAM = m_video->GetAlphaMode();
	return true;
}

bool ETHShaderManager::EndAmbientPass()
{
	//m_video->SetPixelShader(ShaderPtr());
	//m_video->SetVertexShader(ShaderPtr());

	if (m_lastAM != m_video->GetAlphaMode())
		m_video->SetAlphaMode(m_lastAM);
	return true;
}

bool ETHShaderManager::BeginLightPass(ETHSpriteEntity *pRender, const ETHLight* light,
									  const float maxHeight, const float minHeight, const float lightIntensity,
									  const ETHSpriteEntity *pParent, const bool drawToTarget)
{
	if (!light || !pRender->IsApplyLight())
		return false;

	Vector3 v3LightPos;
	if (pParent)
		v3LightPos = pParent->GetPosition() + light->pos;
	else
		v3LightPos = light->pos;

	const Vector2 &v2Size = pRender->GetCurrentSize();
	const float size = Max(v2Size.x, v2Size.y);
	const float distance = SquaredDistance(pRender->GetPosition(), v3LightPos);
	const float radius = (light->range + size);
	if (distance > radius * radius)
		return false;

	m_currentProfile->BeginLightPass(pRender, v3LightPos, v2Size, light, maxHeight, minHeight, lightIntensity, drawToTarget);
	m_parallaxManager.SetShaderParameters(m_video, m_video->GetVertexShader(), pRender->GetPosition(), pRender->GetParallaxIndividualIntensity(), drawToTarget);
	return true;
}

bool ETHShaderManager::EndLightPass()
{
	m_video->SetPixelShader(ShaderPtr());
	m_video->SetVertexShader(ShaderPtr());
	m_video->SetAlphaMode(m_lastAM);
	return true;
}

bool ETHShaderManager::BeginShadowPass(const ETHSpriteEntity *pRender, const ETHLight* light, const float maxHeight, const float minHeight)
{
	if (!light || !light->castShadows || !pRender->IsCastShadow()/* || pRender->GetType() != ETH_VERTICAL*/)
		return false;

	m_lastAM = m_video->GetAlphaMode();
	m_video->SetAlphaMode(Video::AM_PIXEL);
	m_video->SetVertexShader(m_shadowVS);

	m_shadowVS->SetConstant(GS_L("lightRange"), light->range);
	m_video->SetSpriteDepth(((pRender->GetPosition().z + ETH_SMALL_NUMBER - minHeight) / (maxHeight - minHeight)));
	m_video->SetPixelShader(ShaderPtr());
	return true;
}

bool ETHShaderManager::EndShadowPass()
{
	m_video->SetPixelShader(ShaderPtr());
	m_video->SetVertexShader(ShaderPtr());
	m_video->SetAlphaMode(m_lastAM);
	return true;
}

bool ETHShaderManager::BeginHaloPass(const ETHLight* light, const float maxHeight, const ETHSpriteEntity *pParent)
{
	GS2D_UNUSED_ARGUMENT(maxHeight);
	GS2D_UNUSED_ARGUMENT(pParent);
	if (!light || light->haloBitmap == GS_L(""))
		return false;

	m_lastAM = m_video->GetAlphaMode();
	m_video->SetAlphaMode(Video::AM_ADD);
	m_video->SetVertexShader(m_defaultVS);
	m_video->SetPixelShader(ShaderPtr());
	return true;
}

bool ETHShaderManager::EndHaloPass()
{
	m_video->SetPixelShader(ShaderPtr());
	m_video->SetVertexShader(ShaderPtr());
	m_video->SetAlphaMode(m_lastAM);
	return true;
}

bool ETHShaderManager::BeginParticlePass()
{
	m_video->SetVertexShader(m_particle);
	m_video->SetPixelShader(ShaderPtr());
	return true;
}

bool ETHShaderManager::EndParticlePass()
{
	m_video->SetPixelShader(ShaderPtr());
	m_video->SetVertexShader(ShaderPtr());
	return true;
}


void ETHShaderManager::UsePS(const bool usePS)
{
	if (!m_lightingProfiles.empty())
	{
		if (usePS)
			m_currentProfile = FindHighestLightingProfile();
		else
			m_currentProfile = m_lightingProfiles.find(VERTEX_LIGHTING_DIFFUSE)->second;
	}
	else
	{
		m_video->Message(GS_L("ETHShaderManager::UsePS: no lighting profile"), GSMT_WARNING);
	}
}

bool ETHShaderManager::IsUsingPixelShader()
{
	if (m_lightingProfiles.empty())
	{
		m_video->Message(GS_L("ETHShaderManager::IsUsingPixelShader: no lighting profile"), GSMT_WARNING);
		return false;
	}
	return m_currentProfile->IsUsingPixelShader();
}

bool ETHShaderManager::IsPixelLightingSupported()
{
	if (m_lightingProfiles.empty())
	{
		m_video->Message(GS_L("ETHShaderManager::IsPixelLightingSupported: no lighting profile"), GSMT_WARNING);
		return false;
	}
	return (m_lightingProfiles.find(PIXEL_LIGHTING_DIFFUSE_SPECULAR) != m_lightingProfiles.end());
}
