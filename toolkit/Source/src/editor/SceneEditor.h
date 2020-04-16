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

#ifndef ETH_SCENE_EDITOR_H_
#define ETH_SCENE_EDITOR_H_

#include "EditorBase.h"
#include "CustomDataEditor.h"
#include "../engine/Scene/ETHScene.h"
#include "../engine/Shader/ETHShaderManager.h"
#include "../engine/Util/ETHFileChangeDetector.h"

#include <stdio.h>
#include <iostream>

#define _ETH_MAP_WINDOW_TITLE GS_L("Ethanon Scene Editor")

class SceneEditor : public EditorBase
{
public:
	SceneEditor(ETHResourceProviderPtr provider);
	~SceneEditor();
	void LoadEditor();
	std::string DoEditor(SpritePtr pNextAppButton, const float lastFrameElapsedTime);
	void ReloadFiles();
	void UpdateInternalData();
	void StopAllSoundFXs();
	void Clear();
	bool ProjectManagerRequested();

	void OpenByFilename(const std::string& filename);

private:

	void RebuildScene(const str_type::string& fileName);
	void EntityPlacer();
	void PlaceEntitySelection();
	void EntitySelector(const bool guiButtonsFree);
	GSGUI_BUTTON DoMainMenu();
	void RenderScene(const float lastFrameElapsedTime);
	void DrawGrid();
	void DoStateManager();
	bool IsThereAnyFieldActive() const;
	bool AreGUIButtonsFree(SpritePtr pNextAppButton) const;
	str_type::string DrawEntityString(ETHEntity *pEntity, const Color& color, const bool drawName = true);
	void SetSelectionMode();
	void SetPlacingMode();
	bool SaveAs();
	bool Save();
	bool Open();
	void ResetForms();
	void ShowLightmapMessage();
	void DrawEntitySelectionGrid(SpritePtr pNextAppButton);
	void LoopThroughEntityList();
	void UpdateEntities();
	int GetEntityByName(const std::string &name);
	void CopySelectedToClipboard();
	void PasteFromClipboard();
	bool CheckForFileUpdate();
	void CreateFileUpdateDetector(const str_type::string& fullFilePath);

	void CreateEditablePosition();
	float PlaceEditablePosition(const Vector2 &v2Pos);
	void SetEditablePositionPos(const Vector3 &pos, const float angle);
	bool AreEditPosFieldsFocusedByCursor() const;
	bool AreEditPosFieldsActive() const;
	Vector3 GetEditablePositionFieldPos() const;
	float GetEditablePositionFieldAngle() const;

	void EditParallax();
	void CentralizeShortcut();
	Vector3 m_v3Pos;
	Vector2 m_v2CamPos;

	std::vector<boost::shared_ptr<ETHEntityProperties> > m_entityFiles;
	struct less_than_key
	{
		inline bool operator() (const boost::shared_ptr<ETHEntityProperties>& a, const boost::shared_ptr<ETHEntityProperties>& b)
		{
			if (!a || !b) return false;
			return (a->entityName < b->entityName);
		}
	};

	// menu component position control
	float m_guiX, m_guiY;

	const float m_camSpeed;
	Vector2 m_slidingAxis;
	bool m_slidingCamera;

	ETHRenderEntity* m_currentEntity;
	boost::shared_ptr<ETHRenderEntity> m_clipBoard;
	int m_currentEntityIdx;

	GSGUI_FLOAT_INPUT m_ambientLight[3];
	GSGUI_FLOAT_INPUT m_zAxisDirection[2];
	GSGUI_FLOAT_INPUT m_lightIntensity;
	GSGUI_FLOAT_INPUT m_parallaxIntensity;
	GSGUI_FLOAT_INPUT m_entityPosition[3];
	GSGUI_FLOAT_INPUT m_entityAngle;
	GSGUI_STRING_LINE_INPUT m_entityName;

	GSGUI_BUTTONLIST m_renderMode;
	GSGUI_BUTTONLIST m_tool;
	GSGUI_BUTTONLIST m_panel;
	SpritePtr m_lSprite, m_pSprite, m_outline, m_richOutline, m_parallaxCursor;
	SpritePtr m_axis, m_soundWave, m_invisible, m_arrows;
	ETHRenderEntity* m_pSelected;
	ETHSceneProperties m_sceneProps;
	ETHScenePtr m_pScene;
	bool m_updateLights;
	bool m_fullscreen;
	bool m_projManagerRequested;
	int m_genLightmapForThisOneOnly;
	CustomDataEditor m_customDataEditor;
	ETHFileChangeDetectorPtr m_fileChangeDetector;
	ETHBackBufferTargetManagerPtr m_backBuffer;
};

#endif
