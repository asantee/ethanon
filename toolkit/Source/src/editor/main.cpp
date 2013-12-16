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

#include "ProjectManager.h"
#include "EntityEditor.h"
#include "SceneEditor.h"
#include "ParticleFXEditor.h"

#include <Platform/Platform.h>
#include <Platform/FileIOHub.h>
#include <Platform/StdFileManager.h>
#include <Platform/MainSharedHeader.h>

#include "../engine/Platform/ETHAppEnmlFile.h"
#include "../engine/Resource/ETHDirectories.h"

#if defined(MACOSX) || defined(LINUX)
 #define GS2D_USE_SDL
 #include <SDL2/SDL.h>
#endif

#ifndef _DEBUG
 #ifdef __WIN32__
  #include <windows.h>
  #define _CRTDBG_MAP_ALLOC
  #include <stdlib.h>
  #include <crtdbg.h>
 #endif
#endif

#if defined(_DEBUG) || defined(DEBUG)
 #define _SYNC_MODE false
#else
 #define _SYNC_MODE true
#endif

enum EDITOR
{
	PROJECT = 0,
	ENTITY = 1,
	SCENE,
	PARTICLE,
	EEND,
};

static const str_type::char_t *g_editors[] =
{
	GS_L("Project manager"),
	GS_L("Entity editor"),
	GS_L("Scene editor"),
	GS_L("ParticleFX editor")
};

static std::string g_externalProjectOpenRequest;

extern "C" {
	void RequestFileOpen(const char* str)
	{
		g_externalProjectOpenRequest = str;
	}
}

class EditorFileOpener : public gs2d::Application::FileOpenListener
{
public:
	bool OnFileOpen(const str_type::string& fullFilePath)
	{
		g_externalProjectOpenRequest = fullFilePath;
		return true;
	}
};

bool DoNextAppButton(int nextApp, SpritePtr pSprite, VideoPtr video, InputPtr input,
					 const float menuSize, boost::shared_ptr<EditorBase> pEditor)
{
	Video::ALPHA_MODE alphaMode = video->GetAlphaMode();
	video->SetAlphaMode(Video::AM_PIXEL);

	// interpolator to blink the button
	static ETHFrameTimer interp;
	interp.Update(0, 1, 400);

	// draw button and check if clicked
	const Vector2 v2Screen = video->GetScreenSizeF();
	const Vector2 v2Pos = v2Screen-pSprite->GetBitmapSizeF();
	const Vector2 v2Mouse = input->GetCursorPositionF(video);
	const float textSize = 16.0f;
	GS_BYTE alpha = static_cast<GS_BYTE>(interp.GetInterpolationBias() * 255.0f);

	if (interp.Get()%2)
		alpha = 255-alpha;

	const Color dwColor(alpha,255,255,255);
	pSprite->Draw(v2Pos+video->GetCameraPos(), dwColor);
	if (v2Mouse.x > v2Pos.x && v2Mouse.y > v2Pos.y)
	{
		str_type::string message;
		switch (nextApp)
		{
		case SCENE:
			message = GS_L("Go to Scene Editor");
			break;
		case ENTITY:
			message = GS_L("Go to Entity Editor");
			break;
		case PARTICLE:
			message = GS_L("Go to ParticleFX Editor");
			break;
		case PROJECT:
			message = _S_GOTO_PROJ;
			break;
		};
		const Vector2 v2TextPos = v2Screen-Vector2(200.0f, textSize + 3);
		pEditor->ShadowPrint(v2TextPos, message.c_str(), GS_L("Verdana14_shadow.fnt"), gs2d::constant::WHITE);
		if (input->GetLeftClickState() == GSKS_HIT || input->GetRightClickState() == GSKS_HIT)
			return true;
	}

	// draw tab background
	video->DrawRectangle(Vector2(0,0), Vector2(v2Screen.x, menuSize), gs2d::constant::BLACK);
	video->SetAlphaMode(alphaMode);
	return false;
}

