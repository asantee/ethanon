#ifndef ___ENTITY_EDITOR__3420520664358632
#define ___ENTITY_EDITOR__3420520664358632

#include "EditorBase.h"
#include "../engine/Scene/ETHScene.h"
#include "../engine/Util/ETHFrameTimer.h"
#include "../engine/Util/ETHFileChangeDetector.h"
#include "../engine/Shader/ETHShaderManager.h"
#include "CustomDataEditor.h"
#include "ParticleScale.h"

#include <stdio.h>
#include <iostream>

#define _ETH_WINDOW_TITLE GS_L("Ethanon Entity Editor")

class ButtonSprite
{
	bool m_lHold, m_rHold;
public:
	SpritePtr m_sprite;
	ButtonSprite();
	bool MouseOver(InputPtr input, VideoPtr video, Vector2 v2Pos);
	GS_KEY_STATE MouseOverLClick(InputPtr input, VideoPtr video, Vector2 v2Pos);
	GS_KEY_STATE MouseOverRClick(InputPtr input, VideoPtr video, Vector2 v2Pos);
};

class EntityEditor : public EditorBase
{
public:
	EntityEditor(ETHResourceProviderPtr provider);
	~EntityEditor();
	void LoadEditor();
	std::string DoEditor(SpritePtr pNextAppButton, const float lastFrameElapsedTime);
	void StopAllSoundFXs();
	void Clear();
	bool ProjectManagerRequested();

private:
	void InstantiateEntity(const std::string& fileName = GS_L(""));

	SpritePtr m_axis, m_range, m_outline;
	ButtonSprite m_spot, m_particleSpot[ETH_MAX_PARTICLE_SYS_PER_ENTITY];
	ETHSceneProperties m_sceneProps;
	ETHLight m_cursorLight;
	ETHFileChangeDetectorPtr m_fileChangeDetector;

	ETHEntityProperties *m_pEditEntity;
	boost::shared_ptr<ETHRenderEntity> m_renderEntity;
	CustomDataEditor m_customDataEditor;
	cParticleScale m_particleScale;
	ETHFrameTimer m_animationTimer;

	GSGUI_DROPDOWN m_addMenu;
	GSGUI_DROPDOWN m_delMenu;
	GSGUI_BUTTONLIST m_type;
	GSGUI_BUTTONLIST m_bodyShape;
	GSGUI_BUTTONLIST m_bodyProperties;
	GSGUI_BUTTONLIST m_bool;
	GSGUI_BUTTONLIST m_blendMode;
	GSGUI_BUTTONLIST m_renderMode;
	GSGUI_BUTTONLIST m_boolLight;
	GSGUI_BUTTONLIST m_tool;
	GSGUI_BUTTONLIST m_attachLight;
	GSGUI_FLOAT_INPUT m_density, m_friction, m_restitution, m_gravityScale;
	GSGUI_FLOAT_INPUT m_pivotX, m_pivotY;
	GSGUI_FLOAT_INPUT m_lightPos[3];
	GSGUI_FLOAT_INPUT m_lightColor[3];
	GSGUI_FLOAT_INPUT m_emissiveColor[3];
	GSGUI_FLOAT_INPUT m_lightRange;
	GSGUI_FLOAT_INPUT m_haloSize;
	GSGUI_FLOAT_INPUT m_haloAlpha;
	GSGUI_FLOAT_INPUT m_particlePos[ETH_MAX_PARTICLE_SYS_PER_ENTITY][3];
	GSGUI_FLOAT_INPUT m_startFrame;
	GSGUI_FLOAT_INPUT m_spriteCut[2];
	GSGUI_FLOAT_INPUT m_collisionPos[3];
	GSGUI_FLOAT_INPUT m_collisionSize[3];
	GSGUI_FLOAT_INPUT m_shadowScale;
	GSGUI_FLOAT_INPUT m_parallaxIntensity;
	GSGUI_FLOAT_INPUT m_shadowOpacity;
	GSGUI_FLOAT_INPUT m_shadowLength;
	GSGUI_FLOAT_INPUT m_layerDepth;
	GSGUI_FLOAT_INPUT m_specularPower;
	GSGUI_FLOAT_INPUT m_specularBrightness;
	GSGUI_FLOAT_INPUT m_animPreviewStride;
	GSGUI_SWAPBUTTON m_playStopButton;

	Vector2i m_v2LastSpriteCut;
	int m_lastStartFrame;
	bool SpriteFrameChanged();
	bool m_projManagerRequested;

	void DoMainMenu();
	void CreateCollisionBoxFromEntity();
	void ResetSpriteCut();
	bool LoadHalo(const char *file, const char *path);
	bool LoadSprite(const char *file, const char *path);
	bool LoadNormal(const char *file, const char *path);
	bool LoadGloss(const char *file, const char *path);
	bool LoadParticle(const int n, const char *file, const char *path);
	void UnloadAll();
	void UnloadHalo();
	void UnloadSprite();
	void UnloadNormal();
	void UnloadGloss();
	void UnloadParticle(const int n);
	void DrawEntity(const float lastFrameElapsedTime);
	void ResetEntityMenu();
	void ResetParticleMenu();
	void ResetLightMenu();
	void ShadowPrint(const Vector2 &v2Pos, const str_type::char_t *text);
	void ShadowPrint(const Vector2 &v2Pos, const str_type::char_t *text, const str_type::char_t *font, const GS_DWORD color);
	bool SaveAs();
	bool Save(const char *path);
	bool Open();
	void ShowWarnings();
	void ShowEntityResources(Vector2 v2Pos);
	void DrawEntityElementName(const Vector2 &v2Pos, SpritePtr pSprite, const std::string &name);
	bool CheckForFileUpdate();
	void CreateFileUpdateDetector(const std::string& fullFilePath);
	void OpenEntity(const char* fullFilePath);
};

#endif
