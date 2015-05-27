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

#include "../engine/Platform/FileListing.h"

#include "SceneEditor.h"
#include "EditorCommon.h"

#include "../engine/Entity/ETHRenderEntity.h"
#include "../engine/Resource/ETHDirectories.h"

#include <sstream>
#include <string>

#define _S_TOOL_MENU GS_L("Tools")
#define _S_PLACE GS_L("Place entity")
#define _S_SELECT GS_L("Select entity")
#define _ENTITY_SELECTION_BAR_HEIGHT (48.0f)
#define _ENTITY_SELECTION_BAR_PROXIMITY (192.0f)

SceneEditor::SceneEditor(ETHResourceProviderPtr provider) :
	EditorBase(provider),
	m_camSpeed(400.0f), m_slidingAxis(0,0),
	m_slidingCamera(false),
	m_currentEntity(0)
{
	m_currentEntityIdx = 0;
	m_updateLights = false;
	m_genLightmapForThisOneOnly =-1;
	m_fullscreen = false;
	m_projManagerRequested = false;
	m_guiX = m_guiY = 0.0f;
	m_pSelected = 0;
	m_backBuffer = ETHBackBufferTargetManager::Create(provider->GetVideo());
}

SceneEditor::~SceneEditor()
{
	if (m_currentEntity)
	{
		m_currentEntity->Release();
	}
}

void SceneEditor::StopAllSoundFXs()
{
}

bool SceneEditor::ProjectManagerRequested()
{
	const bool r = m_projManagerRequested;
	m_projManagerRequested = false;
	return r;
}

void SceneEditor::RebuildScene(const str_type::string& fileName)
{
	if (fileName != _ETH_EMPTY_SCENE_STRING)
	{
		ETHEntityCache entityCache;
		m_pScene = ETHScenePtr(
			new ETHScene(
				fileName,
				m_provider,
				ETHSceneProperties(),
				0,
				0,
				entityCache,
				_ETH_SCENE_EDITOR_BUCKET_SIZE));
	}
	else
	{
		m_pScene = ETHScenePtr(new ETHScene(m_provider, ETHSceneProperties(), 0, 0, _ETH_SCENE_EDITOR_BUCKET_SIZE));
	}
	m_sceneProps = *m_pScene->GetSceneProperties();
	m_pSelected = 0;
	m_genLightmapForThisOneOnly =-1;
	ResetForms();
	m_pScene->EnableLightmaps(true);

}

void SceneEditor::LoadEditor()
{
	CreateFileMenu();
	m_customDataEditor.LoadEditor(this);

	RebuildScene(_ETH_EMPTY_SCENE_STRING);

	const VideoPtr& video = m_provider->GetVideo();
	video->SetBGColor(m_background);

	const str_type::string programDirectory = m_provider->GetFileIOHub()->GetProgramDirectory();

	m_lSprite = video->CreateSprite(programDirectory + GS_L("data/l.png"), 0xFFFF00FF);
	m_lSprite->SetOrigin(Sprite::EO_CENTER);

	m_pSprite = video->CreateSprite(programDirectory + GS_L("data/p.png"), 0xFFFF00FF);
	m_pSprite->SetOrigin(Sprite::EO_CENTER);

	m_parallaxCursor = video->CreateSprite(programDirectory + GS_L("data/parallax.png"), 0xFFFF00FF);
	m_parallaxCursor->SetOrigin(Sprite::EO_CENTER);

	m_axis = video->CreateSprite(programDirectory + GS_L("data/axis.png"));
	m_axis->SetOrigin(Sprite::EO_CENTER);
	
	m_outline = video->CreateSprite(programDirectory + GS_L("data/outline.png"));
	m_richOutline = video->CreateSprite(programDirectory + GS_L("data/rich_outline.png"));

	m_soundWave = video->CreateSprite(programDirectory + GS_L("data/soundwave.png"));
	m_soundWave->SetOrigin(Sprite::EO_CENTER);

	m_invisible = video->CreateSprite(programDirectory + GS_L("data/invisible.png"), 0xFFFF00FF);
	m_invisible->SetOrigin(Sprite::EO_CENTER);

	m_arrows = video->CreateSprite(programDirectory + GS_L("data/arrows.png"));
	m_arrows->SetOrigin(Sprite::EO_CENTER);

	m_provider->SetEditorSprites(m_outline, m_invisible);

	const InputPtr& input = m_provider->GetInput();
	m_renderMode.SetupMenu(video, input, m_menuSize, m_menuWidth*2, true, false, false);

	const std::string programPath = programDirectory;
	str_type::string rmStatus = GetAttributeFromInfoFile("status", "renderMode");
	if (rmStatus != _S_USE_PS && rmStatus != _S_USE_VS)
	{
		rmStatus = _S_USE_PS;
	}
	m_renderMode.AddButton(_S_USE_PS, rmStatus == _S_USE_PS);
	m_renderMode.AddButton(_S_USE_VS, rmStatus == _S_USE_VS);

	m_panel.SetupMenu(video, input, m_menuSize, m_menuWidth*2, false, true, false);
	m_panel.AddButton(_S_GENERATE_LIGHTMAPS, false);
	m_panel.AddButton(_S_SAVE_LIGHTMAPS, false);
	m_panel.AddButton(_S_UPDATE_ENTITIES, false);
	m_panel.AddButton(_S_TOGGLE_STATIC_DYNAMIC, false);
	m_panel.AddButton(_S_TOGGLE_FLIPX, false);
	m_panel.AddButton(_S_TOGGLE_FLIPY, false);
	m_panel.AddButton(_S_LOCK_STATIC, true);

	// gets the last status saved into the info file
	std::string lmStatus = GetAttributeFromInfoFile("status", "lightmapsEnabled");
	lmStatus = lmStatus.empty() ? "true" : lmStatus;
	m_panel.AddButton(_S_USE_STATIC_LIGHTMAPS, ETHGlobal::IsTrue(lmStatus));

	// gets the last status saved into the info file
	lmStatus = GetAttributeFromInfoFile("status", "showCustomData");
	lmStatus = lmStatus.empty() ? "true" : lmStatus;
	m_panel.AddButton(_S_SHOW_CUSTOM_DATA, ETHGlobal::IsTrue(lmStatus));

	m_panel.AddButton(_S_SHOW_ADVANCED_OPTIONS, false);

	m_tool.SetupMenu(video, input, m_menuSize, m_menuWidth*2, true, false, false);
	m_tool.AddButton(_S_PLACE, false);
	m_tool.AddButton(_S_SELECT, true);

	for (unsigned int t=0; t<3; t++)
	{
		m_ambientLight[t].SetupMenu(video, input, m_menuSize, m_menuWidth, 5, false);
		m_ambientLight[t].SetClamp(true, 0, 1);
		m_ambientLight[t].SetScrollAdd(0.1f);
		m_ambientLight[t].SetDescription(GS_L("Scene ambient light"));
	}
	m_ambientLight[0].SetText(GS_L("Ambient R:"));
	m_ambientLight[1].SetText(GS_L("Ambient G:"));
	m_ambientLight[2].SetText(GS_L("Ambient B:"));

	for (unsigned int t=0; t<2; t++)
	{
		m_zAxisDirection[t].SetupMenu(video, input, m_menuSize, m_menuWidth, 5, false);
		m_zAxisDirection[t].SetClamp(false, 0, 0);
		m_zAxisDirection[t].SetScrollAdd(1.0f);
		m_zAxisDirection[t].SetDescription(GS_L("Scene z-axis direction in screen space"));
	}
	m_zAxisDirection[0].SetText(GS_L("Z-axis dir x:"));
	m_zAxisDirection[1].SetText(GS_L("Z-axis dir y:"));

	m_entityName.SetupMenu(video, input, m_menuSize, m_menuWidth*2, 24, false);

	CreateEditablePosition();

	m_lightIntensity.SetupMenu(video, input, m_menuSize, m_menuWidth, 5, false);
	m_lightIntensity.SetClamp(true, 0, 100.0f);
	m_lightIntensity.SetText(GS_L("Light intensity:"));
	m_lightIntensity.SetScrollAdd(0.25f);
	m_lightIntensity.SetDescription(GS_L("Global light intensity"));

	m_parallaxIntensity.SetupMenu(video, input, m_menuSize, m_menuWidth, 5, false);
	m_parallaxIntensity.SetClamp(false, 0, 100.0f);
	m_parallaxIntensity.SetText(GS_L("Parallax:"));
	m_parallaxIntensity.SetScrollAdd(0.25f);
	m_parallaxIntensity.SetDescription(GS_L("Parallax depth effect intensity"));

	ResetForms();

	m_pScene->EnableLightmaps(true);

	ReloadFiles();
}

