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

#include "EntityEditor.h"
#include <unicode/utf8converter.h>
#include "../engine/Entity/ETHRenderEntity.h"
#include <sstream>

using namespace std;

#define _S_ADD_RESOURCES L"Add resources"
#define _S_OPEN_ENTITY L"Add entity bitmap"
#define _S_OPEN_NORMALMAP L"Add normal map"
#define _S_OPEN_HALO L"Add halo bitmap"
#define _S_OPEN_GLOSSMAP L"Add gloss bitmap"
#define _S_LOAD_PAR0 L"Add particle slot A"
#define _S_LOAD_PAR1 L"Add particle slot B"

#define _S_DEL_RESOURCES L"Remove..."
#define _S_DEL_ENTITY L"Entity bitmap"
#define _S_DEL_NORMALMAP L"Normal map"
#define _S_DEL_GLOSSMAP L"Gloss map"
#define _S_DEL_HALO L"Halo"
#define _S_DEL_PAR0 L"Particle slot A"
#define _S_DEL_PAR1 L"Particle slot B"

#define _S_VERTICAL L"Vertical"
#define _S_HORIZONTAL L"Horizontal"
#define _S_DECAL L"Ground decal"
#define _S_OVERALL L"Over all"
#define _S_OPAQUE_DECAL L"Opaque decal"
#define _S_LAYERABLE L"Layerable"

#define _S_BODY_SHAPE          L"Physics properties:"
#define _S_BODY_SHAPE_NONE     L"Not physics managed"
#define _S_BODY_SHAPE_BOX      L"Box"
#define _S_BODY_SHAPE_CIRCLE   L"Circle"
#define _S_BODY_SHAPE_POLYGON  L"Polygon"
#define _S_BODY_SHAPE_COMPOUND L"Compound shape"

#define _S_BODY_PROPS                L"Body properties:"
#define _S_BODY_PROPS_SENSOR         L"Is sensor"
#define _S_BODY_PROPS_BULLET         L"Is bullet"
#define _S_BODY_PROPS_FIXED_ROTATION L"Fixed rotation"
#define _S_BODY_PROPS_DENSITY        L"Density"
#define _S_BODY_PROPS_RESTITUTION    L"Restitution"
#define _S_BODY_PROPS_FRICTION       L"Friction"

#define _S_CAST_SHADOW L"Cast shadow"
#define _S_STATIC_ENTITY L"Static entity"
#define _S_APPLY_LIGHT L"Apply light"
#define _S_ACTIVE_LIGHT L"Cast light"
#define _S_SHADOW_CASTER L"Shadow caster"
#define _S_STATIC_LIGHT L"Static light"
#define _S_COLLIDABLE L"Collidable"

#define _S_EDIT_LIGHT L"Edit lights"
#define _S_EDIT_PARTICLES L"Edit particles"
#define _S_EDIT_COLLISION L"Edit collision"

#define _S_BLEND_MODE_DEFAULT L"Default"
#define _S_BLEND_MODE_ALPHATEST L"Alpha test only"
#define _S_BLEND_MODE_ADD L"Additive"
#define _S_BLEND_MODE_MODULATE L"Modulate"

EntityEditor::EntityEditor(ETHResourceProviderPtr provider) :
	EditorBase(provider),
	m_lastStartFrame(0),
	m_v2LastSpriteCut(Vector2i(1,1)),
	m_cursorLight(true)
{
	m_projManagerRequested = false;
}

EntityEditor::~EntityEditor()
{
	UnloadSprite();
	UnloadNormal();
	UnloadGloss();
	UnloadHalo();
	for (int t=0; t<ETH_MAX_PARTICLE_SYS_PER_ENTITY; t++)
		UnloadParticle(t);
}

void EntityEditor::InstantiateEntity(const std::wstring& fileName)
{
	if (fileName == L"")
	{
		m_renderEntity = boost::shared_ptr<ETHRenderEntity>(new ETHRenderEntity(m_provider));
	}
	else
	{
		m_renderEntity = boost::shared_ptr<ETHRenderEntity>(new ETHRenderEntity(fileName, m_provider));
	}
	m_pEditEntity = m_renderEntity->GetEditableProperties();
	m_pEditEntity->particleSystems.resize(2);

	if (!m_pEditEntity->particleSystems[0])
		m_pEditEntity->particleSystems[0] = boost::shared_ptr<ETH_PARTICLE_SYSTEM>(new ETH_PARTICLE_SYSTEM);
	if (!m_pEditEntity->particleSystems[1])
		m_pEditEntity->particleSystems[1] = boost::shared_ptr<ETH_PARTICLE_SYSTEM>(new ETH_PARTICLE_SYSTEM);
}

void EntityEditor::StopAllSoundFXs()
{
	m_renderEntity->ForceSFXStop();
}

bool EntityEditor::ProjectManagerRequested()
{
	const bool r = m_projManagerRequested;
	m_projManagerRequested = false;
	return r;
}

