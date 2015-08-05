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

#include "ETHShaderManager.h"
#include "../Scene/ETHScene.h"
#include "ETHPixelLightDiffuseSpecular.h"
#include "ETHShaders.h"

ETHShaderManager::ETHShaderManager(VideoPtr video, const str_type::string& shaderPath, const bool richLighting) :
	m_lastAM(Video::AM_PIXEL),
	m_fakeEyeManager(new ETHFakeEyePositionManager),
	m_richLighting(richLighting)
{
	m_video = video;

	Shader::SHADER_PROFILE sp = Shader::SP_MODEL_2;
	#ifdef OPENGL
	 sp = Shader::SP_MODEL_1;
	#endif

	m_defaultVS = m_video->LoadShaderFromString(GS_L("defaultVS"), ETHShaders::DefaultVS(), Shader::SF_VERTEX, sp);
	m_particle  = m_video->LoadShaderFromString(GS_L("particle"), ETHShaders::Particle_VS(), Shader::SF_VERTEX, sp);
	m_defaultStaticAmbientVS  = m_video->LoadShaderFromString(GS_L("defaultStaticAmbientVS"), ETHShaders::Ambient_VS_Hor(), Shader::SF_VERTEX, sp);
	m_verticalStaticAmbientVS = m_video->LoadShaderFromString(GS_L("verticalStaticAmbientVS"), ETHShaders::Ambient_VS_Ver(), Shader::SF_VERTEX, sp);
	m_shadowVS = m_video->LoadShaderFromString(GS_L("shadowVS"), ETHShaders::Shadow_VS_Ver(), Shader::SF_VERTEX, sp);

	m_highlightPS = m_video->LoadShaderFromString(GS_L("highlightPS"), ETHShaders::Highlight_PS(), Shader::SF_PIXEL, sp);

	#if defined(GLES2) || defined(OPENGL)
		m_projShadow = m_video->CreateSprite(ETHGlobal::GetDataResourceFullPath(shaderPath, GS_L("shadow.png")));
	#else
		m_projShadow = m_video->CreateSprite(ETHGlobal::GetDataResourceFullPath(shaderPath, GS_L("shadow.dds")));
	#endif

	m_opaqueSprite = m_video->CreateSprite(ETHGlobal::GetDataResourceFullPath(shaderPath, GS_L("default_nm.png")));

	if (m_richLighting)
	{
		{ETHLightingProfilePtr profile(new ETHPixelLightDiffuseSpecular(m_video, shaderPath, m_fakeEyeManager));
		if (profile->IsSupportedByHardware())
		{
			m_lightingProfiles[PIXEL_LIGHTING_DIFFUSE_SPECULAR] = profile;
		}}
	}

	if (m_lightingProfiles.empty())
	{
		video->Message(GS_L("ETHShaderManager::ETHShaderManager: no lighting profile"), GSMT_INFO);
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
		m_video->Message(GS_L("ETHShaderManager::FindHighestLightingProfile: no lighting profile"), GSMT_INFO);
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
	const bool shouldUseHighlightPS = pRender->ShouldUseHighlightPixelShader();
	m_video->SetPixelShader(shouldUseHighlightPS ? m_highlightPS : ShaderPtr());

	if (shouldUseHighlightPS)
	{
		m_highlightPS->SetConstant(GS_L("highlight"), pRender->GetColorARGB());
	}

	if (pRender->GetType() == ETHEntityProperties::ET_VERTICAL)
	{
		m_verticalStaticAmbientVS->SetConstant(GS_L("spaceLength"), (maxHeight-minHeight));
		m_video->SetVertexShader(m_verticalStaticAmbientVS);
	}
	else
	{
		m_video->SetVertexShader(m_defaultStaticAmbientVS);
	}
	m_parallaxManager.SetShaderParameters(m_video, m_video->GetVertexShader(), pRender->GetPosition(), pRender->GetParallaxIntensity(), false);

	m_lastAM = m_video->GetAlphaMode();
	return true;
}

bool ETHShaderManager::EndAmbientPass()
{
	if (m_lastAM != m_video->GetAlphaMode())
		m_video->SetAlphaMode(m_lastAM);
	
	m_video->SetPixelShader(ShaderPtr());
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
	m_parallaxManager.SetShaderParameters(m_video, m_video->GetVertexShader(), pRender->GetPosition(), pRender->GetParallaxIntensity(), drawToTarget);
	return true;
}

SpritePtr ETHShaderManager::GetOpaqueSprite()
{
	return m_opaqueSprite;
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

bool ETHShaderManager::BeginHaloPass(const ETHLight* light)
{
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

bool ETHShaderManager::BeginParticlePass(const ETHParticleSystem& system)
{
	m_video->SetVertexShader(m_particle);
	
	const ShaderPtr& ps = (system.ShouldUseHighlightPS()) ? m_highlightPS : ShaderPtr();
	m_video->SetPixelShader(ps);
	return true;
}

bool ETHShaderManager::EndParticlePass()
{
	m_video->SetPixelShader(ShaderPtr());
	m_video->SetVertexShader(ShaderPtr());
	return true;
}