void SceneEditor::ResetForms()
{
	m_ambientLight[0].SetConstant(m_sceneProps.ambient.x);
	m_ambientLight[1].SetConstant(m_sceneProps.ambient.y);
	m_ambientLight[2].SetConstant(m_sceneProps.ambient.z);
	m_lightIntensity.SetConstant(m_sceneProps.lightIntensity);
	m_parallaxIntensity.SetConstant(m_sceneProps.parallaxIntensity);
	m_zAxisDirection[0].SetConstant(m_sceneProps.zAxisDirection.x);
	m_zAxisDirection[1].SetConstant(m_sceneProps.zAxisDirection.y);
}

std::string SceneEditor::DoEditor(SpritePtr pNextAppButton)
{
	const VideoPtr& video = m_provider->GetVideo();
	const InputPtr& input = m_provider->GetInput();
	video->SetCameraPos(m_v2CamPos);

	// update lightmap status
	const bool lmEnabled = m_pScene->AreLightmapsEnabled();
	m_pScene->EnableLightmaps(m_panel.GetButtonStatus(_S_USE_STATIC_LIGHTMAPS));

	const std::string programPath = m_provider->GetFileIOHub()->GetProgramDirectory();

	// if the lightmaps status has changed, save it's status again
	if (lmEnabled != m_pScene->AreLightmapsEnabled())
	{
		SaveAttributeToInfoFile("status", "lightmapsEnabled",
			(m_pScene->AreLightmapsEnabled()) ? "true" : "false");
	}

	// go to the next frame if the user presses the F key while selecting an entity
	if (m_pSelected)
	{
		if (m_pSelected->GetID() >= 0)
		{
			if (input->GetKeyState(GSK_F) == GSKS_HIT)
			{
				if (m_pSelected)
				{
					unsigned int currentFrame = m_pSelected->GetFrame();
					currentFrame++;
					if (currentFrame >= m_pSelected->GetNumFrames())
					{
						currentFrame = 0;
					}
					m_pSelected->SetFrame(currentFrame);
				}
			}
		}
	}

	if (input->GetKeyState(GSK_ESC) == GSKS_HIT)
	{
		m_pSelected = 0;
	}

	const int wheel = (int)input->GetWheelState();
	const Vector2 v2Cursor = input->GetCursorPositionF(video);
	const bool guiButtonsFree = AreGUIButtonsFree(pNextAppButton);
	if (wheel != 0 && guiButtonsFree)
		SetPlacingMode();

	{
		const float fpsRate = video->GetFPSRate();
		const float camMoveSpeed = floor(m_camSpeed/fpsRate);
		if (!IsThereAnyFieldActive())
		{
			if (fpsRate > 0.0f)
			{
				if (input->IsKeyDown(GSK_LEFT))
					video->MoveCamera(Vector2(-camMoveSpeed,0));
				if (input->IsKeyDown(GSK_RIGHT))
					video->MoveCamera(Vector2(camMoveSpeed,0));
				if (input->IsKeyDown(GSK_UP))
					video->MoveCamera(Vector2(0,-camMoveSpeed));
				if (input->IsKeyDown(GSK_DOWN))
					video->MoveCamera(Vector2(0,camMoveSpeed));
			}
		}
		// border camera slide
		if (m_slidingCamera)
		{
			if (input->GetMiddleClickState() == GSKS_HIT
				|| input->GetLeftClickState() == GSKS_HIT
				|| input->GetRightClickState() == GSKS_HIT)
			{
				m_slidingCamera = false;
			}
			if (fpsRate > 0.0f)
			{
				const Vector2 v2Dir = (m_slidingAxis-v2Cursor)*-(camMoveSpeed/200);
				video->MoveCamera(Vector2(floor(v2Dir.x), floor(v2Dir.y)));
			}
		}
		else if (input->GetMiddleClickState() == GSKS_HIT && guiButtonsFree)
		{
			m_slidingCamera = true;
			if (m_slidingCamera)
			{
				m_slidingAxis = v2Cursor;
			}
		}
	}

	m_axis->Draw(Vector2(0,0));
	RenderScene();

	LoopThroughEntityList();

	if (m_tool.GetButtonStatus(_S_PLACE))
		PlaceEntitySelection();

	EntitySelector(guiButtonsFree);

	GSGUI_BUTTON file_r;
	if (!m_fullscreen)
		file_r = DoMainMenu();

	if (file_r.text == _S_NEW)
	{
		RebuildScene(_ETH_EMPTY_SCENE_STRING);		
		video->SetCameraPos(Vector2(0,0));
		SetCurrentFile(_BASEEDITOR_DEFAULT_UNTITLED_FILE);
	}
	if (file_r.text == _S_SAVE_AS)
	{
		SaveAs();
	}
	if (file_r.text == _S_SAVE || (input->GetKeyState(EDITOR_GSK_CONTROL) == GSKS_DOWN && input->GetKeyState(GSK_S) == GSKS_HIT))
	{
		if (GetCurrentFile(true) == _BASEEDITOR_DEFAULT_UNTITLED_FILE)
			SaveAs();
		else
			Save();
	}
	if (file_r.text == _S_OPEN)
	{
		Open();
	}
	if (file_r.text == _S_GOTO_PROJ)
	{
		m_projManagerRequested = true;
	}
	if (!video->WindowInFocus())
	{
		if (CheckForFileUpdate())
			OpenByFilename(GetCurrentFile(true).c_str());
	}

	if (guiButtonsFree && m_tool.GetButtonStatus(_S_PLACE))
		EntityPlacer();

	if (!m_fullscreen)
		DoStateManager();

	EditParallax();

	if (m_entityFiles.empty())
	{
		ShadowPrint(
			Vector2(256,400),
			GS_L("There are no entities\nCreate your .ENT files first"),
			GS_L("Verdana24_shadow.fnt"), gs2d::constant::WHITE);
	}

	if ((input->GetKeyState(GSK_DELETE) == GSKS_HIT || input->GetKeyState(GSK_MAC_DELETE) == GSKS_HIT) && guiButtonsFree)
	{
		if (m_pSelected)
		{
			const bool update = m_pSelected->HasShadow() || m_pSelected->HasLightSource();

			m_pScene->GetBucketManager().DeleteEntity(m_pSelected->GetID(), ETHBucketManager::GetBucket(m_pSelected->GetPositionXY(), m_pScene->GetBucketSize()));
			m_pSelected = 0;
			if (m_pScene->GetNumLights() && update)
			{
				ShowLightmapMessage();
				m_updateLights = true;
			}
		}
	}

	SetFileNameToTitle(video, _ETH_MAP_WINDOW_TITLE);
	m_v2CamPos = video->GetCameraPos();
	DrawEntitySelectionGrid(pNextAppButton);

	#if defined(_DEBUG) || defined(DEBUG)
		if (m_pSelected)
		{
			std::stringstream ss;
			ss << m_pSelected->GetID();

			video->DrawBitmapText(Vector2(100,100), ss.str(), GS_L("Verdana20_shadow.fnt"), gs2d::constant::WHITE
			);
		}
	#endif

	if (m_slidingCamera)
	{
		m_arrows->Draw(m_slidingAxis+m_v2CamPos);
	}

	if (input->IsKeyDown(EDITOR_GSK_CONTROL) && input->GetKeyState(GSK_C) == GSKS_HIT)
		CopySelectedToClipboard();
	if (input->IsKeyDown(EDITOR_GSK_CONTROL) && input->GetKeyState(GSK_V) == GSKS_HIT)
		PasteFromClipboard();

	CentralizeShortcut();
	return "scene";
}

bool SceneEditor::CheckForFileUpdate()
{
	if (m_fileChangeDetector)
	{
		m_fileChangeDetector->Update();
		return m_fileChangeDetector->CheckForChange();
	}
	else
	{
		return false;
	}
}

void SceneEditor::CreateFileUpdateDetector(const str_type::string& fullFilePath)
{
	m_fileChangeDetector = ETHFileChangeDetectorPtr(
		new ETHFileChangeDetector(m_provider->GetVideo(), fullFilePath, ETHFileChangeDetector::UTF16_WITH_BOM));
	if (!m_fileChangeDetector->IsValidFile())
		m_fileChangeDetector.reset();
}

void SceneEditor::EditParallax()
{
	const VideoPtr& video = m_provider->GetVideo();
	if (m_parallaxIntensity.IsActive())
	{
		const Vector2 &cursorPos = m_provider->GetInput()->GetCursorPositionF(video);
		m_provider->GetShaderManager()->SetParallaxNormalizedOrigin(cursorPos / video->GetScreenSizeF());
		m_parallaxCursor->Draw(cursorPos + video->GetCameraPos(), Color(100,255,255,255));
	}
	else
	{
		m_provider->GetShaderManager()->SetParallaxNormalizedOrigin(Vector2(0.5f,0.5f));
	}
}

