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

#include "ETHParticleDrawer.h"
#include "../Resource/ETHResourceProvider.h"
#include "../Resource/ETHDirectories.h"

ETHParticleDrawer::ETHParticleDrawer(
	const ETHResourceProviderPtr& provider,
	ETHGraphicResourceManagerPtr graphicResources,
	ETHShaderManagerPtr shaderManager,
	const str_type::string& resourceDirectory,
	const str_type::string& fileName,
	const Vector2& pos,
	const float angle,
	const float scale) :
	m_shaderManager(shaderManager),
	m_pos(pos),
	m_angle(angle),
	m_provider(provider),
	m_fileName(fileName)
{
	ETHParticleSystem particleSystem;
	particleSystem.ReadFromFile(resourceDirectory + ETHDirectories::GetEffectsDirectory() + fileName, provider->GetFileManager());
	const Vector3 startPos(Vector3(m_pos + provider->GetVideo()->GetCameraPos(), 0.0f));
	m_particleManager = ETHParticleManagerPtr(
		new ETHParticleManager(provider, particleSystem, startPos, angle, scale));
}

bool ETHParticleDrawer::Draw(const unsigned long lastFrameElapsedTimeMS)
{
	m_particleManager->UpdateParticleSystem(
		Vector3(m_pos, 0.0f),
		Vector2(0.0f, 0.0f),
		Vector2(0.0f, 0.0f),
		m_angle,
		static_cast<float>(lastFrameElapsedTimeMS));

	if (m_shaderManager->BeginParticlePass(*m_particleManager->GetSystem()))
	{
		m_particleManager->DrawParticleSystem(
			Vector3(1.0f, 1.0f, 1.0f),
			1.0f,
			0.0f,
			ETHParticleManager::LAYERABLE,
			Vector2(0.0f, 0.0f),
			Vector2(0.5f, 0.5f),
			0.0f);
		m_shaderManager->EndParticlePass();
	}
	return true;
}

bool ETHParticleDrawer::IsAlive() const
{
	if (m_particleManager->GetSystem()->repeat <= 0)
	{
		m_provider->Log(m_fileName + GS_L(" rendering failed. Only temporary particle effects are allowed"), Platform::Logger::ERROR);
		return false;
	}
	return !m_particleManager->Finished();
}