void EntityEditor::LoadEditor()
{
	CreateFileMenu();

	m_cursorLight.castShadows = true;
	m_cursorLight.color = Vector3(1,1,1);
	m_cursorLight.haloBrightness = 0.0f;
	m_cursorLight.haloSize = 0.0f;
	m_cursorLight.pos = Vector3(0,0,20);
	m_cursorLight.range = 800.0f;
	m_cursorLight.staticLight = false;

	VideoPtr video = m_provider->GetVideo();
	video->SetBGColor(m_background);

	InstantiateEntity();

	m_axis = video->CreateSprite(m_provider->GetProgramPath() + L"data/axis.png");
	m_axis->SetOrigin(GSEO_CENTER);

	m_spot.m_sprite = video->CreateSprite(m_provider->GetProgramPath() + L"data/spot.bmp", 0xFFFF00FF);
	m_spot.m_sprite->SetOrigin(GSEO_CENTER);

	m_range = video->CreateSprite(m_provider->GetProgramPath() + L"data/range.png");
	m_range->SetOrigin(GSEO_CENTER);

	m_outline = video->CreateSprite(m_provider->GetProgramPath() + L"data/outline.png");
	m_outline->SetOrigin(GSEO_DEFAULT);

	for (int t=0; t<ETH_MAX_PARTICLE_SYS_PER_ENTITY; t++)
	{
		m_particleSpot[t].m_sprite = video->CreateSprite(m_provider->GetProgramPath() + L"data/arrow.bmp", 0xFFFF00FF);
		m_particleSpot[t].m_sprite->SetOrigin(GSEO_CENTER);
	}

	m_playStopButton.SetupButtons(video, m_provider->GetInput());
	m_playStopButton.AddButton(m_provider->GetProgramPath() + L"data/play.png");
	m_playStopButton.AddButton(m_provider->GetProgramPath() + L"data/stop.png");

	m_addMenu.SetupMenu(video, m_provider->GetInput(), _S_ADD_RESOURCES, m_menuSize*m_menuScale, m_menuWidth*2, true);
	m_addMenu.AddButton(_S_OPEN_ENTITY);
	m_addMenu.AddButton(_S_OPEN_NORMALMAP);
	m_addMenu.AddButton(_S_OPEN_HALO);
	m_addMenu.AddButton(_S_OPEN_GLOSSMAP);
	m_addMenu.AddButton(_S_LOAD_PAR0);
	m_addMenu.AddButton(_S_LOAD_PAR1);

	m_delMenu.SetupMenu(video, m_provider->GetInput(), _S_DEL_RESOURCES, m_menuSize*m_menuScale, m_menuWidth*2, true);
	m_delMenu.AddButton(_S_DEL_ENTITY);
	m_delMenu.AddButton(_S_DEL_NORMALMAP);
	m_delMenu.AddButton(_S_DEL_HALO);
	m_delMenu.AddButton(_S_DEL_GLOSSMAP);
	m_delMenu.AddButton(_S_DEL_PAR0);
	m_delMenu.AddButton(_S_DEL_PAR1);

	m_tool.SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth*2, true, true, false);
	m_tool.AddButton(_S_EDIT_LIGHT, false);
	m_tool.AddButton(_S_EDIT_PARTICLES, false);
	m_tool.AddButton(_S_EDIT_COLLISION, false);

	m_pivotX.SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 6, false);
	m_pivotX.SetConstant(m_pEditEntity->pivotAdjust.x);
	m_pivotX.SetClamp(false, 0, 0);
	m_pivotX.SetText(L"Pivot x:");
	m_pivotX.SetScrollAdd(1.0f);
	m_pivotX.SetDescription(L"Pivot adjustment");

	m_pivotY.SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 6, false);
	m_pivotY.SetConstant(m_pEditEntity->pivotAdjust.y);
	m_pivotY.SetClamp(false, 0, 0);
	m_pivotY.SetText(L"Pivot y:");
	m_pivotY.SetScrollAdd(1.0f);
	m_pivotY.SetDescription(L"Pivot adjustment");

	m_lightColor[0].SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 8, false);
	//m_lightColor[0].SetConstant(m_pEditEntity->light.color.x);
	m_lightColor[0].SetConstant(1.0f);
	m_lightColor[0].SetClamp(true, 0, 2);
	m_lightColor[0].SetText(L"Light.Red:");
	m_lightColor[0].SetScrollAdd(0.1f);
	m_lightColor[0].SetDescription(L"Light color");

	m_lightColor[1].SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 8, false);
	//m_lightColor[1].SetConstant(m_pEditEntity->light.color.y);
	m_lightColor[1].SetConstant(1.0f);
	m_lightColor[1].SetClamp(true, 0, 2);
	m_lightColor[1].SetText(L"Light.Green:");
	m_lightColor[1].SetScrollAdd(0.1f);
	m_lightColor[1].SetDescription(L"Light color");

	m_lightColor[2].SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 8, false);
	//m_lightColor[2].SetConstant(m_pEditEntity->light.color.z);
	m_lightColor[2].SetConstant(1.0f);
	m_lightColor[2].SetClamp(true, 0, 2);
	m_lightColor[2].SetText(L"Light.Blue:");
	m_lightColor[2].SetScrollAdd(0.1f);
	m_lightColor[2].SetDescription(L"Light color");

	m_emissiveColor[0].SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 8, false);
	m_emissiveColor[0].SetConstant(m_pEditEntity->emissiveColor.x);
	m_emissiveColor[0].SetClamp(true, 0, 1);
	m_emissiveColor[0].SetText(L"Emissive.r:");
	m_emissiveColor[0].SetScrollAdd(0.1f);
	m_emissiveColor[0].SetDescription(L"Emissive color");

	m_emissiveColor[1].SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 8, false);
	m_emissiveColor[1].SetConstant(m_pEditEntity->emissiveColor.y);
	m_emissiveColor[1].SetClamp(true, 0, 1);
	m_emissiveColor[1].SetText(L"Emissive.g:");
	m_emissiveColor[1].SetScrollAdd(0.1f);
	m_emissiveColor[1].SetDescription(L"Emissive color");

	m_emissiveColor[2].SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 8, false);
	m_emissiveColor[2].SetConstant(m_pEditEntity->emissiveColor.z);
	m_emissiveColor[2].SetClamp(true, 0, 1);
	m_emissiveColor[2].SetText(L"Emissive.b:");
	m_emissiveColor[2].SetScrollAdd(0.1f);
	m_emissiveColor[2].SetDescription(L"Emissive color");

	m_lightRange.SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 5, false);
	//m_lightRange.SetConstant(m_pEditEntity->light.range);
	m_lightRange.SetConstant(256.0f);
	m_lightRange.SetClamp(true, 1, 9999999.0f);
	m_lightRange.SetText(L"Range:");
	m_lightRange.SetScrollAdd(1.5f);
	m_lightRange.SetDescription(L"Light range");

	m_renderMode.SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth*2, true, false, false);
	m_renderMode.AddButton(_S_USE_PS, true);
	m_renderMode.AddButton(_S_USE_VS, false);

	m_haloSize.SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 5, false);
	//m_haloSize.SetConstant(m_pEditEntity->light.haloSize);
	m_haloSize.SetConstant(128.0f);
	m_haloSize.SetClamp(true, 1.0f, 9999999.0f);
	m_haloSize.SetText(L"Halo size:");
	m_haloSize.SetScrollAdd(2.0f);
	m_haloSize.SetDescription(L"Halo size");

	m_haloAlpha.SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 5, false);
	//m_haloAlpha.SetConstant(m_pEditEntity->light.haloBrightness);
	m_haloAlpha.SetConstant(1.0f);
	m_haloAlpha.SetClamp(true, 0, 1);
	m_haloAlpha.SetText(L"Halo alpha:");
	m_haloAlpha.SetScrollAdd(0.05f);
	m_haloAlpha.SetDescription(L"Halo brightness");

	m_shadowScale.SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 6, false);
	m_shadowScale.SetConstant(m_pEditEntity->shadowScale);
	m_shadowScale.SetClamp(true, 0, 9999999.0f);
	m_shadowScale.SetText(L"Scale:");
	m_shadowScale.SetScrollAdd(0.1f);
	m_shadowScale.SetDescription(L"Shadow scale");

	m_shadowOpacity.SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 6, false);
	m_shadowOpacity.SetConstant(m_pEditEntity->shadowOpacity);
	m_shadowOpacity.SetClamp(true, 0, 1.0f);
	m_shadowOpacity.SetText(L"Opacity:");
	m_shadowOpacity.SetScrollAdd(0.1f);
	m_shadowOpacity.SetDescription(L"Shadow opacity level");

	m_shadowLength.SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 6, false);
	m_shadowLength.SetConstant(m_pEditEntity->shadowLengthScale);
	m_shadowLength.SetClamp(false, 0, 0);
	m_shadowLength.SetText(L"Length scale:");
	m_shadowLength.SetScrollAdd(0.1f);
	m_shadowLength.SetDescription(L"Shadow length scale");

	m_soundVolume.SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 6, false);
	m_soundVolume.SetConstant(m_pEditEntity->soundVolume);
	m_soundVolume.SetClamp(true, 0, 1.0f);
	m_soundVolume.SetText(L"Sound volume:");
	m_soundVolume.SetScrollAdd(0.1f);
	m_soundVolume.SetDescription(L"Entity general sound volume");

	m_lightPos[0].SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 8, false);
	//m_lightPos[0].SetConstant(m_pEditEntity->light.pos.x);
	m_lightPos[0].SetConstant(0.0f);
	m_lightPos[0].SetClamp(false, 0, 0);
	m_lightPos[0].SetText(L"Light.pos.x:");
	m_lightPos[0].SetScrollAdd(2.0f);
	m_lightPos[0].SetDescription(L"Light position");

	m_lightPos[1].SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 8, false);
	//m_lightPos[1].SetConstant(m_pEditEntity->light.pos.y);
	m_lightPos[1].SetConstant(0.0f);
	m_lightPos[1].SetClamp(false, 0, 0);
	m_lightPos[1].SetText(L"Light.pos.y:");
	m_lightPos[1].SetScrollAdd(2.0f);
	m_lightPos[1].SetDescription(L"Light position");

	m_lightPos[2].SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 8, false);
	//m_lightPos[2].SetConstant(m_pEditEntity->light.pos.z);
	m_lightPos[2].SetConstant(0.0f);
	m_lightPos[2].SetClamp(false, 0, 0);
	m_lightPos[2].SetText(L"Light.pos.z:");
	m_lightPos[2].SetScrollAdd(2.0f);
	m_lightPos[2].SetDescription(L"Light position");

	m_startFrame.SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 8, false);
	m_startFrame.SetConstant((float)m_pEditEntity->startFrame);
	m_startFrame.SetClamp(false, 0, 0);
	m_startFrame.SetText(L"Start frame");
	m_startFrame.SetScrollAdd(1.0f);
	m_startFrame.SetDescription(L"Starting frame in the sprite sheet");

	m_spriteCut[0].SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 8, false);
	m_spriteCut[0].SetConstant((float)m_pEditEntity->spriteCut.x);
	m_spriteCut[0].SetClamp(true, 1, 2048);
	m_spriteCut[0].SetText(L"Sprite columns");
	m_spriteCut[0].SetScrollAdd(1.0f);
	m_spriteCut[0].SetDescription(L"Number of columns in the sprite sheet");

	m_spriteCut[1].SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 8, false);
	m_spriteCut[1].SetConstant((float)m_pEditEntity->spriteCut.y);
	m_spriteCut[1].SetClamp(true, 1, 2048);
	m_spriteCut[1].SetText(L"Sprite rows");
	m_spriteCut[1].SetScrollAdd(1.0f);
	m_spriteCut[1].SetDescription(L"Number of rows in the sprite sheet");

	m_collisionPos[0].SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 8, false);
	//m_collisionPos[0].SetConstant(m_pEditEntity->collision.pos.x);
	m_collisionPos[0].SetConstant(0.0f);
	m_collisionPos[0].SetClamp(false, 0, 0);
	m_collisionPos[0].SetText(L"Box.pos.x:");
	m_collisionPos[0].SetScrollAdd(1.0f);
	m_collisionPos[0].SetDescription(L"Collision box position");

	m_collisionPos[1].SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 8, false);
	//m_collisionPos[1].SetConstant(m_pEditEntity->collision.pos.y);
	m_collisionPos[1].SetConstant(0.0f);
	m_collisionPos[1].SetClamp(false, 0, 0);
	m_collisionPos[1].SetText(L"Box.pos.y:");
	m_collisionPos[1].SetScrollAdd(1.0f);
	m_collisionPos[1].SetDescription(L"Collision box position");

	m_collisionPos[2].SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 8, false);
	//m_collisionPos[2].SetConstant(m_pEditEntity->collision.pos.z);
	m_collisionPos[2].SetConstant(0.0f);
	m_collisionPos[2].SetClamp(false, 0, 0);
	m_collisionPos[2].SetText(L"Box.pos.z:");
	m_collisionPos[2].SetScrollAdd(1.0f);
	m_collisionPos[2].SetDescription(L"Collision box position");

	m_collisionSize[0].SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 8, false);
	//m_collisionSize[0].SetConstant(m_pEditEntity->collision.size.x);
	m_collisionSize[0].SetConstant(10.0f);
	m_collisionSize[0].SetClamp(true, 0, 2048);
	m_collisionSize[0].SetText(L"Box.size.x:");
	m_collisionSize[0].SetScrollAdd(1.0f);
	m_collisionSize[0].SetDescription(L"Collision box size");

	m_collisionSize[1].SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 8, false);
	//m_collisionSize[1].SetConstant(m_pEditEntity->collision.size.y);
	m_collisionSize[1].SetConstant(10.0f);
	m_collisionSize[1].SetClamp(true, 0, 2048);
	m_collisionSize[1].SetText(L"Box.size.y:");
	m_collisionSize[1].SetScrollAdd(1.0f);
	m_collisionSize[1].SetDescription(L"Collision box size");

	m_collisionSize[2].SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 8, false);
	//m_collisionSize[2].SetConstant(m_pEditEntity->collision.size.z);
	m_collisionSize[2].SetConstant(10.0f);
	m_collisionSize[2].SetClamp(true, 0, 2048);
	m_collisionSize[2].SetText(L"Box.size.z:");
	m_collisionSize[2].SetScrollAdd(1.0f);
	m_collisionSize[2].SetDescription(L"Collision box size");

	m_layerDepth.SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 6, false);
	m_layerDepth.SetConstant(m_pEditEntity->layerDepth);
	m_layerDepth.SetClamp(true, 0, 1.0f);
	m_layerDepth.SetText(L"Layer depth:");
	m_layerDepth.SetScrollAdd(0.1f);
	m_layerDepth.SetDescription(L"Default layer depth for the entity (0=furthest and 1=closest)");

	m_specularPower.SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 6, false);
	m_specularPower.SetConstant(m_pEditEntity->specularPower);
	m_specularPower.SetClamp(true, 1, 999999.0f);
	m_specularPower.SetText(L"Spec. power:");
	m_specularPower.SetScrollAdd(10.0f);
	m_specularPower.SetDescription(L"Specular power (higher numbers for shinier surface)");

	m_specularBrightness.SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 6, false);
	m_specularBrightness.SetConstant(m_pEditEntity->specularBrightness);
	m_specularBrightness.SetClamp(true, 0, 2);
	m_specularBrightness.SetText(L"Specular:");
	m_specularBrightness.SetScrollAdd(0.1f);
	m_specularBrightness.SetDescription(L"Specular component brightness");

	m_density.SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 9, false);
	m_density.SetConstant(m_pEditEntity->density);
	m_density.SetClamp(true, 0, 999999.0f);
	m_density.SetText(_S_BODY_PROPS_DENSITY);
	m_density.SetScrollAdd(0.1f);
	m_density.SetDescription(L"Body density");

	m_friction.SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 9, false);
	m_friction.SetConstant(m_pEditEntity->friction);
	m_friction.SetClamp(true, 0, 2.0f);
	m_friction.SetText(_S_BODY_PROPS_FRICTION);
	m_friction.SetScrollAdd(0.05f);
	m_friction.SetDescription(L"Body surface friction");

	m_restitution.SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 9, false);
	m_restitution.SetConstant(m_pEditEntity->restitution);
	m_restitution.SetClamp(true, 0, 2.0f);
	m_restitution.SetText(_S_BODY_PROPS_RESTITUTION);
	m_restitution.SetScrollAdd(0.05f);
	m_restitution.SetDescription(L"Body restitution");

	m_animPreviewStride.SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 7, false);
	m_animPreviewStride.SetConstant(200.0f);
	m_animPreviewStride.SetClamp(true, 0, 9999999.0f);
	m_animPreviewStride.SetText(L"Stride (ms):");
	m_animPreviewStride.SetScrollAdd(50.0f);
	m_animPreviewStride.SetDescription(L"Animation preview stride in ms");

	for (int t=0; t<ETH_MAX_PARTICLE_SYS_PER_ENTITY; t++)
	{
		m_particlePos[t][0].SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 5, false);
		//m_particlePos[t][0].SetConstant(m_pEditEntity->particle[t].v3StartPoint.x);
		m_particlePos[t][0].SetConstant(0.0f);
		m_particlePos[t][0].SetClamp(false, 0, 1);
		m_particlePos[t][0].SetText(L"Particle.x:");
		m_particlePos[t][0].SetScrollAdd(2.0f);
		m_particlePos[t][0].SetDescription(L"Particle system x position");

		m_particlePos[t][1].SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 5, false);
		//m_particlePos[t][1].SetConstant(m_pEditEntity->particle[t].v3StartPoint.y);
		m_particlePos[t][1].SetConstant(0.0f);
		m_particlePos[t][1].SetClamp(false, 0, 1);
		m_particlePos[t][1].SetText(L"Particle.y:");
		m_particlePos[t][1].SetScrollAdd(2.0f);
		m_particlePos[t][1].SetDescription(L"Particle system y position");

		m_particlePos[t][2].SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth, 5, false);
		//m_particlePos[t][2].SetConstant(m_pEditEntity->particle[t].v3StartPoint.z);
		m_particlePos[t][2].SetConstant(0.0f);
		m_particlePos[t][2].SetClamp(false, 0, 1);
		m_particlePos[t][2].SetText(L"Particle.z:");
		m_particlePos[t][2].SetScrollAdd(2.0f);
		m_particlePos[t][2].SetDescription(L"Particle system z position");
	}

	m_attachLight.SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth*2, true, true, false);

	Vector2 screenMid = video->GetScreenSizeF()/2.0f;
	ResetEntityMenu();

	// load editor plug-ins
	m_customDataEditor.LoadEditor(this);
	m_particleScale.LoadEditor(this);
}

