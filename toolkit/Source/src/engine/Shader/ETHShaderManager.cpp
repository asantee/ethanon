#include "ETHShaderManager.h"
#include "../Scene/ETHScene.h"
#include "ETHShaders.h"

ETHShaderManager::ETHShaderManager(VideoPtr video, const str_type::string& shaderPath, const bool richLighting) :
	m_lastAM(Video::AM_PIXEL)
{
	m_video = video;

	m_defaultShader    = m_video->LoadShaderFromString("defaultShaderVS",    ETHShaders::DefaultVS(),      "", "defaultPS", ETHShaders::DefaultPS(), "minimal");
	m_ambientShader    = m_video->LoadShaderFromString("ambientShaderVS",    ETHShaders::Ambient_VS_Hor(), "", "defaultPS", ETHShaders::DefaultPS(), "minimal");
	m_ambientAddShader = m_video->LoadShaderFromString("ambientAddShaderVS", ETHShaders::Ambient_VS_Hor(), "", "defaultPS", ETHShaders::DefaultPS(), "add");
	m_highlightShader  = m_video->LoadShaderFromString("highlightShaderVS",  ETHShaders::Ambient_VS_Hor(), "", "highlightPS", ETHShaders::Highlight_PS(), "");
	m_solidColorShader = m_video->LoadShaderFromString("solidColorShaderVS", ETHShaders::Ambient_VS_Hor(), "", "solidColorPS", ETHShaders::SolidColor_PS(), "");
	m_particleShader   = m_video->LoadShaderFromString("particleShaderVS",   ETHShaders::Particle_VS(),    "", "defaultPS", ETHShaders::DefaultPS(), "minimal");
	m_particleHighlightShader = m_video->LoadShaderFromString("particleHighlightShaderVS", ETHShaders::Particle_VS(), "", "highlightPS", ETHShaders::Highlight_PS(), "");

	m_opaqueSprite = m_video->CreateSprite(ETHGlobal::GetDataResourceFullPath(shaderPath, GS_L("default_nm.png")));

}

bool ETHShaderManager::BeginAmbientPass(const ETHSpriteEntity *pRender, const float maxHeight, const float minHeight)
{
	const bool shouldUseHighlightPS = pRender->ShouldUseHighlightPixelShader();
	const bool shouldUseSolidColorPS = pRender->ShouldUseSolidColorPixelShader();
	const bool shouldUsePass1PS = pRender->ShouldUsePass1AddPixelShader();

	ShaderPtr shader = m_ambientShader;
	if (shouldUseHighlightPS)  shader = m_highlightShader;
	if (shouldUseSolidColorPS) shader = m_solidColorShader;
	if (shouldUsePass1PS)      shader = m_ambientAddShader;

	m_video->SetCurrentShader(shader);

	if (shouldUseHighlightPS)
	{
		m_highlightShader->SetConstant(GS_L("highlight"), pRender->GetColorARGB());
	}

	if (shouldUseSolidColorPS)
	{
		m_solidColorShader->SetConstant(GS_L("solidColor"), pRender->GetSolidColorARGB());
	}

	m_parallaxManager.SetShaderParameters(m_video, m_video->GetCurrentShader(), pRender->GetPosition(), pRender->GetParallaxIntensity(), false);

	m_lastAM = m_video->GetAlphaMode();
	return true;
}

bool ETHShaderManager::EndAmbientPass()
{
	if (m_lastAM != m_video->GetAlphaMode())
		m_video->SetAlphaMode(m_lastAM);
	return true;
}

SpritePtr ETHShaderManager::GetOpaqueSprite()
{
	return m_opaqueSprite;
}

bool ETHShaderManager::BeginHaloPass(const ETHLight* light)
{
	if (!light || light->haloBitmap == GS_L(""))
		return false;

	m_lastAM = m_video->GetAlphaMode();
	m_video->SetAlphaMode(Video::AM_ADD);
	m_video->SetCurrentShader(ShaderPtr());
	return true;
}

bool ETHShaderManager::EndHaloPass()
{
	m_video->SetAlphaMode(m_lastAM);
	return true;
}

bool ETHShaderManager::BeginParticlePass(const ETHParticleSystem& system)
{
	const ShaderPtr& shader = (system.ShouldUseHighlightPS()) ? m_particleHighlightShader : m_particleShader;
	m_video->SetCurrentShader(shader);
	return true;
}

bool ETHShaderManager::EndParticlePass()
{
	return true;
}