void SceneEditor::Clear()
{
	m_pScene->ClearResources();
	RebuildScene(_ETH_EMPTY_SCENE_STRING);
	m_provider->GetVideo()->SetCameraPos(Vector2(0,0));
	SetCurrentFile(_BASEEDITOR_DEFAULT_UNTITLED_FILE);
	m_currentEntityIdx = 0;
}

void SceneEditor::SetSelectionMode()
{
	m_tool.ActivateButton(_S_SELECT);
	m_tool.DeactivateButton(_S_PLACE);
	m_panel.HideButton(_S_LOCK_STATIC, false);
	m_panel.HideButton(_S_TOGGLE_STATIC_DYNAMIC, false);
	m_panel.HideButton(_S_TOGGLE_FLIPX, false);
	m_panel.HideButton(_S_TOGGLE_FLIPY, false);
}
void SceneEditor::SetPlacingMode()
{
	m_tool.ActivateButton(_S_PLACE);
	m_tool.DeactivateButton(_S_SELECT);
	m_panel.HideButton(_S_LOCK_STATIC, true);
	m_panel.HideButton(_S_TOGGLE_STATIC_DYNAMIC, true);
	m_panel.HideButton(_S_TOGGLE_FLIPX, true);
	m_panel.HideButton(_S_TOGGLE_FLIPY, true);
}

void SceneEditor::EntitySelector(const bool guiButtonsFree)
{
	if (!m_tool.GetButtonStatus(_S_SELECT))
		return;

	const VideoPtr& video = m_provider->GetVideo();
	const InputPtr& input = m_provider->GetInput();

	const bool zBuffer = video->GetZBuffer();
	video->SetZBuffer(false);
	video->SetSpriteDepth(0.0f);
	const bool rightClick = (input->GetRightClickState() == GSKS_HIT);

	// is the user moving the entity around?
	static bool moving = false;

	// Looks for the target entity ID and selects it if it exists
	if ((input->GetLeftClickState() == GSKS_RELEASE || rightClick) && guiButtonsFree)
	{
		const Vector2 v2Cursor = input->GetCursorPositionF(video);

		const int id = m_pScene->GetBucketManager().SeekEntity(v2Cursor + video->GetCameraPos(), (reinterpret_cast<ETHEntity**>(&m_pSelected)), m_sceneProps, m_pSelected);

		if (m_pSelected)
		{
			if (m_pSelected->IsTemporary())
			{
				m_pSelected = 0;
			}
			if (id < 0)
			{
				m_pSelected = 0;
			}
		}

		if (m_pSelected)
		{
			m_entityName.SetActive(false);
			m_entityName.SetValue(m_pSelected->GetEntityName());

			SetEditablePositionPos(m_pSelected->GetPosition(), m_pSelected->GetAngle());
			#if defined(_DEBUG) || defined(DEBUG)
			std::cout << "(Scene editor)Selected entity: " << m_pSelected->GetID() << std::endl;
			#endif
		}
	}

	// handle selection
	if (m_pSelected)
	{
		// If the user is holding the left-button and the cursor is over the selected tile...
		// move it...
		if (input->GetLeftClickState() == GSKS_DOWN)
		{
			const Vector2 v2CursorPos = input->GetCursorPositionF(video);
			ETHEntityProperties::VIEW_RECT box = m_pSelected->GetScreenRect(*m_pScene->GetSceneProperties());
			if ((v2CursorPos.x > box.min.x && v2CursorPos.x < box.max.x &&
				v2CursorPos.y > box.min.y && v2CursorPos.y < box.max.y) || moving)
			{
				const bool lockStatic = m_panel.GetButtonStatus(_S_LOCK_STATIC);
				if ((m_pSelected->IsStatic() && !lockStatic) || !m_pSelected->IsStatic())
				{
					moving = true;
					m_pSelected->AddToPositionXY(input->GetMouseMoveF(), m_pScene->GetBucketManager());
					SetEditablePositionPos(m_pSelected->GetPosition(), m_pSelected->GetAngle());
				}
			}
		}

		// draws the current selected entity outline if we are on the selection mode
		if (!moving)
		{
			const Color dwColor(128,255,255,255);
			m_richOutline->SetOrigin(Sprite::EO_CENTER);
			const Vector2 v2Pos = m_pSelected->ComputeInScreenSpriteCenter(*m_pScene->GetSceneProperties()) + video->GetCameraPos();
			video->SetVertexShader(ShaderPtr());
			m_richOutline->DrawShaped(v2Pos, m_pSelected->GetCurrentSize(), dwColor, dwColor, dwColor, dwColor, m_pSelected->GetAngle());
			DrawEntityString(m_pSelected, gs2d::constant::WHITE);

			ShadowPrint(Vector2(m_guiX,m_guiY), GS_L("Entity name:")); m_guiY += m_menuSize;
			m_pSelected->ChangeEntityName(m_entityName.PlaceInput(Vector2(m_guiX,m_guiY))); m_guiY += m_menuSize;
			m_guiY += m_menuSize/2;

			// assign the position according to the position panel
			m_pSelected->SetPosition(GetEditablePositionFieldPos(), m_pScene->GetBucketManager());
			m_pSelected->SetAngle(GetEditablePositionFieldAngle());
		}
	}

	// show custom data editing options
	m_guiY += m_customDataEditor.DoEditor(Vector2(m_guiX, m_guiY), m_pSelected, this);

	// draw the outline to the cursor entity
	if (!moving)
	{
		const Vector2 v2Cursor = input->GetCursorPositionF(video);
		ETHEntity *pOver;
		const int id = m_pScene->GetBucketManager().SeekEntity(v2Cursor + video->GetCameraPos(), &pOver, m_sceneProps);
		if (id >= 0 && (!m_pSelected || m_pSelected->GetID() != id))
		{
			ETHRenderEntity *pSelected = reinterpret_cast<ETHRenderEntity*>(m_pScene->GetBucketManager().SeekEntity(id));
			if (pSelected)
			{
				const Color dwColor(48, 255, 255, 255);
				const Vector2& size(pSelected->GetCurrentSize());
				m_richOutline->SetOrigin(Sprite::EO_CENTER);
				video->SetVertexShader(ShaderPtr());
				m_richOutline->DrawShaped(pSelected->ComputeInScreenSpriteCenter(*m_pScene->GetSceneProperties()) + video->GetCameraPos(),
									  size, dwColor, dwColor, dwColor, dwColor, pSelected->GetAngle());
				DrawEntityString(pSelected, Color(100, 255, 255, 255));
			}
		}
	}

	// if the user is moving and has released the mouse button
	if (moving && input->GetLeftClickState() == GSKS_RELEASE)
	{
		moving = false;
	}

	video->SetZBuffer(zBuffer);
}

void SceneEditor::CreateEditablePosition()
{
	for (unsigned int t=0; t<3; t++)
	{
		m_entityPosition[t].SetupMenu(m_provider->GetVideo(), m_provider->GetInput(), m_menuSize, m_menuWidth, 10, false);
		m_entityPosition[t].SetClamp(false, 0, 0);
		m_entityPosition[t].SetScrollAdd(2.0f);
		m_entityPosition[t].SetDescription(GS_L("Entity position"));
	}
	m_entityPosition[0].SetText(GS_L("Position.x:"));
	m_entityPosition[1].SetText(GS_L("Position.y:"));
	m_entityPosition[2].SetText(GS_L("Position.z:"));

	m_entityAngle.SetupMenu(m_provider->GetVideo(), m_provider->GetInput(), m_menuSize, m_menuWidth, 10, false);
	m_entityAngle.SetClamp(false, 0, 0);
	m_entityAngle.SetScrollAdd(5.0f);
	m_entityAngle.SetDescription(GS_L("Entity angle"));
	m_entityAngle.SetText(GS_L("Angle:"));
}

bool SceneEditor::AreEditPosFieldsActive() const
{
	for (unsigned int t=0; t<3; t++)
	{
		if (m_entityPosition[t].IsActive())
			return true;
	}
	if (m_entityAngle.IsActive())
		return true;
	return false;
}

bool SceneEditor::AreEditPosFieldsFocusedByCursor() const
{
	for (unsigned int t=0; t<3; t++)
	{
		if (m_entityPosition[t].IsMouseOver())
			return true;
	}
	if (m_entityAngle.IsMouseOver())
		return true;
	return false;
}

float SceneEditor::PlaceEditablePosition(const Vector2 &v2Pos)
{
	ShadowPrint(v2Pos, GS_L("Entity position:"));
	for (unsigned int t=0; t<3; t++)
	{
		m_entityPosition[t].PlaceInput(v2Pos+Vector2(0,static_cast<float>(t+1)*m_menuSize), Vector2(0,0), m_menuSize);
	}
	m_entityAngle.PlaceInput(v2Pos+Vector2(0,4.0f*m_menuSize), Vector2(0,0), m_menuSize);
	return m_menuSize*5;
}