void EntityEditor::CreateCollisionBoxFromEntity()
{
	SpritePtr pSprite = m_renderEntity->GetSprite();

	if (!pSprite)
		return;

	const Vector2 v2CurrentSize = m_renderEntity->GetCurrentSize();

	Vector3 v3Size, v3Pos(m_pEditEntity->pivotAdjust.x, m_pEditEntity->pivotAdjust.y,0);
	if (m_pEditEntity->type == ETH_VERTICAL)
	{
		v3Size.x = v2CurrentSize.x;
		v3Size.y = v2CurrentSize.x;
		v3Size.z = v2CurrentSize.y;
	}
	else
	{
		v3Size.x = v2CurrentSize.x;
		v3Size.y = v2CurrentSize.y;
		v3Size.z = 1.0f;
	}

	m_collisionPos[0].SetConstant(v3Pos.x);
	m_collisionPos[1].SetConstant(v3Pos.y);
	m_collisionPos[2].SetConstant(v3Pos.z);
	m_collisionSize[0].SetConstant(v3Size.x);
	m_collisionSize[1].SetConstant(v3Size.y);
	m_collisionSize[2].SetConstant(v3Size.z);
}

void EntityEditor::ResetEntityMenu()
{
	VideoPtr video = m_provider->GetVideo();

	m_type.Destroy();
	m_type.SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth*2, true, false, false);
	m_type.AddButton(_S_HORIZONTAL, (m_pEditEntity->type == ETH_HORIZONTAL));
	m_type.AddButton(_S_VERTICAL, (m_pEditEntity->type == ETH_VERTICAL));
	m_type.AddButton(_S_LAYERABLE, (m_pEditEntity->type == ETH_LAYERABLE));

	m_bodyShape.Destroy();
	m_bodyShape.SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth*2, true, false, false);
	m_bodyShape.AddButton(_S_BODY_SHAPE_NONE, (m_pEditEntity->shape == ETHBS_NONE));
	m_bodyShape.AddButton(_S_BODY_SHAPE_BOX, (m_pEditEntity->shape == ETHBS_BOX));
	m_bodyShape.AddButton(_S_BODY_SHAPE_CIRCLE, (m_pEditEntity->shape == ETHBS_CIRCLE));
	m_bodyShape.AddButton(_S_BODY_SHAPE_POLYGON, (m_pEditEntity->shape == ETHBS_POLYGON));
	m_bodyShape.AddButton(_S_BODY_SHAPE_COMPOUND, (m_pEditEntity->shape == ETHBS_COMPOUND));

	m_bodyProperties.Destroy();
	m_bodyProperties.SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth * 2, false, true, false);
	m_bodyProperties.AddButton(_S_BODY_PROPS_SENSOR, ETHGlobal::ToBool(m_pEditEntity->sensor));
	m_bodyProperties.AddButton(_S_BODY_PROPS_FIXED_ROTATION, ETHGlobal::ToBool(m_pEditEntity->fixedRotation));
	m_bodyProperties.AddButton(_S_BODY_PROPS_BULLET, ETHGlobal::ToBool(m_pEditEntity->bullet));

	m_density.SetConstant(m_pEditEntity->density);
	m_friction.SetConstant(m_pEditEntity->friction);
	m_restitution.SetConstant(m_pEditEntity->restitution);

	m_bool.Destroy();
	m_bool.SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth*2, false, true, false);
	m_bool.AddButton(_S_CAST_SHADOW, ETHGlobal::ToBool(m_pEditEntity->castShadow));
	m_bool.AddButton(_S_STATIC_ENTITY, ETHGlobal::ToBool(m_pEditEntity->staticEntity));
	m_bool.AddButton(_S_COLLIDABLE, m_pEditEntity->collision);
	m_bool.AddButton(_S_APPLY_LIGHT, ETHGlobal::ToBool(m_pEditEntity->applyLight));
	m_bool.AddButton(_S_ACTIVE_LIGHT, m_pEditEntity->light);

	m_blendMode.Destroy();
	m_blendMode.SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth*2, true, false, false);
	m_blendMode.AddButton(_S_BLEND_MODE_DEFAULT, (m_pEditEntity->blendMode == GSAM_PIXEL));
	m_blendMode.AddButton(_S_BLEND_MODE_ALPHATEST, (m_pEditEntity->blendMode == GSAM_ALPHA_TEST));
	m_blendMode.AddButton(_S_BLEND_MODE_ADD, (m_pEditEntity->blendMode == GSAM_ADD));
	m_blendMode.AddButton(_S_BLEND_MODE_MODULATE, (m_pEditEntity->blendMode == GSAM_MODULATE));

	m_boolLight.Destroy();
	if (m_pEditEntity->light)
	{
		m_boolLight.SetupMenu(video, m_provider->GetInput(), m_menuSize, m_menuWidth*2, false, true, false);
		m_boolLight.AddButton(_S_SHADOW_CASTER, ETHGlobal::ToBool(m_pEditEntity->light->castShadows));
		m_boolLight.AddButton(_S_STATIC_LIGHT, ETHGlobal::ToBool(m_pEditEntity->staticEntity));

		m_lightColor[0].SetConstant(m_pEditEntity->light->color.x);
		m_lightColor[1].SetConstant(m_pEditEntity->light->color.y);
		m_lightColor[2].SetConstant(m_pEditEntity->light->color.z);

		m_lightRange.SetConstant(m_pEditEntity->light->range);
		m_haloSize.SetConstant(m_pEditEntity->light->haloSize);
		m_haloAlpha.SetConstant(m_pEditEntity->light->haloBrightness);
		m_lightPos[0].SetConstant(m_pEditEntity->light->pos.x);
		m_lightPos[1].SetConstant(m_pEditEntity->light->pos.y);
		m_lightPos[2].SetConstant(m_pEditEntity->light->pos.z);
	}

	m_pivotX.SetConstant(m_pEditEntity->pivotAdjust.x);
	m_pivotY.SetConstant(m_pEditEntity->pivotAdjust.y);
	m_startFrame.SetConstant((float)m_pEditEntity->startFrame);
	m_spriteCut[0].SetConstant((float)m_pEditEntity->spriteCut.x);
	m_spriteCut[1].SetConstant((float)m_pEditEntity->spriteCut.y);
	m_emissiveColor[0].SetConstant(m_pEditEntity->emissiveColor.x);
	m_emissiveColor[1].SetConstant(m_pEditEntity->emissiveColor.y);
	m_emissiveColor[2].SetConstant(m_pEditEntity->emissiveColor.z);

	if (m_pEditEntity->collision)
	{
		m_collisionPos[0].SetConstant(m_pEditEntity->collision->pos.x);
		m_collisionPos[1].SetConstant(m_pEditEntity->collision->pos.y);
		m_collisionPos[2].SetConstant(m_pEditEntity->collision->pos.z);
		m_collisionSize[0].SetConstant(m_pEditEntity->collision->size.x);
		m_collisionSize[1].SetConstant(m_pEditEntity->collision->size.y);
		m_collisionSize[2].SetConstant(m_pEditEntity->collision->size.z);
	}

	m_shadowOpacity.SetConstant(m_pEditEntity->shadowOpacity);
	m_shadowLength.SetConstant(m_pEditEntity->shadowLengthScale);
	m_shadowScale.SetConstant(m_pEditEntity->shadowScale);
	m_soundVolume.SetConstant(m_pEditEntity->soundVolume);
	m_layerDepth.SetConstant(m_pEditEntity->layerDepth);
	m_specularPower.SetConstant(m_pEditEntity->specularPower);
	m_specularBrightness.SetConstant(m_pEditEntity->specularBrightness);
	m_animPreviewStride.SetConstant(200.0f);
	m_density.SetConstant(m_pEditEntity->density);
	m_restitution.SetConstant(m_pEditEntity->restitution);
	m_friction.SetConstant(m_pEditEntity->friction);

	ResetParticleMenu();
	ResetLightMenu();
	m_customDataEditor.Rebuild(m_renderEntity.get(), this);
	m_playStopButton.SetButton(L"play.png");
	m_animationTimer.Reset();
}

void EntityEditor::ResetParticleMenu()
{
	for (int t=0; t<ETH_MAX_PARTICLE_SYS_PER_ENTITY; t++)
	{
		if (m_pEditEntity->particleSystems[t])
		{
			m_particlePos[t][0].SetConstant(m_pEditEntity->particleSystems[t]->v3StartPoint.x);
			m_particlePos[t][1].SetConstant(m_pEditEntity->particleSystems[t]->v3StartPoint.y);
			m_particlePos[t][2].SetConstant(m_pEditEntity->particleSystems[t]->v3StartPoint.z);
		}
		else
		{
			m_particlePos[t][0].SetConstant(0.0f);
			m_particlePos[t][1].SetConstant(0.0f);
			m_particlePos[t][2].SetConstant(0.0f);
		}
	}
}

void EntityEditor::ResetLightMenu()
{
	if (m_pEditEntity->light)
	{
		m_lightPos[0].SetConstant(m_pEditEntity->light->pos.x);
		m_lightPos[1].SetConstant(m_pEditEntity->light->pos.y);
		m_lightPos[2].SetConstant(m_pEditEntity->light->pos.z);
	}
}

