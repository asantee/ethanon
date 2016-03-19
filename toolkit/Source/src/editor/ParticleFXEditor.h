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

#ifndef PARTICLE_EDITOR_HEADER_34235325
#define PARTICLE_EDITOR_HEADER_34235325

#include "EditorBase.h"

#include <Video.h>
#include <Audio.h>
using namespace gs2d::math;
using namespace gs2d;

#include <stdlib.h>

#include "../engine/Particles/ETHParticleManager.h"
#include "../engine/Util/ETHSpeedTimer.h"
#include "gsgui.h"

class ParticleEditor : public EditorBase
{
	ETHParticleManagerPtr m_manager;
	ETHParticleSystem m_system;
	SpritePtr m_backgroundSprite, m_sphereSprite;
	Vector2 m_v2Pos;
	float m_systemAngle;
	bool m_projManagerRequested;

	GSGUI_INT_INPUT m_particles;
	GSGUI_INT_INPUT m_repeats;

	GSGUI_FLOAT_INPUT m_gravity[2];
	GSGUI_FLOAT_INPUT m_direction[2];
	GSGUI_FLOAT_INPUT m_randDir[2];

	GSGUI_FLOAT_INPUT m_startPoint[2];
	GSGUI_FLOAT_INPUT m_randStart[2];

	GSGUI_FLOAT_INPUT m_color0[4];
	GSGUI_FLOAT_INPUT m_color1[4];
	GSGUI_FLOAT_INPUT m_luminance[3];

	GSGUI_FLOAT_INPUT m_size;
	GSGUI_FLOAT_INPUT m_growth;
	GSGUI_FLOAT_INPUT m_lifeTime;
	GSGUI_FLOAT_INPUT m_randLifeTime;
	GSGUI_FLOAT_INPUT m_angle;
	GSGUI_FLOAT_INPUT m_randAngle;
	GSGUI_FLOAT_INPUT m_randSize;
	GSGUI_FLOAT_INPUT m_randAngleStart;
	GSGUI_FLOAT_INPUT m_minSize;
	GSGUI_FLOAT_INPUT m_maxSize;
	GSGUI_FLOAT_INPUT m_angleStart;
	GSGUI_FLOAT_INPUT m_allAtOnce;

	GSGUI_FLOAT_INPUT m_spriteCut[2];

	GSGUI_BUTTONLIST m_alphaModes;
	GSGUI_BUTTONLIST m_animationModes;

	GSGUI_DROPDOWN m_fileMenu;

	void CreateParticles();
	void DrawParticleSystem();
	void SetupMenu();
	void SetMenuConstants();
	void SaveAs();
	void Save();
	void ParticlePanel();
	bool OpenParticleBMP(char *filePathName, char *fileName);
	bool OpenSystem(char *filePathName, char *fileName);
	bool SaveSystem(char *filePathName, char *fileName);
	void ResetSystem();

	const Color BSPHERE_COLOR;
	const std::string BSPHERE_BMP;

	ETHSpeedTimer m_timer;

public:
	ParticleEditor(ETHResourceProviderPtr provider);
	~ParticleEditor();
	std::string DoEditor(SpritePtr pNextAppButton);
	void StopAllSoundFXs();
	void LoadEditor();
	void Clear();
	bool ProjectManagerRequested();
};

#endif