Vector3 SceneEditor::GetEditablePositionFieldPos() const
{
	const Vector3 r(m_entityPosition[0].GetLastValue(),
					   m_entityPosition[1].GetLastValue(),
					   m_entityPosition[2].GetLastValue());
	return r;
}

float SceneEditor::GetEditablePositionFieldAngle() const
{
	return m_entityAngle.GetLastValue();
}

void SceneEditor::SetEditablePositionPos(const Vector3 &pos, const float angle)
{
	const float *pPos = (float*)&pos;
	for (unsigned int t=0; t<3; t++)
	{
		m_entityPosition[t].SetConstant(pPos[t]);
	}
	m_entityAngle.SetConstant(angle);
}

str_type::string SceneEditor::DrawEntityString(ETHEntity *pEntity, const Color& dwColor, const bool drawName)
{
	ETHEntityProperties::VIEW_RECT box = pEntity->GetScreenRect(*m_pScene->GetSceneProperties());
	box.min -= Vector2(0,m_menuSize);
	str_type::stringstream sID;
	sID << pEntity->GetID() << " ";
	if (drawName)
		sID << pEntity->GetEntityName();
	ShadowPrint(box.min, sID.str().c_str(), dwColor);

	str_type::stringstream sPos;
	sPos << GS_L("pos: ") << ETHGlobal::Vector3ToString(pEntity->GetPosition()) << std::endl << ((pEntity->IsStatic()) ? GS_L("[static]") : GS_L("[dynamic]"));
	ShadowPrint(box.min+Vector2(0,m_menuSize), sPos.str().c_str(), Color(dwColor.a/2,dwColor.r,dwColor.g,dwColor.b));
	return sID.str()+GS_L("\n")+sPos.str();
}

bool SceneEditor::IsThereAnyFieldActive() const
{
	for (unsigned int t=0; t<3; t++)
	{
		if (m_ambientLight[t].IsActive() || m_entityPosition[t].IsActive())
			return true;
	}
	if (m_entityAngle.IsActive())
		return true;
	for (unsigned int t=0; t<2; t++)
	{
		if (m_zAxisDirection[t].IsActive())
			return true;
	}

	if (m_entityName.IsActive())
		return true;

	if (m_fileMenu.IsActive())
		return true;

	if (m_lightIntensity.IsActive())
		return true;

	if (m_parallaxIntensity.IsActive())
		return true;

	if (m_customDataEditor.IsFieldActive())
		return true;

	return false;
}

bool SceneEditor::AreGUIButtonsFree(SpritePtr pNextAppButton) const
{
	for (unsigned int t=0; t<3; t++)
	{
		if ((m_ambientLight[t].IsMouseOver() || m_ambientLight[t].IsActive()))
			return false;
		if (m_entityPosition[t].IsMouseOver() || m_entityPosition[t].IsActive())
			return false;
	}
	if (m_entityAngle.IsMouseOver() || m_entityAngle.IsActive())
		return false;
	for (unsigned int t=0; t<2; t++)
	{
		if ((m_zAxisDirection[t].IsMouseOver() || m_zAxisDirection[t].IsActive()))
			return false;
	}

	const Vector2 v2Screen = m_provider->GetVideo()->GetScreenSizeF();
	const Vector2 v2Size = (pNextAppButton) ? pNextAppButton->GetBitmapSizeF() : Vector2(32,32);
	const Vector2 v2Pos = v2Screen-v2Size;
	const Vector2 v2Cursor = m_provider->GetInput()->GetCursorPositionF(m_provider->GetVideo());

	// if the cursor is over the selection bar...
	if (v2Cursor.y > v2Screen.y-_ENTITY_SELECTION_BAR_HEIGHT)
		return false;

	// if the cursor is over the tab area
	if (v2Cursor.y < m_menuSize*2)
		return false;

	// if the cursor is over any of the cCustomDataEditor object
	if (!m_customDataEditor.IsCursorAvailable())
		return false;

	// If the cursor is over it, don't let it place
	if (AreEditPosFieldsFocusedByCursor() || AreEditPosFieldsActive() || m_panel.IsMouseOver()
		|| m_entityName.IsActive()/* || m_entityName.IsMouseOver()*/)
		return false;

	return (!IsFileMenuActive() && !IsMouseOverFileMenu() && !m_fileMenu.IsActive()
			&& !m_renderMode.IsMouseOver()
			&& !m_lightIntensity.IsActive() && !m_lightIntensity.IsMouseOver()
			&& !m_parallaxIntensity.IsActive() && !m_parallaxIntensity.IsMouseOver()
			&& !m_tool.IsMouseOver() && (v2Cursor.x < v2Pos.x || v2Cursor.y < v2Pos.y));
}

GSGUI_BUTTON SceneEditor::DoMainMenu()
{
	GSGUI_BUTTON file_r = PlaceFileMenu();

	return file_r;
}

void SceneEditor::DoStateManager()
{
	if (m_provider->GetInput()->GetRightClickState() == GSKS_HIT)
	{
		SetSelectionMode();
	}

	m_guiX = m_provider->GetVideo()->GetScreenSizeF().x-m_menuWidth*2;
	m_guiY = m_menuSize+m_menuSize*2;
	m_guiY+=m_menuSize/2;

	str_type::stringstream ss;
	const unsigned int nEntities = m_pScene->GetNumEntities();
	ss << GS_L("Entities in scene: ") << m_pScene->GetNumRenderedEntities() << GS_L("/") << nEntities;
	ShadowPrint(Vector2(m_guiX, m_guiY), ss.str().c_str(), Color(255,255,255,255)); m_guiY += m_menuSize;
	m_guiY += m_menuSize/2;

	ShadowPrint(Vector2(m_guiX,m_guiY), GS_L("Lighting mode:")); m_guiY+=m_menuSize;
	m_renderMode.PlaceMenu(Vector2(m_guiX,m_guiY)); m_guiY += m_menuSize*m_renderMode.GetNumButtons();

	const ETHShaderManagerPtr& shaderManager = m_provider->GetShaderManager();
	const bool usePS = shaderManager->IsUsingPixelShader();
	shaderManager->UsePS(m_renderMode.GetButtonStatus(_S_USE_PS));
	m_guiY+=m_menuSize/2;

	const std::string programPath = m_provider->GetFileIOHub()->GetProgramDirectory();

	// if this button's status has changed, save the change to the ENML file
	if (usePS != shaderManager->IsUsingPixelShader())
	{
		const str_type::string str = shaderManager->IsUsingPixelShader() ? _S_USE_PS : _S_USE_VS;
		SaveAttributeToInfoFile("status", "renderMode", str);
	}

	ShadowPrint(Vector2(m_guiX, m_guiY), _S_TOOL_MENU); m_guiY+=m_menuSize;
	m_tool.PlaceMenu(Vector2(m_guiX, m_guiY)); m_guiY += m_menuSize * m_renderMode.GetNumButtons();
	m_guiY+=m_menuSize/2;

	const Vector2 v2DescPos(0.0f, m_provider->GetVideo()->GetScreenSizeF().y-m_menuSize);
	m_sceneProps.ambient.x = m_ambientLight[0].PlaceInput(Vector2(m_guiX,m_guiY), v2DescPos, m_menuSize); m_guiY += m_menuSize;
	m_sceneProps.ambient.y = m_ambientLight[1].PlaceInput(Vector2(m_guiX,m_guiY), v2DescPos, m_menuSize); m_guiY += m_menuSize;
	m_sceneProps.ambient.z = m_ambientLight[2].PlaceInput(Vector2(m_guiX,m_guiY), v2DescPos, m_menuSize); m_guiY += m_menuSize;
	m_sceneProps.lightIntensity = m_lightIntensity.PlaceInput(Vector2(m_guiX,m_guiY), v2DescPos, m_menuSize); m_guiY += m_menuSize;
	m_sceneProps.parallaxIntensity = m_parallaxIntensity.PlaceInput(Vector2(m_guiX,m_guiY), v2DescPos, m_menuSize); m_guiY += m_menuSize;
	m_guiY+=m_menuSize/2;
	if (m_panel.GetButtonStatus(_S_SHOW_ADVANCED_OPTIONS))
	{
		m_sceneProps.zAxisDirection.x = m_zAxisDirection[0].PlaceInput(Vector2(m_guiX,m_guiY), v2DescPos, m_menuSize); m_guiY += m_menuSize;
		m_sceneProps.zAxisDirection.y = m_zAxisDirection[1].PlaceInput(Vector2(m_guiX,m_guiY), v2DescPos, m_menuSize); m_guiY += m_menuSize;
	}
	m_pScene->SetSceneProperties(m_sceneProps);
	m_guiY+=m_menuSize/2;

	// save the last button status to compare to the new one and check if it has changed
	const bool showCustomData = m_panel.GetButtonStatus(_S_SHOW_CUSTOM_DATA);

	m_panel.PlaceMenu(Vector2(m_guiX, m_guiY)); m_guiY += m_menuSize*m_panel.GetNumButtons();
	m_guiY+=m_menuSize/2;

	// if the button status has changed, save it
	if (showCustomData != m_panel.GetButtonStatus(_S_SHOW_CUSTOM_DATA))
	{
		SaveAttributeToInfoFile("status", "showCustomData", (!showCustomData) ? "true" : "false");
	}

	// if the user clicked to update lightmaps...
	if (m_panel.GetButtonStatus(_S_GENERATE_LIGHTMAPS))
	{
		m_updateLights = true;
		ShowLightmapMessage();
		m_panel.DeactivateButton(_S_GENERATE_LIGHTMAPS);
	}

	// if the user clicked to update entities...
	if (m_panel.GetButtonStatus(_S_UPDATE_ENTITIES) || m_provider->GetInput()->GetKeyState(GSK_F5) == GSKS_HIT)
	{
		UpdateEntities();
		m_updateLights = true;
		ShowLightmapMessage();
		m_panel.DeactivateButton(_S_UPDATE_ENTITIES);
	}

	// if the user clicked to save lightmaps button...
	if (m_panel.GetButtonStatus(_S_SAVE_LIGHTMAPS))
	{
		m_panel.DeactivateButton(_S_SAVE_LIGHTMAPS);
		if (m_pScene)
		{
			str_type::string directoryName = m_pScene->ConvertFileNameToLightmapDirectory(GetCurrentFile(true));
			Platform::FixSlashes(directoryName);
			Platform::CreateDirectory(directoryName.c_str());
			m_pScene->SaveLightmapsToFile(directoryName);
		}
	}

	// if the user clicked to toggle form
	if (m_panel.GetButtonStatus(_S_TOGGLE_STATIC_DYNAMIC))
	{
		if (m_pSelected)
		{
			if (m_pSelected->IsStatic())
			{
				m_pSelected->TurnDynamic();
			}
			else
			{
				m_pSelected->TurnStatic();
			}

			// if it has a shadow, generate lightmaps again
			if (m_pSelected->HasShadow() || m_pSelected->HasLightSource())
			{
				if (!m_pSelected->HasShadow() && !m_pSelected->HasLightSource())
				{
					m_genLightmapForThisOneOnly = m_pSelected->GetID();
				}
				m_updateLights = true;
				ShowLightmapMessage();
			}
		}
		m_panel.DeactivateButton(_S_TOGGLE_STATIC_DYNAMIC);
	}

	// Flip toggle buttons
	if (m_panel.GetButtonStatus(_S_TOGGLE_FLIPX))
	{
		if (m_pSelected)
		{
			m_pSelected->SetFlipX(!m_pSelected->GetFlipX());
		}
		m_panel.DeactivateButton(_S_TOGGLE_FLIPX);
	}
	if (m_panel.GetButtonStatus(_S_TOGGLE_FLIPY))
	{
		if (m_pSelected)
		{
			m_pSelected->SetFlipY(!m_pSelected->GetFlipY());
		}
		m_panel.DeactivateButton(_S_TOGGLE_FLIPY);
	}

	if (m_tool.GetButtonStatus(_S_PLACE) && m_currentEntity)
	{
		ShadowPrint(
			Vector2(m_guiX,m_guiY), 
			m_currentEntity->GetEntityName().c_str()
		); 
		m_guiY+=m_menuSize;
	}

	if (m_pSelected && m_tool.GetButtonStatus(_S_SELECT))
	{
		// if the current selected entity is valid, draw the fields
		if (m_pScene->GetBucketManager().SeekEntity(m_pSelected->GetID()))
		{
			m_guiY +=m_menuSize/2;
			m_guiY += PlaceEditablePosition(Vector2(m_guiX,m_guiY));
			m_guiY +=m_menuSize/2;
		}
	}

}