void EntityEditor::DoMainMenu()
{
	const string currentProjectPath = GetCurrentProjectPath(true);

	VideoPtr video = m_provider->GetVideo();
	Vector2 v2ScreenDim = video->GetScreenSizeF();

	GSGUI_BUTTON file_r = PlaceFileMenu();

	float y = m_menuSize*2, x = m_menuWidth*2;

	if (file_r.text == _S_NEW)
	{
		m_renderEntity->ForceSFXStop();
		UnloadAll();
		SetCurrentFile(_BASEEDITOR_DEFAULT_UNTITLED_FILE);
		InstantiateEntity();
		ResetEntityMenu();
	}
	if (file_r.text == _S_SAVE_AS)
	{
		SaveAs();
	}
	if (file_r.text == _S_SAVE)
	{
		if (GetCurrentFile(true) == _BASEEDITOR_DEFAULT_UNTITLED_FILE)
		{
			SaveAs();
		}
		else
		{
			Save(GetCurrentFile(true).c_str());
		}
	}
	if (file_r.text == _S_OPEN)
	{
		Open();
	}
	if (file_r.text == _S_GOTO_PROJ)
	{
		m_projManagerRequested = true;
	}

	file_r = m_addMenu.PlaceMenu(Vector2(x,y)); x+=m_menuWidth*2;
	if (file_r.text == _S_OPEN_ENTITY)
	{
		char filter[] = "Supported image files\0*.png;*.bmp;*.tga;*.jpg;*.jpeg;*.dds\0\0";
		char path[___OUTPUT_LENGTH], file[___OUTPUT_LENGTH];
		if (OpenForm(filter, utf8::c(ETHDirectories::GetEntityPath()).c_str(), path, file, currentProjectPath.c_str()))
		{
			ETHGlobal::CopyFileToProject(utf8::c(currentProjectPath).wstr(), utf8::c(path).wstr(), ETHDirectories::GetEntityPath(), m_provider->GetVideo()->GetFileManager());
			if (LoadSprite(file, path))
			{
				Vector2 screenMid = video->GetScreenSizeF()/2.0f;
				/*m_renderEntity.Create(video, m_audio, utf8::c(currentProjectPath).wstr(), *m_pEditEntity,
								m_provider->GetGraphicResourceManager(), m_audioResources, m_provider->GetShaderManager(), Vector3(screenMid.x, screenMid.y, 0),
								0.0f, m_pEditEntity->startFrame, m_renderEntity.GetColorARGB());*/
				m_renderEntity->SetSprite(m_pEditEntity->spriteFile);			
			}
		}
	}
	if (file_r.text == _S_OPEN_NORMALMAP)
	{
		char filter[] = "Supported image files\0*.png;*.bmp;*.tga;*.jpg;*.jpeg;*.dds\0\0";
		char path[___OUTPUT_LENGTH], file[___OUTPUT_LENGTH];
		if (OpenForm(filter, utf8::c(ETHDirectories::GetNormalMapPath()).c_str(), path, file, currentProjectPath.c_str()))
		{
			ETHGlobal::CopyFileToProject(utf8::c(currentProjectPath).wstr(), utf8::c(path).wstr(), ETHDirectories::GetNormalMapPath(), m_provider->GetVideo()->GetFileManager());
			if (LoadNormal(file, path))
			{
				Vector2 screenMid = video->GetScreenSizeF()/2.0f;
				/*m_renderEntity.Create(video, m_audio, utf8::c(currentProjectPath).wstr(), *m_pEditEntity, m_provider->GetGraphicResourceManager(),
								m_audioResources, m_provider->GetShaderManager(), Vector3(screenMid.x, screenMid.y, 0),
								0.0f, m_pEditEntity->startFrame, m_renderEntity.GetColorARGB());*/
				m_renderEntity->SetNormal(m_pEditEntity->normalFile);			
			}
		}
	}
	if (file_r.text == _S_OPEN_HALO && m_pEditEntity->light)
	{
		char filter[] = "Supported image files\0*.png;*.bmp;*.tga;*.jpg;*.jpeg;*.dds\0\0";
		char path[___OUTPUT_LENGTH], file[___OUTPUT_LENGTH];
		if (OpenForm(filter, utf8::c(ETHDirectories::GetEntityPath()).c_str(), path, file, currentProjectPath.c_str()))
		{
			ETHGlobal::CopyFileToProject(utf8::c(currentProjectPath).wstr(), utf8::c(path).wstr(), ETHDirectories::GetEntityPath(), m_provider->GetVideo()->GetFileManager());
			if (LoadHalo(file, path))
			{
				Vector2 screenMid = video->GetScreenSizeF()/2.0f;
				/*m_renderEntity.Create(video, m_audio, utf8::c(currentProjectPath).wstr(), *m_pEditEntity, m_provider->GetGraphicResourceManager(),
								m_audioResources, m_provider->GetShaderManager(), Vector3(screenMid.x, screenMid.y, 0), 0.0f,
								m_pEditEntity->startFrame, m_renderEntity.GetColorARGB());*/
				m_renderEntity->SetHalo(m_pEditEntity->light->haloBitmap);
			}
		}
	}
	if (file_r.text == _S_OPEN_GLOSSMAP)
	{
		char filter[] = "Supported image files\0*.png;*.bmp;*.tga;*.jpg;*.jpeg;*.dds\0\0";
		char path[___OUTPUT_LENGTH], file[___OUTPUT_LENGTH];
		if (OpenForm(filter, utf8::c(ETHDirectories::GetEntityPath()).c_str(), path, file, currentProjectPath.c_str()))
		{
			ETHGlobal::CopyFileToProject(utf8::c(currentProjectPath).wstr(), utf8::c(path).wstr(), ETHDirectories::GetEntityPath(), m_provider->GetVideo()->GetFileManager());
			if (LoadGloss(file, path))
			{
				Vector2 screenMid = video->GetScreenSizeF()/2.0f;
				/*m_renderEntity.Create(video, m_audio, utf8::c(currentProjectPath).wstr(), *m_pEditEntity, m_provider->GetGraphicResourceManager(),
								m_audioResources, m_provider->GetShaderManager(), Vector3(screenMid.x, screenMid.y, 0),
								0.0f, m_pEditEntity->startFrame, m_renderEntity.GetColorARGB());*/
				m_renderEntity->SetGloss(m_pEditEntity->glossFile);
			}
		}
	}
	if (file_r.text == _S_LOAD_PAR0)
	{
		char filter[] = "Ethanon engine PAR files\0*.par\0\0";
		char path[___OUTPUT_LENGTH], file[___OUTPUT_LENGTH];
		if (OpenForm(filter, "effects\\", path, file, currentProjectPath.c_str()))
		{
			if (LoadParticle(0, file, path))
			{
				Vector2 screenMid = video->GetScreenSizeF()/2.0f;
				/*m_renderEntity.Create(video, m_audio, utf8::c(currentProjectPath).wstr(), *m_pEditEntity, m_provider->GetGraphicResourceManager(),
								m_audioResources, m_provider->GetShaderManager(), Vector3(screenMid.x, screenMid.y, 0), 0.0f,
								m_pEditEntity->startFrame, m_renderEntity.GetColorARGB());*/
				m_renderEntity->LoadParticleSystem();
				ResetParticleMenu();
			}
		}
	}
	if (file_r.text == _S_LOAD_PAR1)
	{
		char filter[] = "Ethanon engine PAR files\0*.par\0\0";
		char path[___OUTPUT_LENGTH], file[___OUTPUT_LENGTH];
		if (OpenForm(filter, "effects\\", path, file, currentProjectPath.c_str()))
		{
			if (LoadParticle(1, file, path))
			{
				Vector2 screenMid = video->GetScreenSizeF()/2.0f;
				/*m_renderEntity.Create(video, m_audio, utf8::c(currentProjectPath).wstr(), *m_pEditEntity, m_provider->GetGraphicResourceManager(),
								m_audioResources, m_provider->GetShaderManager(), Vector3(screenMid.x, screenMid.y, 0), 0.0f,
								m_pEditEntity->startFrame, m_renderEntity.GetColorARGB());*/
				m_renderEntity->LoadParticleSystem();
				ResetParticleMenu();
			}
		}
	}
	file_r = m_delMenu.PlaceMenu(Vector2(x, y)); x += m_menuWidth * 2;
	if (file_r.text == _S_DEL_ENTITY)
	{
		UnloadSprite();
	}
	if (file_r.text == _S_DEL_NORMALMAP)
	{
		UnloadNormal();
	}
	if (file_r.text == _S_DEL_HALO)
	{
		UnloadHalo();
	}
	if (file_r.text == _S_DEL_GLOSSMAP)
	{
		UnloadGloss();
	}
	if (file_r.text == _S_DEL_PAR0)
	{
		UnloadParticle(0);
	}
	if (file_r.text == _S_DEL_PAR1)
	{
		UnloadParticle(1);
	}
}

void EntityEditor::ShowWarnings()
{
	// collect warnings
	list<wstring> warnings;
	/*if (m_pEditEntity->castShadow && m_pEditEntity->type != ETH_VERTICAL)
	{
		warnings.push_back("The entity must be 'vertical' to cast shadow");
	}*/
	if (m_pEditEntity->spriteFile != L"" && m_pEditEntity->normalFile == L"" && m_pEditEntity->applyLight &&
		m_pEditEntity->emissiveColor != Vector4(1,1,1,1) && m_pEditEntity->type == ETH_VERTICAL)
	{
		warnings.push_back(L"Your entity doesn't have a normal map. Consider switching 'apply light' off and use maximum emissive brightness");
	}
	if (m_pEditEntity->collision && m_pEditEntity->shape == ETHBS_COMPOUND)
	{
		if (m_pEditEntity->entityName.empty())
			warnings.push_back(L"Please save this entity, then setup compound shape data inside XML <Compound> tags in its file");
		else
			warnings.push_back(str_type::string(L"Please setup compound shape data inside XML <Compound> tags in file ") + m_pEditEntity->entityName);
	}

	// draw warnings
	if (!warnings.empty())
	{
		list<wstring>::iterator iter;
		wstringstream ss;
		ss << L"Warnings" << endl;
		for (iter = warnings.begin(); iter != warnings.end(); iter++)
		{
			ss << L"-" << *iter << endl;
		}
		VideoPtr video = m_provider->GetVideo();
		Vector2 v2ScreenDim = video->GetScreenSizeF();
		ShadowPrint(Vector2(m_menuWidth*2, v2ScreenDim.y-(m_menuSize*(warnings.size()+2))), ss.str().c_str());
	}
}

void EntityEditor::DrawEntityElementName(const Vector2 &v2Pos, SpritePtr pSprite, const string &name)
{
	if (pSprite)
	{
		pSprite->SetOrigin(GSEO_DEFAULT);
		pSprite->DrawShaped(v2Pos, Vector2(m_menuSize, m_menuSize), GS_WHITE, GS_WHITE, GS_WHITE, GS_WHITE);
	}
	m_provider->GetVideo()->DrawBitmapText(
		v2Pos+Vector2(m_menuSize, 0), 
		utf8::c(name).wc_str(), L"Verdana14_shadow.fnt", GS_WHITE
	);
}

