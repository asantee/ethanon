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

#include "ParticleScale.h"

void cParticleScale::LoadEditor(EditorBase *pEditor)
{
	VideoPtr video = pEditor->GetVideoHandler();
	InputPtr input = pEditor->GetInputHandler();

	for (unsigned int t=0; t<ETH_MAX_PARTICLE_SYS_PER_ENTITY; t++)
	{
		m_scaleInput[t].SetupMenu(video, input, pEditor->GetMenuSize(), pEditor->GetMenuWidth(), 7, false);
		m_scaleInput[t].SetConstant(1.0f);
		m_scaleInput[t].SetText(L"Particle scale");
		m_scaleInput[t].SetScrollAdd(0.2f);
	}
}

float cParticleScale::DoEditor(const Vector2 &v2Pos, ETHEntity* pEntity, EditorBase *pEditor)
{
	float fR=0.0f;
	if (pEntity->HasParticleSystems())
	{
		fR += pEditor->GetMenuSize();
		pEditor->ShadowPrint(v2Pos+Vector2(0,fR), L"Particle scale:"); fR += pEditor->GetMenuSize();
		for (unsigned int t=0; t<ETH_MAX_PARTICLE_SYS_PER_ENTITY; t++)
		{
			if (pEntity->HasParticleSystem(t))
			{
				float fValue = m_scaleInput[t].PlaceInput(v2Pos+Vector2(0,fR), Vector2(0,0), pEditor->GetMenuSize());
				if (fValue != 1.0f && !m_scaleInput[t].IsActive())
				{
					fValue = Max(0.1f, fValue);
					pEntity->ScaleParticleSystem(t, fValue);
					pEntity->ScaleParticleSystemOrigin(t, fValue);
					m_scaleInput[t].SetConstant(1.0f);
				}
				fR += pEditor->GetMenuSize();
			}
		}
	}
	return fR;
}

bool cParticleScale::IsCursorAvailable() const
{
	for (unsigned int t=0; t<ETH_MAX_PARTICLE_SYS_PER_ENTITY; t++)
	{
		if (m_scaleInput[t].IsActive() || m_scaleInput[t].IsMouseOver())
			return false;
	}
	return true;
}

bool cParticleScale::IsFieldActive() const
{
	for (unsigned int t=0; t<ETH_MAX_PARTICLE_SYS_PER_ENTITY; t++)
	{
		if (m_scaleInput[t].IsActive())
			return true;
	}
	return false;
}