void SceneEditor::DrawGrid()
{
	const Vector2 currentSize(m_currentEntity->GetCurrentSize());
	const VideoPtr& video = m_provider->GetVideo();

	int sizeX = (int)currentSize.x;
	int sizeY = (int)currentSize.y;
	const float oldWidth = video->GetLineWidth();
	const Vector2 v2Screen = video->GetScreenSizeF();
	const Vector2 v2CamPos = video->GetCameraPos();
	video->SetCameraPos(Vector2(0,0));
	const Color dwLineColor = ARGB(70,255,255,255);
	video->SetLineWidth(2);

	Vector2 v2PosFix;
	if (m_currentEntity->GetType() == ETHEntityProperties::ET_VERTICAL)
	{
		sizeY = (int)currentSize.x;
	}
	else
	{
		sizeY = (int)currentSize.y;
	}
	v2PosFix.x = float((int)v2CamPos.x%(sizeX));
	v2PosFix.y = float((int)v2CamPos.y%(sizeY));

	// draw vertical lines
	for (unsigned int t=0;; t++)
	{
		const Vector2 a((float)t*(float)sizeX-v2PosFix.x, 0);
		const Vector2 b((float)t*(float)sizeX-v2PosFix.x, v2Screen.y);
		video->DrawLine(a, b, dwLineColor, dwLineColor);
		if (a.x > v2Screen.x)
			break;
	}

	// draw horizontal lines
	for (unsigned int t=0;; t++)
	{
		const Vector2 a(0, (float)t*(float)sizeY-v2PosFix.y);
		const Vector2 b(v2Screen.x, (float)t*(float)sizeY-v2PosFix.y);
		video->DrawLine(a, b, dwLineColor, dwLineColor);
		if (a.y > v2Screen.y)
			break;
	}

	video->SetLineWidth(oldWidth);
	video->SetCameraPos(v2CamPos);
}