void EntityEditor::ShowEntityResources(Vector2 v2Pos)
{
	m_provider->GetVideo()->DrawBitmapText(
		v2Pos, 
		L"Resources:", L"Verdana14_shadow.fnt", GS_WHITE
	);
	v2Pos.x += m_menuSize/2;
	v2Pos.y += m_menuSize;

	if (m_renderEntity->GetSprite())
	{
		DrawEntityElementName(v2Pos, m_renderEntity->GetSprite(), utf8::c(m_pEditEntity->spriteFile + L" (sprite)").str());
		v2Pos.y += m_menuSize;
	}
	if (m_renderEntity->GetNormal())
	{
		DrawEntityElementName(v2Pos, m_renderEntity->GetNormal(), utf8::c(m_pEditEntity->normalFile + L" (normal)").str());
		v2Pos.y += m_menuSize;
	}
	if (m_renderEntity->GetGloss())
	{
		DrawEntityElementName(v2Pos, m_renderEntity->GetGloss(), utf8::c(m_pEditEntity->glossFile + L" (gloss)").str());
		v2Pos.y += m_menuSize;
	}
	if (m_renderEntity->GetHalo() && m_pEditEntity->light)
	{
		DrawEntityElementName(v2Pos, m_renderEntity->GetHalo(), utf8::c(m_pEditEntity->light->haloBitmap + L" (halo)").str());
		v2Pos.y += m_menuSize;
	}
	for (unsigned int t=0; t<ETH_MAX_PARTICLE_SYS_PER_ENTITY; t++)
	{
		if (!m_renderEntity->HasParticleSystem(t))
			continue;

		m_provider->GetVideo()->DrawBitmapText(
			v2Pos, 
			L"Particles:", L"Verdana14_shadow.fnt", GS_WHITE
		);
		v2Pos.y += m_menuSize;

		DrawEntityElementName(v2Pos+Vector2(m_menuSize/2,0), m_renderEntity->GetParticleBMP(t), utf8::c(m_pEditEntity->particleSystems[t]->bitmapFile).c_str());
		v2Pos.y += m_menuSize;
		DrawEntityElementName(v2Pos+Vector2(m_menuSize/2,0), SpritePtr(), utf8::c(m_pEditEntity->particleSystems[t]->soundFXFile).c_str());
		v2Pos.y += m_menuSize;
	}
}