std::string FindStartupProject(const int argc, char **argv, const Platform::FileManagerPtr& fileManager, const Platform::Logger& logger)
{
	for (int t = 0; t < argc; t++)
	{
		logger.Log(argv[t], Platform::Logger::INFO);
		if (Platform::IsExtensionRight(argv[t], GS_L(".ethproj")) && ETHGlobal::FileExists(argv[t], fileManager))
		{
			return argv[t];
		}
	}
	return ("");
}

int main(int argc, char **argv)
{
	VideoPtr video;
	InputPtr input;
	AudioPtr audio;

	Platform::FileManagerPtr fileManager = Platform::FileManagerPtr(new Platform::StdFileManager);

	const str_type::string emptyString = GS_L("");
	Platform::FileIOHubPtr fileIOHub = Platform::CreateFileIOHub(fileManager, ETHDirectories::GetBitmapFontDirectory(), &emptyString);

	fileIOHub->SeekFontFromProgramPath(true);

	const ETHAppEnmlFile app(fileIOHub->GetProgramDirectory() + GS_L("editor.enml"), fileManager, gs2d::Application::GetPlatformName());
	Vector2i v2Backbuffer(app.GetWidth(), app.GetHeight());

	if ((video = CreateVideo(v2Backbuffer.x, v2Backbuffer.y, app.GetTitle(), app.IsWindowed(), app.IsVsyncEnabled(),
							 fileIOHub, Texture::PF_UNKNOWN, true)))
	{
		#ifndef _DEBUG
		#ifdef __WIN32__
			HWND hWnd = GetConsoleWindow();
			ShowWindow( hWnd, SW_HIDE );
		#endif
		#endif

		input = CreateInput(0, false);
		audio = CreateAudio(0);

		int current = PROJECT;
		const unsigned int nEditors = 4;

		ETHResourceProviderPtr provider = ETHResourceProviderPtr(new ETHResourceProvider(
			ETHGraphicResourceManagerPtr(new ETHGraphicResourceManager(ETHSpriteDensityManager())),
			ETHAudioResourceManagerPtr(new ETHAudioResourceManager()),
			ETHShaderManagerPtr(new ETHShaderManager(video, fileIOHub->GetProgramDirectory() + ETHDirectories::GetShaderDirectory(), true)),
			video, audio, input, fileIOHub, true));

		video->SetFileOpenListener(gs2d::Application::FileOpenListenerPtr(new EditorFileOpener()));

		// instantiate and load every editor
		std::vector<boost::shared_ptr<EditorBase> > editor(nEditors);
		editor[PROJECT] = boost::shared_ptr<EditorBase>(new ProjectManager(provider));
		editor[ENTITY] = boost::shared_ptr<EditorBase>(new EntityEditor(provider));
		editor[SCENE] = boost::shared_ptr<EditorBase>(new SceneEditor(provider));
		editor[PARTICLE] = boost::shared_ptr<EditorBase>(new ParticleEditor(provider));

		// create all editors
		for (unsigned int t = 0; t < nEditors; t++)
		{
			editor[t]->LoadEditor();
		}

		// load the arrow sprite used by every editor
		const str_type::string nextAppButtonPath = fileIOHub->GetProgramDirectory() + GS_L("data/nextapp.png");
		SpritePtr nextAppButton = video->CreateSprite(nextAppButtonPath);

		//ETH_STARTUP_RESOURCES_ENML_FILE startup(fileIOHub->GetProgramDirectory() + GS_L("editor.enml"), Platform::FileManagerPtr(new Platform::StdFileManager));

		// if the user tried the "open with..." feature on windows, open that project
		const std::string projectFile = FindStartupProject(argc, argv, fileManager, *provider->GetLogger());
		if (!projectFile.empty())
		{
			editor[PROJECT]->SetCurrentProject(projectFile.c_str());
			editor[PROJECT]->SetCurrentFile(projectFile.c_str());
		}

		std::string lastProjectName = editor[PROJECT]->GetCurrentProject();

		while (1)
		{
			Application::APP_STATUS status = video->HandleEvents();
			if (status == Application::APP_QUIT)
				break;
			else if (status == Application::APP_SKIP)
				continue;

			input->Update();
			video->BeginSpriteScene();

			const Vector2 v2ScreenF = video->GetScreenSizeF();

			// execute the current editor
			if (current >= 0)
				editor[current]->DoEditor(nextAppButton);

			// if the project has changed, clear all
			if (lastProjectName != editor[PROJECT]->GetCurrentProject())
			{
				for (unsigned int t = 1; t < nEditors; t++)
				{
					editor[t]->Clear();
				}
				video->SetCameraPos(Vector2(0,0));
			}
			lastProjectName = editor[PROJECT]->GetCurrentProject();

			// updates the current project path so the editors can gather files from that sources
			for (unsigned int t = 1; t < nEditors; t++)
			{
				editor[t]->SetCurrentProject(editor[PROJECT]->GetCurrentProject().c_str());
			}

			int nextApp = current+1;
			if (nextApp >= EEND)
				nextApp = PROJECT;
			// if a project is already loaded, skip the project manager here
			if (nextApp == PROJECT && editor[PROJECT]->GetCurrentProject() != "")
			{
				nextApp++;
			}

			// if we don't have a project yet, don't let it go to the next editors
			bool arrowClicked = false;
			bool tabClicked = false;

			// if there's a project, allow editor swap
			if (editor[PROJECT]->GetCurrentProject() != "")
			{
				arrowClicked = DoNextAppButton(nextApp, nextAppButton, video, input, editor[PARTICLE]->GetMenuSize() * 2, editor[0]);
			}
			else
			{
				current = PROJECT;
			}

			// if any editor has requested to open the project manager...
			{
				bool requested = false;
				for (unsigned int t = 1; t < nEditors; t++)
				{
					if (editor[t]->ProjectManagerRequested())
					{
						requested = true;
					}
				}
				if (requested)
				{
					nextApp = PROJECT;
					arrowClicked = true;
				}
			}

			// draw tabs
			if (editor[PROJECT]->GetCurrentProject() != "")
			{
				const float tabWidth = 160.0f;
				for (int t = 3; t >= 0; t--)
				{
					Color dwColor = editor[PROJECT]->GetBGColor();

					if (t != current)
						dwColor = Color(127, dwColor.r, dwColor.g, dwColor.b);

					if (editor[PROJECT]->DrawTab(
						video,
						input,
						Vector2(v2ScreenF.x - tabWidth * static_cast<float>(t + 1) - 32.0f, 0),
						tabWidth,
						g_editors[t],
						dwColor))
					{
						if (current != t)
						{
							tabClicked = true;
							current = t;
						}
					}
				}
			}

			// if all conditions are OK, perform editor swap
			if ((arrowClicked || tabClicked) && editor[PROJECT]->GetCurrentProject() != "")
			{
				if (!tabClicked)
					current = nextApp;

				for (unsigned int t = 1; t < nEditors; t++)
				{
					if (current != t)
					{
						editor[t]->StopAllSoundFXs();
					}
				}
				if (current == SCENE)
				{
					editor[SCENE]->ReloadFiles();
				}
			}

			// check and execute external project load requests
			if (!g_externalProjectOpenRequest.empty())
			{
				boost::shared_ptr<ProjectManager> projectManager = boost::dynamic_pointer_cast<ProjectManager>(editor[PROJECT]);
				projectManager->RequestProjectOpen(g_externalProjectOpenRequest);
				current = PROJECT;
				g_externalProjectOpenRequest.clear();
			}

			// show fps rate
			Vector2 v2FPSPos;
			v2FPSPos.x = video->GetScreenSizeF().x - 40.0f;
			v2FPSPos.y = video->GetScreenSizeF().y - 24.0f - 32.0f;
			str_type::stringstream ss;
			ss << video->GetFPSRate();
			editor[0]->ShadowPrint(v2FPSPos, ss.str().c_str(), GS_L("Verdana24_shadow.fnt"), gs2d::constant::WHITE);

			video->EndSpriteScene();

			// generate lightmaps if needed. It must be outside the drawing process
			editor[SCENE]->UpdateInternalData();
		}
	}
	#if defined(_DEBUG) && defined(__WIN32__)
	 _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif
	return 0;
}