void SceneEditor::PlaceEntitySelection()
{
	if (m_currentEntityIdx < 0)
	{
		return;
	}

	if (m_entityFiles.empty())
	{
		return;
	}

	const InputPtr& input = m_provider->GetInput();
	const VideoPtr& video = m_provider->GetVideo();
	const ETHShaderManagerPtr& shaderManager = m_provider->GetShaderManager();

	// select between entity list with home/end buttons
	if (input->GetKeyState(GSK_HOME) == GSKS_HIT)
		m_currentEntityIdx--;
	if (input->GetKeyState(GSK_END) == GSKS_HIT)
		m_currentEntityIdx++;

	const int wheelValue = static_cast<int>(input->GetWheelState());

	// is the mouse wheel available for entity switching?
	const bool allowSwapWheel = AreGUIButtonsFree(SpritePtr());

	if (allowSwapWheel)
	{
		m_currentEntityIdx += wheelValue;

		if (m_currentEntityIdx >= static_cast<int>(m_entityFiles.size()))
			m_currentEntityIdx = 0;
		if (m_currentEntityIdx < 0)
			m_currentEntityIdx = static_cast<int>(m_entityFiles.size()) - 1;

		// move the entity up and down (along the Z axis)
		if (input->GetKeyState(GSK_PAGEUP) == GSKS_HIT)
		{
			m_v3Pos.z+=4;
		}
		if (input->GetKeyState(GSK_PAGEDOWN) == GSKS_HIT)
		{
			m_v3Pos.z-=4;
		}

		// if the user has changed the current entity, return it's height to 0
		if (wheelValue != 0)
		{
			m_v3Pos.z = 0.0f;
		}
	}
	// Reset the current entity according to the selected one
	std::string currentProjectPath = GetCurrentProjectPath(true);

	// save the current angle to restore after entity reset
	const float angle = (m_currentEntity) ? m_currentEntity->GetAngle() : 0.0f;

	if (m_currentEntity)
		m_currentEntity->Release();

	m_currentEntity = new ETHRenderEntity(m_provider, *m_entityFiles[m_currentEntityIdx].get(), angle, 1.0f);
	m_currentEntity->SetOrphanPosition(m_v3Pos);
	//m_currentEntity->SetFrame(m_entityFiles[m_currentEntityIdx]->startFrame);
	m_currentEntity->SetAngle(angle);

	const Vector2 v2CamPos(video->GetCameraPos());
	Vector2 v2Cursor(input->GetCursorPositionF(video));
	const Vector2 screenSize(video->GetScreenSizeF());
	m_v3Pos = Vector3(v2Cursor.x+v2CamPos.x, v2Cursor.y+v2CamPos.y, m_v3Pos.z);

	const Vector2 currentSize(m_currentEntity->GetCurrentSize());
	if (input->GetKeyState(GSK_SHIFT) == GSKS_DOWN && currentSize != Vector2(0,0))
	{
		int sizeX = (int)currentSize.x;
		int sizeY = (int)currentSize.y;

		const float angle = m_currentEntity->GetAngle();
		if (angle != 0.0f && m_currentEntity->GetType() == ETHEntityProperties::ET_HORIZONTAL)
		{
			const float fSizeX = (float)sizeX;
			const float fSizeY = (float)sizeY;
			const float a  = DegreeToRadian(90.0f - angle);
			const float b  = DegreeToRadian(90.0f - (90.0f - angle));
			const float e  = DegreeToRadian(90.0f - (90.0f - angle));
			const float bl = DegreeToRadian(90.0f - (90.0f - (90.0f - angle)));
			const float A  = sinf(a)/(1.0f/Max(1.0f, fSizeX));
			const float B  = sinf(b)/(1.0f/Max(1.0f, fSizeX));
			const float F  = 1/(sinf(bl)/B);
			const float E  = sinf(e)/(1.0f/F);
			const float Bl = sinf(bl)/(1.0f/Max(1.0f, fSizeY));
			sizeX = int((A+E)/2.0f);

			if (fSizeX < fSizeY)
				sizeY = int(B);
			else
				sizeY = int(Bl);
		}

		Vector2 v2PosFix;
		if (m_currentEntity->GetType() == ETHEntityProperties::ET_VERTICAL)
		{
			sizeY = (int)currentSize.x;
			v2PosFix.x = float((int)Abs(m_v3Pos.x)%Max(1, (sizeX/4)));
			v2PosFix.y = float((int)Abs(m_v3Pos.y)%Max(1, (sizeY/4)));
		}
		else
		{
			//sizeY = (int)currentSize.y;
			v2PosFix.x = float((int)Abs(m_v3Pos.x)%Max(1, (sizeX/2)));
			v2PosFix.y = float((int)Abs(m_v3Pos.y)%Max(1, (sizeY/2)));
		}

		const float division = (m_currentEntity->GetType() == ETHEntityProperties::ET_VERTICAL) ? 4.0f : 2.0f;

		if (m_v3Pos.x > 0)
		{
			m_v3Pos.x -= v2PosFix.x;
			m_v3Pos.x += (float)sizeX/division;
		}
		else
		{
			m_v3Pos.x += v2PosFix.x;
			m_v3Pos.x -= (float)sizeX/division;
		}

		if (m_v3Pos.y > 0)
		{
			m_v3Pos.y -= v2PosFix.y;
			m_v3Pos.y += (float)sizeY/division;
		}
		else
		{
			m_v3Pos.y += v2PosFix.y;
			m_v3Pos.y -= (float)sizeY/division;
		}
		DrawGrid();
	}
	else
	{
		if (m_currentEntity->GetType() != ETHEntityProperties::ET_VERTICAL)
		{
			const Vector2 v2TipPos(0.0f, screenSize.y-m_menuSize-m_menuSize-_ENTITY_SELECTION_BAR_HEIGHT);
			ShadowPrint(v2TipPos, GS_L("You can hold SHIFT to align the entity like in a tile map"), Color(255,255,255,255));
		}
	}

	// Controls the angle
	if (m_currentEntity->GetType() != ETHEntityProperties::ET_VERTICAL && allowSwapWheel)
	{
		if (input->GetKeyState(GSK_Q) == GSKS_HIT)
			m_currentEntity->AddToAngle(5);
		if (input->GetKeyState(GSK_W) == GSKS_HIT)
			m_currentEntity->AddToAngle(-5);
	}
	else
	{
		m_currentEntity->SetAngle(0.0f);
	}

	// Min and max screen depth for the temporary current entity
	const float maxDepth = screenSize.y;
	const float minDepth =-screenSize.y;

	// draw a shadow preview for the current entity
	ETHLight shadowLight(true);
	shadowLight.castShadows = true; shadowLight.range = ETH_DEFAULT_SHADOW_RANGE * 10;
	shadowLight.pos = m_currentEntity->GetPosition()+Vector3(100, 100, 0); shadowLight.color = Vector3(0.5f, 0.5f, 0.5f);
	if (m_currentEntity->GetEntityName() != GS_L(""))
	{
		if (shaderManager->BeginShadowPass(m_currentEntity, &shadowLight, maxDepth, minDepth))
		{
			m_currentEntity->DrawShadow(maxDepth, minDepth, *m_pScene->GetSceneProperties(), shadowLight, 0, false, false);
			shaderManager->EndShadowPass();
		}
	}

	const bool zBuffer = video->GetZBuffer();
	video->SetZBuffer(false);
	// draws the current entity ambient pass
	if (shaderManager->BeginAmbientPass(m_currentEntity, video->GetScreenSizeF().y, -video->GetScreenSizeF().y))
	{
		if (!m_currentEntity->IsInvisible())
		{
			m_currentEntity->DrawAmbientPass(maxDepth, minDepth, false, *m_pScene->GetSceneProperties(), shaderManager->GetParallaxIntensity());
		}
		else
		{
			if (m_currentEntity->IsCollidable())
			{
				m_currentEntity->DrawCollisionBox(m_outline, gs2d::constant::WHITE, m_sceneProps.zAxisDirection);
			}
			else
			{
				video->SetVertexShader(ShaderPtr());
				m_invisible->Draw(m_currentEntity->GetPositionXY());
			}
		}
		shaderManager->EndAmbientPass();
	}
	video->SetZBuffer(zBuffer);

	// draws the light symbol to show that this entity has light
	if (m_entityFiles[m_currentEntityIdx]->light)
	{
		Vector3 v3Light = m_v3Pos + m_entityFiles[m_currentEntityIdx]->light->pos;
		m_lSprite->Draw(Vector2(v3Light.x, v3Light.y - v3Light.z),
						Vector4(m_entityFiles[m_currentEntityIdx]->light->color, 1.0f));
	}

	// draws the particle symbols to show that the entity has particles
	for (std::size_t t=0; t<m_entityFiles[m_currentEntityIdx]->particleSystems.size(); t++)
	{
		if (m_entityFiles[m_currentEntityIdx]->particleSystems[t]->nParticles > 0) //-V807
		{
			const Vector3 v3Part = m_v3Pos+m_entityFiles[m_currentEntityIdx]->particleSystems[t]->startPoint;
			m_pSprite->Draw(Vector2(v3Part.x, v3Part.y-v3Part.z),
							(m_entityFiles[m_currentEntityIdx]->particleSystems[t]->color0));
		}
	}

	// if the current entity has lights... lit the scene this time
	if (m_currentEntity->HasLightSource())
	{
		ETHLight light = *m_currentEntity->GetLight();
		light.pos.x += v2Cursor.x + v2CamPos.x;
		light.pos.y += v2Cursor.y + v2CamPos.y;
		light.staticLight = false;
		m_pScene->AddLight(light);
	}
}

void SceneEditor::UpdateInternalData()
{
	if (m_updateLights && m_pScene->AreLightmapsEnabled())
	{
		m_pScene->GenerateLightmaps(m_genLightmapForThisOneOnly);
		m_updateLights = false;
		m_genLightmapForThisOneOnly =-1;
	}
}

void SceneEditor::EntityPlacer()
{
	if (m_entityFiles.empty())
		return;

	if (m_provider->GetInput()->GetLeftClickState() == GSKS_HIT)
	{
		std::string currentProjectPath = GetCurrentProjectPath(true);
		ETHRenderEntity* entity = new ETHRenderEntity(m_provider, *m_currentEntity->GetProperties(), m_currentEntity->GetAngle(), 1.0f);
		entity->SetOrphanPosition(m_v3Pos);
		entity->SetAngle(m_currentEntity->GetAngle());
		entity->SetColor(ETH_WHITE_V4);

		const int id = m_pScene->AddEntity(entity);
		if (m_pScene->CountLights() && (m_currentEntity->IsStatic()))
		{
			m_updateLights = true;

			// if possible, generate the lightmap for only this one
			if (!entity->HasShadow() && !entity->HasLightSource())
			{
				m_genLightmapForThisOneOnly = id;
			}
			else
			{
				ShowLightmapMessage();
			}
		}
	}

	// Show entity info
	str_type::stringstream ss;
	ss << ETHGlobal::Vector3ToString(m_currentEntity->GetPosition()) << std::endl;
	const float angle = m_currentEntity->GetAngle();
	if (angle != 0.0f)
	{
		ss.precision(0);
		ss.setf(std::ios_base::fixed | std::ios_base::dec, std::ios_base::floatfield);
		ss << angle;
	}
	ShadowPrint(m_currentEntity->GetScreenRect(*m_pScene->GetSceneProperties()).max, ss.str().c_str(), Color(100, 255, 255, 255));
}