string EntityEditor::DoEditor(SpritePtr pNextAppButton)
{
	VideoPtr video = m_provider->GetVideo();
	video->SetCameraPos(Vector2(0,0));
	DrawEntity();

	const string programPath = GetCurrentProjectPath(false);

	Vector2 v2ScreenDim = video->GetScreenSizeF();
	const float x = video->GetScreenSizeF().x-m_menuWidth*2;
	float y = m_menuSize*m_menuScale+(m_menuSize*2);
	y+=m_menuSize/2;

	if (!IsFileMenuActive() && !m_addMenu.IsActive() && !m_delMenu.IsActive())
	{
		ShadowPrint(Vector2(0,y), L"Entity type:"); y+=m_menuSize;
		m_type.PlaceMenu(Vector2(0,y)); y += m_menuSize*m_type.GetNumButtons();
		if (m_type.GetButtonStatus(_S_HORIZONTAL))
			m_pEditEntity->type = ETH_HORIZONTAL;
		if (m_type.GetButtonStatus(_S_VERTICAL))
			m_pEditEntity->type = ETH_VERTICAL;
		if (m_type.GetButtonStatus(_S_DECAL))
			m_pEditEntity->type = ETH_GROUND_DECAL;
		if (m_type.GetButtonStatus(_S_OVERALL))
			m_pEditEntity->type = ETH_OVERALL;
		if (m_type.GetButtonStatus(_S_OPAQUE_DECAL))
			m_pEditEntity->type = ETH_OPAQUE_DECAL;
		if (m_type.GetButtonStatus(_S_LAYERABLE))
			m_pEditEntity->type = ETH_LAYERABLE;
		y+=m_menuSize/2;

		m_pEditEntity->pivotAdjust.x = m_pivotX.PlaceInput(Vector2(0.0f, y), Vector2(0.0f, v2ScreenDim.y-m_menuSize), m_menuWidth);	y+=m_menuSize;
		m_pEditEntity->pivotAdjust.y = m_pivotY.PlaceInput(Vector2(0.0f, y), Vector2(0.0f, v2ScreenDim.y-m_menuSize), m_menuWidth);	y+=m_menuSize;
		y+=m_menuSize/2;

		m_pEditEntity->spriteCut.x = (int)m_spriteCut[0].PlaceInput(Vector2(0.0f, y), Vector2(0.0f, v2ScreenDim.y-m_menuSize), m_menuWidth);	y+=m_menuSize;
		m_pEditEntity->spriteCut.y = (int)m_spriteCut[1].PlaceInput(Vector2(0.0f, y), Vector2(0.0f, v2ScreenDim.y-m_menuSize), m_menuWidth);	y+=m_menuSize;
		m_pEditEntity->startFrame = (int)m_startFrame.PlaceInput(Vector2(0.0f, y), Vector2(0.0f, v2ScreenDim.y-m_menuSize), m_menuWidth);
		if (m_pEditEntity->spriteCut.x > 1 || m_pEditEntity->spriteCut.y > 1)
		{
			if (m_playStopButton.GetCurrentButton()->fileName == L"stop.png")
			{
				y+=m_menuSize;
			}

			GSGUI_BUTTON playStop = m_playStopButton.PlaceMenu(Vector2(m_menuWidth*2, y));
			if (playStop.text == L"stop.png")
			{
				const float stride = m_animPreviewStride.PlaceInput(Vector2(0.0f, y), Vector2(0.0f, v2ScreenDim.y-m_menuSize), m_menuWidth);
				m_animationTimer.Update(0, static_cast<unsigned int>(m_pEditEntity->spriteCut.x*m_pEditEntity->spriteCut.y)-1, static_cast<unsigned int>(stride));
				m_renderEntity->SetFrame(m_animationTimer.Get());
			}
			else
			{
				m_animationTimer.Update(m_pEditEntity->startFrame, m_pEditEntity->startFrame, 5000);
			}
		}
		else
		{
			m_playStopButton.SetButton(L"play.png");
			m_animationTimer.Reset();
		}
		y+=m_menuSize/2;
		y+=m_menuSize;

		m_pEditEntity->emissiveColor.x = m_emissiveColor[0].PlaceInput(Vector2(0.0f, y), Vector2(0.0f, v2ScreenDim.y-m_menuSize), m_menuWidth);	y+=m_menuSize;
		m_pEditEntity->emissiveColor.y = m_emissiveColor[1].PlaceInput(Vector2(0.0f, y), Vector2(0.0f, v2ScreenDim.y-m_menuSize), m_menuWidth);	y+=m_menuSize;
		m_pEditEntity->emissiveColor.z = m_emissiveColor[2].PlaceInput(Vector2(0.0f, y), Vector2(0.0f, v2ScreenDim.y-m_menuSize), m_menuWidth);	y+=m_menuSize;
		y+=m_menuSize/2;

		ShadowPrint(Vector2(0,y), L"Boolean properties:"); y+=m_menuSize;
		GSGUI_BUTTON file_r = m_bool.PlaceMenu(Vector2(0,y)); y += m_menuSize*m_bool.GetNumButtons();
		m_pEditEntity->castShadow = m_bool.GetButtonStatus(_S_CAST_SHADOW);
		m_pEditEntity->staticEntity = m_bool.GetButtonStatus(_S_STATIC_ENTITY);

		if (!m_pEditEntity->collision && m_bool.GetButtonStatus(_S_COLLIDABLE))
		{
			m_pEditEntity->collision = boost::shared_ptr<ETHCollisionBox>(new ETHCollisionBox);
		}
		else if (m_pEditEntity->collision && !m_bool.GetButtonStatus(_S_COLLIDABLE))
		{
			m_pEditEntity->collision.reset();
		}

		m_pEditEntity->applyLight = m_bool.GetButtonStatus(_S_APPLY_LIGHT);
		
		if (!m_pEditEntity->light && m_bool.GetButtonStatus(_S_ACTIVE_LIGHT))
		{
			m_pEditEntity->light = boost::shared_ptr<ETHLight>(new ETHLight(true));
			ResetEntityMenu();
		}
		else if (m_pEditEntity->light && !m_bool.GetButtonStatus(_S_ACTIVE_LIGHT))
		{
			m_pEditEntity->light.reset();
			ResetEntityMenu();
		}

		if (file_r.text == _S_ACTIVE_LIGHT && m_bool.GetButtonStatus(_S_ACTIVE_LIGHT))
		{
			m_tool.ResetButtons();
			m_tool.ActivateButton(_S_EDIT_LIGHT);
		}
		if (file_r.text == _S_COLLIDABLE && m_bool.GetButtonStatus(_S_COLLIDABLE))
		{
			CreateCollisionBoxFromEntity();
			m_bool.ActivateButton(_S_COLLIDABLE);
			m_tool.ResetButtons();
			m_tool.ActivateButton(_S_EDIT_COLLISION);
		}
		y+=m_menuSize/2;

		if (m_pEditEntity->castShadow)
		{
			ShadowPrint(Vector2(0,y), L"Shadow properties:"); y+=m_menuSize;
			m_pEditEntity->shadowScale   = m_shadowScale.PlaceInput(Vector2(0.0f, y), Vector2(0.0f, v2ScreenDim.y-m_menuSize), m_menuWidth);	y+=m_menuSize;
			m_pEditEntity->shadowOpacity = m_shadowOpacity.PlaceInput(Vector2(0.0f, y), Vector2(0.0f, v2ScreenDim.y-m_menuSize), m_menuWidth);	y+=m_menuSize;
			m_pEditEntity->shadowLengthScale = m_shadowLength.PlaceInput(Vector2(0.0f, y), Vector2(0.0f, v2ScreenDim.y-m_menuSize), m_menuWidth);	y+=m_menuSize;
			y+=m_menuSize/2;
		}

		if (m_pEditEntity->light)
		{
			if (m_pEditEntity->staticEntity)
			{
				m_pEditEntity->light->staticLight = true;
				m_boolLight.ActivateButton(_S_STATIC_LIGHT);
			}
			else
			{
				m_pEditEntity->light->staticLight = false;
				m_boolLight.DeactivateButton(_S_STATIC_LIGHT);
			}

			ShadowPrint(Vector2(0,y), L"Light properties:"); y+=m_menuSize;
			m_boolLight.PlaceMenu(Vector2(0,y)); y += m_menuSize*m_boolLight.GetNumButtons();
			m_pEditEntity->light->staticLight = m_boolLight.GetButtonStatus(_S_STATIC_LIGHT);
			m_pEditEntity->light->castShadows = m_boolLight.GetButtonStatus(_S_SHADOW_CASTER);
			y+=m_menuSize/2;

			m_pEditEntity->light->pos.x = m_lightPos[0].PlaceInput(Vector2(0.0f, y), Vector2(0.0f, v2ScreenDim.y-m_menuSize), m_menuWidth);	y+=m_menuSize;
			m_pEditEntity->light->pos.y = m_lightPos[1].PlaceInput(Vector2(0.0f, y), Vector2(0.0f, v2ScreenDim.y-m_menuSize), m_menuWidth);	y+=m_menuSize;
			m_pEditEntity->light->pos.z = m_lightPos[2].PlaceInput(Vector2(0.0f, y), Vector2(0.0f, v2ScreenDim.y-m_menuSize), m_menuWidth);	y+=m_menuSize;
			m_pEditEntity->light->range = m_lightRange.PlaceInput(Vector2(0.0f, y), Vector2(0.0f, v2ScreenDim.y-m_menuSize), m_menuWidth);	y+=m_menuSize;
			y+=m_menuSize/2;
			m_pEditEntity->light->color.x = m_lightColor[0].PlaceInput(Vector2(0.0f, y), Vector2(0.0f, v2ScreenDim.y-m_menuSize), m_menuWidth);	y+=m_menuSize;
			m_pEditEntity->light->color.y = m_lightColor[1].PlaceInput(Vector2(0.0f, y), Vector2(0.0f, v2ScreenDim.y-m_menuSize), m_menuWidth);	y+=m_menuSize;
			m_pEditEntity->light->color.z = m_lightColor[2].PlaceInput(Vector2(0.0f, y), Vector2(0.0f, v2ScreenDim.y-m_menuSize), m_menuWidth);	y+=m_menuSize;
			y+=m_menuSize/2;

			m_pEditEntity->specularBrightness = m_specularBrightness.PlaceInput(Vector2(0.0f, y), Vector2(0.0f, v2ScreenDim.y-m_menuSize), m_menuWidth);	y+=m_menuSize;
			y+=m_menuSize/2;

			if (m_pEditEntity->light->haloBitmap != L"")
			{
				m_pEditEntity->light->haloBrightness = m_haloAlpha.PlaceInput(Vector2(0.0f, y), Vector2(0.0f, v2ScreenDim.y-m_menuSize), m_menuWidth);	y+=m_menuSize;
				m_pEditEntity->light->haloSize       = m_haloSize.PlaceInput(Vector2(0.0f, y), Vector2(0.0f, v2ScreenDim.y-m_menuSize), m_menuWidth);	y+=m_menuSize;
				y+=m_menuSize/2;
			}

			for (int t=0; t<ETH_MAX_PARTICLE_SYS_PER_ENTITY; t++)
			{
				if (m_pEditEntity->particleSystems[t]->nParticles>0)
				{
					ShadowPrint(Vector2(0,y), L"Attach light to particles:"); y+=m_menuSize;
					break;
				}
			}
			m_attachLight.PlaceMenu(Vector2(0,y));
			for (int t=0; t<ETH_MAX_PARTICLE_SYS_PER_ENTITY; t++)
			{
				if (
					m_attachLight.GetButtonStatus(
						utf8::c(m_pEditEntity->particleSystems[t]->bitmapFile).wc_str()
					)
				)
				if (m_pEditEntity->light)
				{
					m_pEditEntity->light->pos.x = m_pEditEntity->particleSystems[t]->v3StartPoint.x;
					m_pEditEntity->light->pos.y = m_pEditEntity->particleSystems[t]->v3StartPoint.y;
					m_pEditEntity->light->pos.z = m_pEditEntity->particleSystems[t]->v3StartPoint.z;
					ResetLightMenu();
				}
			}
		}

		y=m_menuSize+m_menuSize*2;
		y+=m_menuSize/2;
		m_tool.PlaceMenu(Vector2(x,y)); y += m_menuSize*m_tool.GetNumButtons();
		y+=m_menuSize/2;

		ShadowPrint(Vector2(x,y), L"Lighting mode:"); y+=m_menuSize;
		m_renderMode.PlaceMenu(Vector2(x,y)); y += m_menuSize*m_renderMode.GetNumButtons();
		m_provider->GetShaderManager()->UsePS(m_renderMode.GetButtonStatus(_S_USE_PS));
		y+=m_menuSize/2;

		ShadowPrint(Vector2(x,y), L"Blend mode:"); y+=m_menuSize;
		m_blendMode.PlaceMenu(Vector2(x,y)); y += m_menuSize*m_blendMode.GetNumButtons();
		if (m_blendMode.GetButtonStatus(_S_BLEND_MODE_DEFAULT))
			m_pEditEntity->blendMode = GSAM_PIXEL;
		else if (m_blendMode.GetButtonStatus(_S_BLEND_MODE_ADD))
			m_pEditEntity->blendMode = GSAM_ADD;
		else if (m_blendMode.GetButtonStatus(_S_BLEND_MODE_ALPHATEST))
			m_pEditEntity->blendMode = GSAM_ALPHA_TEST;
		else if (m_blendMode.GetButtonStatus(_S_BLEND_MODE_MODULATE))
			m_pEditEntity->blendMode = GSAM_MODULATE;
		y+=m_menuSize/2;

		if (m_tool.GetButtonStatus(_S_EDIT_LIGHT) && m_pEditEntity->light)
		{
			Vector2 v2LightPos(m_pEditEntity->light->pos.x, m_pEditEntity->light->pos.y);
			if (m_spot.MouseOverLClick(m_provider->GetInput(), video, v2LightPos + m_renderEntity->GetPositionXY()) == GSKS_DOWN)
			{
				const Vector2 v2Cursor = m_provider->GetInput()->GetCursorPositionF(video);
				m_pEditEntity->light->pos.x = v2Cursor.x - m_renderEntity->GetPosition().x;
				m_pEditEntity->light->pos.y = v2Cursor.y - m_renderEntity->GetPosition().y;
				m_lightPos[0].SetConstant(m_pEditEntity->light->pos.x);
				m_lightPos[1].SetConstant(m_pEditEntity->light->pos.y);
				ResetLightMenu();
			}
		}
		if (m_tool.GetButtonStatus(_S_EDIT_PARTICLES))
		{
			for (int t=0; t<ETH_MAX_PARTICLE_SYS_PER_ENTITY; t++)
			{
				if (m_pEditEntity->particleSystems[t]->nParticles > 0)
				{
					ShadowPrint(Vector2(x,y), L"Particle start position:"); y+=m_menuSize;
					break;
				}
			}

			for (int t=0; t<ETH_MAX_PARTICLE_SYS_PER_ENTITY; t++)
			{
				if (m_pEditEntity->particleSystems[t]->nParticles > 0)
				{
					m_pEditEntity->particleSystems[t]->v3StartPoint.x = m_particlePos[t][0].PlaceInput(Vector2(x, y), Vector2(0.0f, v2ScreenDim.y-m_menuSize), m_menuWidth);	y+=m_menuSize;
					m_pEditEntity->particleSystems[t]->v3StartPoint.y = m_particlePos[t][1].PlaceInput(Vector2(x, y), Vector2(0.0f, v2ScreenDim.y-m_menuSize), m_menuWidth);	y+=m_menuSize;
					m_pEditEntity->particleSystems[t]->v3StartPoint.z = m_particlePos[t][2].PlaceInput(Vector2(x, y), Vector2(0.0f, v2ScreenDim.y-m_menuSize), m_menuWidth);	y+=m_menuSize;

					y+=m_menuSize/2;
					Vector2 v2ParticlePos = ETHGlobal::ToVector2(m_pEditEntity->particleSystems[t]->v3StartPoint);
					if (m_particleSpot[t].MouseOverLClick(m_provider->GetInput(), video, v2ParticlePos + m_renderEntity->GetPositionXY()) == GSKS_DOWN)
					{
						const Vector2 v2Cursor = m_provider->GetInput()->GetCursorPositionF(video);
						m_pEditEntity->particleSystems[t]->v3StartPoint.x = v2Cursor.x - m_renderEntity->GetPosition().x;
						m_pEditEntity->particleSystems[t]->v3StartPoint.y = v2Cursor.y - m_renderEntity->GetPosition().y;
						//m_lightPos[0].SetConstant(m_pEditEntity->particle[t].v2StartPoint.x);
						//m_lightPos[1].SetConstant(m_pEditEntity->particle[t].v2StartPoint.y);
						//break;
						ResetParticleMenu();
					}
					m_renderEntity->SetParticlePosition(t, m_pEditEntity->particleSystems[t]->v3StartPoint);
				}
			}
		}

		if (m_pEditEntity->collision)
		{
			ShadowPrint(Vector2(x,y), _S_BODY_SHAPE); y+=m_menuSize;
			m_bodyShape.PlaceMenu(Vector2(x, y)); y += m_menuSize * m_bodyShape.GetNumButtons();
			if (m_bodyShape.GetButtonStatus(_S_BODY_SHAPE_BOX))
				m_pEditEntity->shape = ETHBS_BOX;
			if (m_bodyShape.GetButtonStatus(_S_BODY_SHAPE_CIRCLE))
				m_pEditEntity->shape = ETHBS_CIRCLE;
			if (m_bodyShape.GetButtonStatus(_S_BODY_SHAPE_POLYGON))
				m_pEditEntity->shape = ETHBS_POLYGON;
			if (m_bodyShape.GetButtonStatus(_S_BODY_SHAPE_COMPOUND))
				m_pEditEntity->shape = ETHBS_COMPOUND;

			y+=m_menuSize/2;
			if (!m_bodyShape.GetButtonStatus(_S_BODY_SHAPE_NONE))
			{
				m_pEditEntity->density     = m_density.PlaceInput(Vector2(x, y), Vector2(0.0f, v2ScreenDim.y - m_menuSize), m_menuWidth);	y+=m_menuSize;
				m_pEditEntity->friction    = m_friction.PlaceInput(Vector2(x, y), Vector2(0.0f, v2ScreenDim.y - m_menuSize), m_menuWidth);	y+=m_menuSize;
				m_pEditEntity->restitution = m_restitution.PlaceInput(Vector2(x, y), Vector2(0.0f, v2ScreenDim.y - m_menuSize), m_menuWidth);	y+=m_menuSize;
			}
			y+=m_menuSize/2;
			y+=m_menuSize/2;

			ShadowPrint(Vector2(x,y), L"Collision box position:"); y+=m_menuSize;
			m_pEditEntity->collision->pos.x = m_collisionPos[0].PlaceInput(Vector2(x, y), Vector2(0.0f, v2ScreenDim.y - m_menuSize), m_menuWidth);	y+=m_menuSize;
			m_pEditEntity->collision->pos.y = m_collisionPos[1].PlaceInput(Vector2(x, y), Vector2(0.0f, v2ScreenDim.y - m_menuSize), m_menuWidth);	y+=m_menuSize;
			m_pEditEntity->collision->pos.z = m_collisionPos[2].PlaceInput(Vector2(x, y), Vector2(0.0f, v2ScreenDim.y - m_menuSize), m_menuWidth);	y+=m_menuSize;
			y+=m_menuSize/2;

			ShadowPrint(Vector2(x,y), L"Collision box size:"); y+=m_menuSize;
			m_pEditEntity->collision->size.x = m_collisionSize[0].PlaceInput(Vector2(x, y), Vector2(0.0f, v2ScreenDim.y - m_menuSize), m_menuWidth);	y+=m_menuSize;
			m_pEditEntity->collision->size.y = m_collisionSize[1].PlaceInput(Vector2(x, y), Vector2(0.0f, v2ScreenDim.y - m_menuSize), m_menuWidth);	y+=m_menuSize;
			m_pEditEntity->collision->size.z = m_collisionSize[2].PlaceInput(Vector2(x, y), Vector2(0.0f, v2ScreenDim.y - m_menuSize), m_menuWidth);	y+=m_menuSize;
			y+=m_menuSize/2;
		}

		if (m_renderEntity->HasSoundEffect())
		{
			m_pEditEntity->soundVolume = m_soundVolume.PlaceInput(Vector2(x, y), Vector2(0.0f, v2ScreenDim.y-m_menuSize), m_menuWidth);	y+=m_menuSize;
			m_renderEntity->SetSoundVolume(m_pEditEntity->soundVolume);
			y+=m_menuSize/2;
		}

		if (m_type.GetButtonStatus(_S_LAYERABLE))
		{
			m_pEditEntity->layerDepth = m_layerDepth.PlaceInput(Vector2(x, y), Vector2(0.0f, v2ScreenDim.y-m_menuSize), m_menuWidth);	y+=m_menuSize;
			y+=m_menuSize/2;
		}

		if (m_renderEntity->GetGloss())
		{
			m_pEditEntity->specularPower = m_specularPower.PlaceInput(Vector2(x, y), Vector2(0.0f, v2ScreenDim.y-m_menuSize), m_menuWidth);	y+=m_menuSize;
			y+=m_menuSize/2;
		}
	}

	// execute plug-ins
	y += m_customDataEditor.DoEditor(Vector2(x,y), m_renderEntity.get(), this);
	y += m_particleScale.DoEditor(Vector2(x,y), m_renderEntity.get(), this);
	y+=m_menuSize/2;

	// print resources
	ShowEntityResources(Vector2(x,y));

	if (SpriteFrameChanged() && m_renderEntity->GetSprite())
	{
		ResetSpriteCut();
	}
	m_renderEntity->SetFrame(m_animationTimer.Get());

	DoMainMenu();
	SetFileNameToTitle(video, _ETH_WINDOW_TITLE);
	ShowWarnings();
	return "entity";
}

bool EntityEditor::SaveAs()
{
	char filter[] = "Ethanon Entity files (*.ent)\0*.ent\0\0";
	char path[___OUTPUT_LENGTH], file[___OUTPUT_LENGTH];
	if (SaveForm(filter, utf8::c(ETHDirectories::GetEntityPath()).c_str(), path, file, GetCurrentProjectPath(true).c_str()))
	{
		Save(path);
	}
	return true;
}

bool EntityEditor::Save(const char *path)
{
	string sOut;
	AddExtension(path, ".ent", sOut);
	m_pEditEntity->SaveToFile(utf8::c(sOut).wstr());
	SetCurrentFile(sOut.c_str());
	return true;
}

bool EntityEditor::Open()
{
	string currentProjectPath = GetCurrentProjectPath(true);

	char filter[] = "Ethanon Entity files (*.ent)\0*.ent\0\0";
	char path[___OUTPUT_LENGTH], file[___OUTPUT_LENGTH];
	if (OpenForm(filter, utf8::c(ETHDirectories::GetEntityPath()).c_str(), path, file, currentProjectPath.c_str()))
	{
		m_renderEntity->ForceSFXStop();
		m_renderEntity->ClearCustomData();

		InstantiateEntity(utf8::c(path).wstr());

		m_attachLight.Clear();
		ResetEntityMenu();
		ResetSpriteCut();
		SetCurrentFile(path);
		for (int t = 0; t < ETH_MAX_PARTICLE_SYS_PER_ENTITY; t++)
		{
			if (m_pEditEntity->particleSystems[t]->nParticles > 0 && m_pEditEntity->particleSystems[t]->bitmapFile != L"")
			{
				m_attachLight.AddButton(utf8::c(m_pEditEntity->particleSystems[t]->bitmapFile).wc_str());
			}
		}
		m_customDataEditor.Rebuild(m_renderEntity.get(), this);
	}
	return true;
}

void EntityEditor::DrawEntity()
{
	VideoPtr video = m_provider->GetVideo();
	Vector2 screenMid = video->GetScreenSizeF()/2.0f;
	m_axis->Draw(screenMid);
	m_renderEntity->SetOrphanPositionXY(screenMid);

	const Vector2 v2Cursor = m_provider->GetInput()->GetCursorPositionF(video);
	m_cursorLight.pos.x = v2Cursor.x;
	m_cursorLight.pos.y = v2Cursor.y;

	// move the cursor light up and down (along the Z axis)
	if (m_provider->GetInput()->GetKeyState(GSK_PAGEUP) == GSKS_HIT)
	{
		m_cursorLight.pos.z+=5;
	}
	if (m_provider->GetInput()->GetKeyState(GSK_PAGEDOWN) == GSKS_HIT)
	{
		m_cursorLight.pos.z-=5;
	}

	if (m_pEditEntity->light)
	{
		const Vector2 v2LightPos(m_pEditEntity->light->pos.x, m_pEditEntity->light->pos.y);
		const float diameter = m_pEditEntity->light->range * 2;
		const GS_COLOR color = ConvertToDW(m_pEditEntity->light->color);
		m_range->DrawShaped(m_renderEntity->GetPositionXY() + v2LightPos, Vector2(diameter,diameter),
					  color,color,color,color);
	}

	video->SetZWrite(true);
	video->SetZBuffer(true);

	// Render the shadow from the entity light
	if (m_pEditEntity->light)
	{
		ETHLight editLight = *m_pEditEntity->light.get();
		editLight.pos += m_renderEntity->GetPosition();
		if (m_provider->GetShaderManager()->BeginShadowPass(m_renderEntity.get(), &editLight, video->GetScreenSizeF().y,-video->GetScreenSizeF().y))
		{
			m_renderEntity->DrawShadow(video->GetScreenSizeF().y,-video->GetScreenSizeF().y,
									  m_sceneProps, *m_pEditEntity->light.get(), m_renderEntity.get());
			m_provider->GetShaderManager()->EndShadowPass();
		}
	}

	// Render the shadow from the cursor light
	if (!m_pEditEntity->light)
	{
		if (m_provider->GetShaderManager()->BeginShadowPass(m_renderEntity.get(), &m_cursorLight, video->GetScreenSizeF().y,-video->GetScreenSizeF().y))
		{
			m_renderEntity->DrawShadow(video->GetScreenSizeF().y,-video->GetScreenSizeF().y, m_sceneProps, m_cursorLight, 0);
			m_provider->GetShaderManager()->EndShadowPass();
		}
	}
	video->SetZWrite(false);
	video->SetZBuffer(false);

	if (m_pEditEntity->collision && m_tool.GetButtonStatus(_S_EDIT_COLLISION))
	{
		if (m_pEditEntity->collision->size.x != 0.0f &&
			m_pEditEntity->collision->size.y != 0.0f &&
			m_pEditEntity->collision->size.z != 0.0f)
		{
			m_renderEntity->DrawCollisionBox(true, m_outline, GS_WHITE,	ETH_DEFAULT_ZDIRECTION);
		}
	}

	m_sceneProps.ambient =
		(m_pEditEntity->applyLight) ? Vector3(_ETH_DEFAULT_AMBIENT_LIGHT,_ETH_DEFAULT_AMBIENT_LIGHT,_ETH_DEFAULT_AMBIENT_LIGHT) : constant::ONE_VECTOR3;

	video->SetZWrite(true);
	video->SetZBuffer(true);

	m_provider->GetShaderManager()->BeginAmbientPass(m_renderEntity.get(), video->GetScreenSizeF().y,-video->GetScreenSizeF().y);
	m_renderEntity->DrawAmbientPass(video->GetScreenSizeF().y,-video->GetScreenSizeF().y, false, m_sceneProps);
	m_provider->GetShaderManager()->EndAmbientPass();

	video->SetZWrite(false);
	video->SetZBuffer(false);

	video->SetZWrite(false);
	video->SetZBuffer(true);

	if (!m_renderEntity->HasLightSource())
	{
		if (m_provider->GetShaderManager()->BeginLightPass(m_renderEntity.get(), &m_cursorLight, video->GetScreenSizeF().y,-video->GetScreenSizeF().y, 2.0f))
		{
			m_renderEntity->DrawLightPass(ETH_DEFAULT_ZDIRECTION);
			m_provider->GetShaderManager()->EndLightPass();
		}
	}

	if (m_pEditEntity->light)
	{
		if (m_provider->GetShaderManager()->BeginLightPass(m_renderEntity.get(), m_pEditEntity->light.get(), video->GetScreenSizeF().y,-video->GetScreenSizeF().y, 2.0f, m_renderEntity.get()))
		{
			m_renderEntity->DrawLightPass(ETH_DEFAULT_ZDIRECTION);
			m_provider->GetShaderManager()->EndLightPass();
		}
	}

	video->SetZWrite(false);
	video->SetZBuffer(false);

	if (m_pEditEntity->collision && m_tool.GetButtonStatus(_S_EDIT_COLLISION))
	{
		if (m_pEditEntity->collision->size.x != 0.0f &&
			m_pEditEntity->collision->size.y != 0.0f &&
			m_pEditEntity->collision->size.z != 0.0f)
		{
			m_renderEntity->DrawCollisionBox(false, m_outline, GS_WHITE, ETH_DEFAULT_ZDIRECTION);
		}
	}

	if ((m_lightRange.IsActive() || m_lightRange.IsMouseOver())
		&& m_renderEntity->GetType() != ETH_VERTICAL && m_pEditEntity->light)
	{
		const Vector2 v2LightPos(m_pEditEntity->light->pos.x, m_pEditEntity->light->pos.y);
		const float diameter = m_pEditEntity->light->range * 2;
		const GS_COLOR color = ConvertToDW(m_pEditEntity->light->color);
		m_range->DrawShaped(m_renderEntity->GetPositionXY() + v2LightPos, Vector2(diameter,diameter),
							color, color, color, color);
	}

	m_speedTimer.CalcLastFrame();
	m_renderEntity->UpdateParticleSystems(ETH_DEFAULT_ZDIRECTION, static_cast<unsigned long>(m_speedTimer.GetElapsedTime() * 1000.0));

	video->SetZWrite(true);
	video->SetZBuffer(true);
	list<ETHLight> lights;
	if (m_pEditEntity->light)
	{
		lights.push_back(*m_pEditEntity->light.get());
	}
	else
	{
		lights.push_back(m_cursorLight);
	}
	if (m_provider->GetShaderManager()->BeginParticlePass())
	{
		for (int t=0; t<ETH_MAX_PARTICLE_SYS_PER_ENTITY; t++)
		{
			if (m_renderEntity->HasParticleSystem(t))
				m_renderEntity->DrawParticles(t, video->GetScreenSizeF().y,-video->GetScreenSizeF().y, m_sceneProps);
		}
		m_provider->GetShaderManager()->EndParticlePass();

		if (m_renderEntity->AreParticlesOver())
		{
			for (int t=0; t<ETH_MAX_PARTICLE_SYS_PER_ENTITY; t++)
			{
				if (m_renderEntity->HasParticleSystem(t))
					m_renderEntity->PlayParticleSystem(t, ETH_DEFAULT_ZDIRECTION);
			}
		}
	}
	video->SetZWrite(false);
	video->SetZBuffer(false);

	if (m_pEditEntity->light)
	{
		video->SetZWrite(true);
		video->SetZBuffer(true);
		if (m_provider->GetShaderManager()->BeginHaloPass(m_pEditEntity->light.get(), video->GetScreenSizeF().y, m_renderEntity.get()))
		{
			m_renderEntity->DrawHalo(video->GetScreenSizeF().y,-video->GetScreenSizeF().y, false, ETH_DEFAULT_ZDIRECTION);
			m_provider->GetShaderManager()->EndHaloPass();
		}
		video->SetZWrite(false);
		video->SetZBuffer(false);

		bool show = false;
		if (m_lightRange.IsActive() || m_lightRange.IsMouseOver() || m_tool.GetButtonStatus(_S_EDIT_LIGHT))
		{
			show = true;
		}
		else
		{
			for (int t=0; t<3; t++)
			{
				if (m_lightPos[t].IsMouseOver() || m_lightPos[t].IsActive() ||
					m_lightColor[t].IsMouseOver() || m_lightColor[t].IsActive())
					show = true;
			}
		}
		if (show)
		{
			const Vector2 v2LightPos(m_pEditEntity->light->pos.x, m_pEditEntity->light->pos.y);
			m_spot.m_sprite->Draw(m_renderEntity->GetPositionXY() + v2LightPos, ConvertToDW(m_pEditEntity->light->color));
		}
	}
	if (m_tool.GetButtonStatus(_S_EDIT_PARTICLES))
	{
		for (int t=0; t<ETH_MAX_PARTICLE_SYS_PER_ENTITY; t++)
		{
			if (m_pEditEntity->particleSystems[t]->nParticles > 0)
			{
				m_particleSpot[t].m_sprite->Draw(m_renderEntity->GetPositionXY() + ETHGlobal::ToVector2(m_pEditEntity->particleSystems[t]->v3StartPoint),
												 ConvertToDW(m_pEditEntity->particleSystems[t]->v4Color0));
			}
		}
	}
}