void SceneEditor::DrawEntitySelectionGrid(SpritePtr pNextAppButton)
{
	const InputPtr& input = m_provider->GetInput();
	const VideoPtr& video = m_provider->GetVideo();

	const Vector2 v2Cursor = input->GetCursorPositionF(video);
	const Vector2 v2Screen = video->GetScreenSizeF();
	const Vector2 v2HalfTile(_ENTITY_SELECTION_BAR_HEIGHT/2, _ENTITY_SELECTION_BAR_HEIGHT/2);
	//const Vector2 v2Tile(_ENTITY_SELECTION_BAR_HEIGHT, _ENTITY_SELECTION_BAR_HEIGHT);
	static const float barHeight = _ENTITY_SELECTION_BAR_HEIGHT;
	const float barWidth = v2Screen.x-pNextAppButton->GetBitmapSizeF().x;
	const float barPosY = v2Screen.y-barHeight;
	const GSGUI_STYLE *pStyle = m_fileMenu.GetGUIStyle();
	video->DrawRectangle(Vector2(0, barPosY), Vector2(barWidth, barHeight),
		0x00000000, 0x00000000, pStyle->inactive_bottom, pStyle->inactive_bottom, 0.0f);

	if (m_entityFiles.empty())
		return;

	const bool zBuffer = video->GetZBuffer();
	const bool roundUp = video->IsRoundingUpPosition();
	video->SetZBuffer(false);
	video->RoundUpPosition(true);

	const float fullLengthSize = _ENTITY_SELECTION_BAR_HEIGHT*static_cast<float>(m_entityFiles.size());
	float globalOffset = 0.0f;

	// adjust the horizontal offset
	if (fullLengthSize > barWidth)
		globalOffset = (-(Clamp(v2Cursor.x, 0.0f, barWidth)/barWidth)*(fullLengthSize-barWidth));

	float offset = 0.0f;
	float heightOffset = _ENTITY_SELECTION_BAR_HEIGHT;

	// adjust the vertical offset
	if (v2Cursor.y >= v2Screen.y-_ENTITY_SELECTION_BAR_PROXIMITY)
	{
		float cursorProximityFactor = (v2Cursor.y-(v2Screen.y-_ENTITY_SELECTION_BAR_PROXIMITY-_ENTITY_SELECTION_BAR_HEIGHT))/_ENTITY_SELECTION_BAR_PROXIMITY;
		cursorProximityFactor = Max(0.0f, Min(cursorProximityFactor, 1.0f));
		heightOffset *= (1-cursorProximityFactor);
	}
	else
	{
		return;
	}

	str_type::string textToDraw;
	const Vector2 v2Cam = video->GetCameraPos();
	video->SetCameraPos(Vector2(0,0));
	for (std::size_t t = 0; t < m_entityFiles.size(); t++)
	{
		SpritePtr pSprite = m_provider->GetGraphicResourceManager()->GetPointer(
			video, 
			m_entityFiles[t]->spriteFile, GetCurrentProjectPath(true),
			ETHDirectories::GetEntityDirectory(), false
		);

		if (!m_entityFiles[t]->spriteFile.empty() && !pSprite)
		{
			video->Message(str_type::string(GS_L("Removing entity from list: ")) + m_entityFiles[t]->entityName, GSMT_WARNING);
			m_entityFiles.erase(m_entityFiles.begin() + t);
			t--;
			continue;
		}

		const Vector2 v2Pos = Vector2(offset+globalOffset, barPosY+heightOffset)+v2HalfTile;

		// skip it if it's not yet visible
		if (v2Pos.x+_ENTITY_SELECTION_BAR_HEIGHT < 0)
		{
			offset += _ENTITY_SELECTION_BAR_HEIGHT;
			continue;
		}

		float bias = 1.0f;
		if (pSprite)
		{
			//const unsigned int nFrame = pSprite->GetCurrentRect();
			if (m_entityFiles[t]->spriteCut.x > 1 || m_entityFiles[t]->spriteCut.y > 1) //-V807
			{
				pSprite->SetupSpriteRects(m_entityFiles[t]->spriteCut.x, m_entityFiles[t]->spriteCut.y);
				//pSprite->SetRect(m_entityFiles[t]->startFrame);
				pSprite->SetRect(0);
			}
			else
			{
				pSprite->UnsetRect();
			}
			const Vector2 v2Size = pSprite->GetFrameSize();
			const float largestSize = Max(v2Size.x, v2Size.y);
			Vector2 v2NewSize = v2Size;
			if (v2Size.x > _ENTITY_SELECTION_BAR_HEIGHT || v2Size.y > _ENTITY_SELECTION_BAR_HEIGHT)
			{
				bias = (largestSize/_ENTITY_SELECTION_BAR_HEIGHT);
				v2NewSize = v2Size/bias;
			}
			const Vector2 v2Origin = pSprite->GetOrigin();

			pSprite->SetOrigin(Sprite::EO_CENTER);
			pSprite->DrawShaped(v2Pos, v2NewSize,
								gs2d::constant::WHITE, gs2d::constant::WHITE, gs2d::constant::WHITE, gs2d::constant::WHITE, 0.0f);
			//pSprite->SetRect(nFrame);

			pSprite->SetOrigin(v2Origin);
		}
		else if (!m_entityFiles[t]->light) // if it has no light source and no particles, draw the "invisible" symbol
		{
			bool hasParticles = false;
			for (std::size_t p = 0; p < m_entityFiles[t]->particleSystems.size(); p++)
			{
				if (m_entityFiles[t]->particleSystems[p]->nParticles > 0)
					hasParticles = true;
			}
			if (!hasParticles)
				m_invisible->Draw(v2Pos);
		}

		// if it has particle systems, show the P symbol
		for (std::size_t p = 0; p < m_entityFiles[t]->particleSystems.size(); p++)
		{
			if (m_entityFiles[t]->particleSystems[p]->nParticles > 0)
			{
				Vector2 v2SymbolPos = Vector2(
					m_entityFiles[t]->particleSystems[p]->startPoint.x, //-V807
					m_entityFiles[t]->particleSystems[p]->startPoint.y - m_entityFiles[t]->particleSystems[p]->startPoint.z) / bias;
				v2SymbolPos.x = Clamp(v2SymbolPos.x,-_ENTITY_SELECTION_BAR_HEIGHT/2, _ENTITY_SELECTION_BAR_HEIGHT/2);
				v2SymbolPos.y = Clamp(v2SymbolPos.y,-_ENTITY_SELECTION_BAR_HEIGHT/2, _ENTITY_SELECTION_BAR_HEIGHT/2);
				const Vector2 v2PPos = v2Pos+v2SymbolPos;
				m_pSprite->Draw(v2PPos, (m_entityFiles[t]->particleSystems[p]->color0));
			}
		}

		// if it has light, show the light symbol
		if (m_entityFiles[t]->light)
		{
			Vector2 v2SymbolPos = Vector2(m_entityFiles[t]->light->pos.x, m_entityFiles[t]->light->pos.y - m_entityFiles[t]->light->pos.z) / bias; //-V807
			v2SymbolPos.x = Clamp(v2SymbolPos.x,-_ENTITY_SELECTION_BAR_HEIGHT/2, _ENTITY_SELECTION_BAR_HEIGHT/2);
			v2SymbolPos.y = Clamp(v2SymbolPos.y,-_ENTITY_SELECTION_BAR_HEIGHT/2, _ENTITY_SELECTION_BAR_HEIGHT/2);
			const Vector2 v2LightPos = v2Pos + v2SymbolPos;
			m_lSprite->Draw(v2LightPos, Vector4(m_entityFiles[t]->light->color, 1.0f));
		}

		// highlight the 'mouseover' entity
		if (ETHGlobal::PointInRect(v2Cursor, v2Pos, Vector2(_ENTITY_SELECTION_BAR_HEIGHT,_ENTITY_SELECTION_BAR_HEIGHT)))
		{
			const Color dwColor(55,255,255,255);
			m_outline->SetOrigin(Sprite::EO_CENTER);
			m_outline->DrawShaped(v2Pos, Vector2(_ENTITY_SELECTION_BAR_HEIGHT,_ENTITY_SELECTION_BAR_HEIGHT),
								  dwColor, dwColor, dwColor, dwColor);

			// if the user clicked, select this one
			if (m_provider->GetInput()->GetKeyState(GSK_LMOUSE) == GSKS_HIT)
			{
				m_v3Pos.z = 0.0f;
				m_currentEntityIdx = static_cast<int>(t);
				SetPlacingMode();
			}

			textToDraw = m_entityFiles[t]->entityName;
			textToDraw += GS_L(" {");
			textToDraw += (m_entityFiles[t]->staticEntity) ? GS_L("static/") : GS_L("dynamic/");
			textToDraw += ETHGlobal::entityTypes[m_entityFiles[t]->type];
			textToDraw += GS_L("}");
		}

		// highlight the current entity
		if (m_currentEntityIdx == t)
		{
			const Color dwColor(127,255,255,255);
			m_outline->SetOrigin(Sprite::EO_CENTER);
			m_outline->DrawShaped(v2Pos, Vector2(_ENTITY_SELECTION_BAR_HEIGHT,_ENTITY_SELECTION_BAR_HEIGHT),
								  dwColor, dwColor, dwColor, dwColor);
		}

		// stop drawing if it reaches the end
		if (v2Pos.x > barWidth)
		{
			break;
		}

		offset += _ENTITY_SELECTION_BAR_HEIGHT;
	}

	if (textToDraw != GS_L(""))
	{
		const Vector2 v2TextPos(v2Cursor-Vector2(0, m_menuSize));
		const Color dwLeft = ARGB(155,0,0,0);
		const Color dwRight = ARGB(155,0,0,0);
		Vector2 boxSize = video->ComputeTextBoxSize(GS_L("Verdana14_shadow.fnt"), textToDraw.c_str());
		video->DrawRectangle(v2TextPos, boxSize, dwLeft, dwRight, dwLeft, dwRight);
		ShadowPrint(v2TextPos, textToDraw.c_str(), Color(255,255,255,255));
	}
	video->SetCameraPos(v2Cam);

	video->RoundUpPosition(roundUp);
	video->SetZBuffer(zBuffer);
}