bool EntityEditor::LoadHalo(const char *file, const char *path)
{
	if (m_provider->GetGraphicResourceManager()->AddFile(m_provider->GetVideo(), utf8::c(path).wc_str(), true) && m_pEditEntity->light)
	{
		m_pEditEntity->light->haloBitmap = utf8::c(file).wstr();
		return true;
	}
	else
	{
		return false;
	}
}

bool EntityEditor::LoadSprite(const char *file, const char *path)
{
	//if (m_provider->GetGraphicResourceManager()->AddFile(m_provider->GetVideo(), utf8::c(path).wc_str(), false))
	{
		m_pEditEntity->spriteFile = utf8::c(file).wstr();
		return true;
	}
	//else
	//{
	//	return false;
	//}
}

bool EntityEditor::LoadNormal(const char *file, const char *path)
{
	//if (m_provider->GetGraphicResourceManager()->AddFile(m_provider->GetVideo(), utf8::c(path).wc_str(), false))
	{
		m_pEditEntity->normalFile = utf8::c(file).wstr();
		return true;
	}
	//else
	//{
	//	return false;
	//}
}

bool EntityEditor::LoadGloss(const char *file, const char *path)
{
	//if (m_provider->GetGraphicResourceManager()->AddFile(m_provider->GetVideo(), utf8::c(path).wc_str(), false))
	{
		m_pEditEntity->glossFile = utf8::c(file).wstr();
		return true;
	}
	//else
	//{
	//	return false;
	//}
}

bool EntityEditor::LoadParticle(const int n, const char *file, const char *path)
{
	UnloadParticle(n);
	ETH_PARTICLE_SYSTEM parSystem;
	if (parSystem.ReadFromFile(utf8::c(path).wstr(), m_provider->GetVideo()->GetFileManager()))
	{
		*(m_pEditEntity->particleSystems[n].get()) = parSystem;
		m_attachLight.AddButton(
			utf8::c(m_pEditEntity->particleSystems[n]->bitmapFile).wc_str()
		);
		m_tool.ResetButtons();
		m_tool.ActivateButton(_S_EDIT_PARTICLES);
		return true;
	}
	else
	{
		return false;
	}
}

void EntityEditor::UnloadAll()
{
	for (int t=0; t<ETH_MAX_PARTICLE_SYS_PER_ENTITY; t++)
		UnloadParticle(t);
	UnloadHalo();
	UnloadSprite();
	UnloadNormal();
	UnloadGloss();
}

void EntityEditor::Clear()
{
	UnloadAll();
	m_pEditEntity->Reset();
	ResetEntityMenu();
	SetCurrentFile(_BASEEDITOR_DEFAULT_UNTITLED_FILE);
}

void EntityEditor::UnloadParticle(const int n)
{
	if (m_renderEntity->HasParticleSystem(n))
	{
		if (m_renderEntity->GetParticleSFX(n))
		{
			m_renderEntity->GetParticleSFX(n)->Stop();
		}
	}
	m_attachLight.DelButton(utf8::c(m_pEditEntity->particleSystems[n]->bitmapFile).wc_str());
	m_pEditEntity->particleSystems[n]->bitmapFile = L"";
	m_pEditEntity->particleSystems[n]->nParticles = 0;
	m_renderEntity->DestroyParticleSystem(n);
}

void EntityEditor::UnloadHalo()
{
	if (m_pEditEntity->light)
	{
		m_pEditEntity->light->haloBitmap = L"";
		m_renderEntity->SetHalo(L"");
	}
}

void EntityEditor::UnloadSprite()
{
	m_pEditEntity->spriteFile = L"";
	m_renderEntity->SetSprite(L"");
}

void EntityEditor::UnloadNormal()
{
	m_pEditEntity->normalFile = L"";
	m_renderEntity->SetNormal(L"");
}

void EntityEditor::UnloadGloss()
{
	m_pEditEntity->glossFile = L"";
	m_renderEntity->SetGloss(L"");
}

void EntityEditor::ShadowPrint(const Vector2 &v2Pos, const wchar_t *text)
{
	m_provider->GetVideo()->DrawBitmapText(v2Pos, text, L"Verdana14_shadow.fnt", GS_WHITE);
}

void EntityEditor::ShadowPrint(const Vector2 &v2Pos, const wchar_t *text, const wchar_t *font, const GS_DWORD color)
{
	m_provider->GetVideo()->DrawBitmapText(v2Pos, text, font, color);
}

bool EntityEditor::SpriteFrameChanged()
{
	bool r = true;
	if (m_pEditEntity->spriteCut.x != m_v2LastSpriteCut.x ||
		m_pEditEntity->spriteCut.y != m_v2LastSpriteCut.y ||
		m_pEditEntity->startFrame != m_lastStartFrame)
	{
		r = true;
	}
	else
		r = false;

	m_v2LastSpriteCut = m_pEditEntity->spriteCut;
	m_lastStartFrame = m_pEditEntity->startFrame;

	return r;
}

void EntityEditor::ResetSpriteCut()
{
	if (m_renderEntity->GetSprite() && m_pEditEntity->spriteCut.x > 0 && m_pEditEntity->spriteCut.y > 0)
	{
		m_renderEntity->GetSprite()->SetupSpriteRects(m_pEditEntity->spriteCut.x, m_pEditEntity->spriteCut.y);
		if (m_pEditEntity->startFrame >= 0 && m_pEditEntity->startFrame < (m_pEditEntity->spriteCut.x*m_pEditEntity->spriteCut.y))
		{
			m_renderEntity->GetSprite()->SetRect(m_pEditEntity->startFrame);
		}
		/*if (m_renderEntity.GetNormal())
		{
			m_renderEntity.GetNormal()->SetupSpriteRects(m_pEditEntity->spriteCut.x, m_pEditEntity->spriteCut.y);
			m_renderEntity.GetNormal()->SetRect(m_pEditEntity->startFrame);
		}*/
	}
	m_startFrame.SetClamp(true, 0, (float)(m_pEditEntity->spriteCut.x * m_pEditEntity->spriteCut.y - 1));
}

ButtonSprite::ButtonSprite()
{
	m_lHold = m_rHold = false;
}

bool ButtonSprite::MouseOver(InputPtr input, VideoPtr video, Vector2 v2Pos)
{
	const Vector2 m = input->GetCursorPositionF(video);
	Vector2 min, max;
	if (m_sprite->GetOrigin() == Vector2(0,0))
	{
		min = v2Pos;
		max = v2Pos+m_sprite->GetBitmapSizeF();
	}
	else
	{
		min = v2Pos-m_sprite->GetBitmapSizeF()/2;
		max = v2Pos+m_sprite->GetBitmapSizeF()/2;
	}
	if (m.x<min.x || m.x>max.x || m.y<min.y || m.y>max.y)
		return false;
	return true;
}

GS_KEY_STATE ButtonSprite::MouseOverLClick(InputPtr input, VideoPtr video, Vector2 v2Pos)
{
	const bool mouseOver = MouseOver(input, video, v2Pos);
	const GS_KEY_STATE state = input->GetLeftClickState();

	if (state == GSKS_HIT && mouseOver)
	{
		m_lHold = true;
		return GSKS_DOWN;
	}
	if (m_lHold && state == GSKS_DOWN)
		return GSKS_DOWN;

	if (state == GSKS_RELEASE || state == GSKS_UP)
		m_lHold = false;
	return GSKS_UP;
}

GS_KEY_STATE ButtonSprite::MouseOverRClick(InputPtr input, VideoPtr video, Vector2 v2Pos)
{
	const bool mouseOver = MouseOver(input, video, v2Pos);
	const GS_KEY_STATE state = input->GetRightClickState();

	if (state == GSKS_HIT && mouseOver)
	{
		m_rHold = true;
		return GSKS_DOWN;
	}
	if (m_rHold && state == GSKS_DOWN)
		return GSKS_DOWN;

	if (state == GSKS_RELEASE || state == GSKS_UP)
		m_rHold = false;
	return GSKS_UP;
}