void SceneEditor::LoopThroughEntityList()
{
	ETHEntityArray entityArray;
	m_pScene->GetBucketManager().GetVisibleEntities(entityArray);

	for (unsigned int t=0; t<entityArray.size(); t++)
	{
		const ETHRenderEntity *pEntity = static_cast<ETHRenderEntity*>(entityArray[t]);
		const Vector3 v3EntityPos(pEntity->GetPosition());

		// draw light symbol where we have invisible light sources
		if (pEntity->IsInvisible() && !pEntity->HasHalo())
		{
			if (pEntity->HasLightSource())
			{
				const Vector3 v3LightPos(pEntity->GetLightRelativePosition());
				m_lSprite->Draw(Vector2(v3EntityPos.x + v3LightPos.x, v3EntityPos.y + v3LightPos.y - v3EntityPos.z - v3LightPos.z),
					Vector4(pEntity->GetLightColor(), 1.0f));
			}
			else if (pEntity->IsCollidable())
			{
				pEntity->DrawCollisionBox(m_outline, gs2d::constant::WHITE, m_pScene->GetZAxisDirection());
			}
		}

		// draw invisible entity symbol
		if (pEntity->IsInvisible() && !pEntity->IsCollidable() && !pEntity->HasHalo())
		{
			VideoPtr video = m_provider->GetVideo();
			const float depth = video->GetSpriteDepth();
			video->SetSpriteDepth(0.0f);
			video->SetVertexShader(ShaderPtr());
			m_invisible->Draw(pEntity->GetPositionXY());
			video->SetSpriteDepth(depth);
		}

		// draw custom data
		if (pEntity->HasCustomData() && m_panel.GetButtonStatus(_S_SHOW_CUSTOM_DATA))
		{
			m_customDataEditor.ShowInScreenCustomData(pEntity, this);
		}
	}
}

void SceneEditor::UpdateEntities()
{
	ETHEntityArray entityArray;
	m_pScene->GetBucketManager().GetEntityArray(entityArray);
	const std::string currentProjectPath = GetCurrentProjectPath(true);
	const unsigned int size = entityArray.size();
	for (unsigned int t=0; t<size; t++)
	{
		const int idx = GetEntityByName(entityArray[t]->GetEntityName());
		if (idx >= 0)
		{
			entityArray[t]->Refresh(*m_entityFiles[idx].get());
		}
	}
}

int SceneEditor::GetEntityByName(const std::string &name)
{
	for (std::size_t t = 0; t < m_entityFiles.size(); t++)
	{
		if (m_entityFiles[t]->entityName == name)
		{
			return static_cast<int>(t);
		}
	}
	return -1;
}

void SceneEditor::CopySelectedToClipboard()
{
	if (!m_pSelected)
		return;

	const Vector2 v2Size = m_pSelected->GetCurrentSize();
	m_clipBoard = boost::shared_ptr<ETHRenderEntity>(new ETHRenderEntity(m_provider, *m_pSelected->GetProperties(), m_pSelected->GetAngle(), 1.0f));
	m_clipBoard->SetOrphanPosition(m_pSelected->GetPosition() + Vector3(v2Size.x, v2Size.y, 0));
	m_clipBoard->SetAngle(m_pSelected->GetAngle());
	m_clipBoard->SetFrame(m_pSelected->GetFrame());
	m_clipBoard->SetColor(m_pSelected->GetColorARGB());
}

void SceneEditor::PasteFromClipboard()
{
	if (!m_clipBoard)
		return;

	ETHRenderEntity* entity = new ETHRenderEntity(m_provider);
	*entity = *m_clipBoard.get();
	m_pScene->AddEntity(entity);
}

void SceneEditor::RenderScene()
{
	m_pScene->SetBorderBucketsDrawing(true);
	const float lastFrameElapsedTimeMS = ComputeElapsedTimeF(m_provider->GetVideo());
	m_pScene->Update(lastFrameElapsedTimeMS, m_backBuffer, 0);
	m_pScene->RenderScene(false, m_backBuffer);
}

bool SceneEditor::SaveAs()
{
	FILE_FORM_FILTER filter(GS_L("Ethanon scene files (*.esc)"), GS_L("esc"));
	char path[___OUTPUT_LENGTH], file[___OUTPUT_LENGTH];
	if (SaveForm(filter, std::string(GetCurrentProjectPath(true) + ETHDirectories::GetSceneDirectory()).c_str(), path, file))
	{
		std::string sOut;
		AddExtension(path, ".esc", sOut);
		ETHEntityCache entityCache;
		if (m_pScene->SaveToFile(sOut, entityCache))
		{
			SetCurrentFile(sOut.c_str());
			CreateFileUpdateDetector(GetCurrentFile(true));
		}
	}
	return true;
}

bool SceneEditor::Save()
{
	ETHEntityCache entityCache;
	m_pScene->SaveToFile(GetCurrentFile(true), entityCache);
	CreateFileUpdateDetector(GetCurrentFile(true));
	return true;
}

bool SceneEditor::Open()
{
	FILE_FORM_FILTER filter(GS_L("Ethanon scene files (*.esc)"), GS_L("esc"));
	char path[___OUTPUT_LENGTH], file[___OUTPUT_LENGTH];
	if (OpenForm(filter, std::string(GetCurrentProjectPath(true) + ETHDirectories::GetSceneDirectory()).c_str(), path, file))
	{
		OpenByFilename(path);
	}
	return true;
}

void SceneEditor::OpenByFilename(const std::string& filename)
{
	RebuildScene(filename);
	if (m_pScene->GetNumEntities())
	{
		m_sceneProps = *m_pScene->GetSceneProperties();
		SetCurrentFile(filename.c_str());
		ResetForms();
		if (m_panel.GetButtonStatus(_S_USE_STATIC_LIGHTMAPS))
		{
			m_updateLights = true;
		}

		m_provider->GetVideo()->SetCameraPos(Vector2(0,0));
		CreateFileUpdateDetector(filename);
	}
}

void SceneEditor::ShowLightmapMessage()
{
	ShadowPrint(
		m_provider->GetVideo()->GetScreenSizeF()/2-Vector2(400,0),
		GS_L("Generating lightmap. Please wait..."),
		GS_L("Verdana30_shadow.fnt"),
		gs2d::constant::WHITE);
}

void SceneEditor::CentralizeShortcut()
{
	if (m_provider->GetInput()->GetKeyState(EDITOR_GSK_CONTROL) == GSKS_DOWN && m_provider->GetInput()->GetKeyState(GSK_SPACE) == GSKS_HIT)
	{
		m_v2CamPos = Vector2(0,0);
	}
}

void SceneEditor::ReloadFiles()
{
	m_entityFiles.clear();

	if (GetCurrentProjectPath(false) == "")
	{
		return;
	}

	Platform::FileListing entityFiles;
	std::string currentProjectPath = GetCurrentProjectPath(true) + ETHDirectories::GetEntityDirectory();

	entityFiles.ListDirectoryFiles(currentProjectPath.c_str(), GS_L("ent"));
	const int nFiles = entityFiles.GetNumFiles();
	if (nFiles >= 1)
	{
		for (int t=0; t<nFiles; t++)
		{
			Platform::FileListing::FILE_NAME file;
			entityFiles.GetFileName(t, file);
			str_type::string full;
			file.GetFullFileName(full);

			std::cout << "(Scene editor)Entity found: " <<  file.file << " - Loading...";

			boost::shared_ptr<ETHEntityProperties> props(new ETHEntityProperties(full, m_provider->GetFileManager()));
			if (!props->hideFromSceneEditor)
			{
				m_entityFiles.push_back(props);
				std::cout << " loaded";
			}

			std::cout << std::endl;
		}
	}
	else
	{
		GS2D_CERR << GS_L("The editor couldn't find any *.ent files. Make sure there are valid files at the ")
			 << ETHDirectories::GetEntityDirectory() << GS_L(" folder\n");
	}
}
