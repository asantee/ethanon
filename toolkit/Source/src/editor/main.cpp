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

#include "ProjectManager.h"
#include "EntityEditor.h"
#include "SceneEditor.h"
#include "ParticleFXEditor.h"
#include <Platform/Platform.h>
#include <unicode/utf8converter.h>
#include <Platform/StdFileManager.h>

#ifndef _DEBUG
#ifdef __WIN32__
#include <windows.h>
#endif
#endif

#ifdef _DEBUG
#define _SYNC_MODE false
#else
#define _SYNC_MODE true
#endif

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

enum EDITOR
{
	PROJECT = 0,
	ENTITY = 1,
	SCENE,
	PARTICLE,
	EEND,
};

static const wchar_t *g_wcsEditors[] = 
{
	L"Project manager",
	L"Entity editor",
	L"Scene editor",
	L"ParticleFX editor"
};

bool DoNextAppButton(int nextApp, SpritePtr pSprite, VideoPtr video, InputPtr input,
					 const float menuSize, boost::shared_ptr<EditorBase> pEditor)
{
	GS_ALPHA_MODE alphaMode = video->GetAlphaMode();
	video->SetAlphaMode(GSAM_PIXEL);

	// interpolator to blink the button
	static ETHFrameTimer interp;
	interp.Update(0, 1, 400);

	// draw button and check if clicked
	const Vector2 v2Screen = video->GetScreenSizeF();
	const Vector2 v2Pos = v2Screen-pSprite->GetBitmapSizeF();
	const Vector2 v2Mouse = input->GetCursorPositionF(video);
	const float textSize = 16.0f;
	GS_BYTE alpha = static_cast<GS_BYTE>(interp.GetInterpolationBias()*255.0f);
	if (interp.Get()%2)
		alpha = 255-alpha;
	const GS_COLOR dwColor(alpha,255,255,255);
	pSprite->Draw(v2Pos+video->GetCameraPos(), dwColor);
	if (v2Mouse.x > v2Pos.x && v2Mouse.y > v2Pos.y)
	{
		const Vector2 v2TextPos = v2Screen-Vector2(200.0f, textSize+3);
		switch (nextApp)
		{
		case SCENE:
			pEditor->ShadowPrint(v2TextPos, L"Go to Scene Editor", L"Verdana14_shadow.fnt", GS_WHITE);
			break;
		case ENTITY:
			pEditor->ShadowPrint(v2TextPos, L"Go to Entity Editor", L"Verdana14_shadow.fnt", GS_WHITE);
			break;
		case PARTICLE:
			pEditor->ShadowPrint(v2TextPos, L"Go to ParticleFX Editor", L"Verdana14_shadow.fnt", GS_WHITE);
			break;
		case PROJECT:
			pEditor->ShadowPrint(v2TextPos, _S_GOTO_PROJ, L"Verdana14_shadow.fnt", GS_WHITE);
			break;
		};
		if (input->GetLeftClickState() == GSKS_HIT || input->GetRightClickState() == GSKS_HIT)
			return true;
	}

	// draw tab background
	video->DrawRectangle(Vector2(0,0), Vector2(v2Screen.x, menuSize), GS_BLACK);
	video->SetAlphaMode(alphaMode);
	return false;
}

int main(const int argc, const char* argv[])
{
	#ifdef _DEBUG
		std::cout << "\nRunning debug mode" << std::endl << std::endl;
	#endif

	#ifdef _DEBUG
		const wstring resourcePath = Platform::GetCurrentDirectoryPath();
	#else
		const wstring resourcePath = Platform::GetModulePath();
	#endif

	// get default bitmap font path
	wstring defaultBitmapFontPath = resourcePath;
	defaultBitmapFontPath += ETHDirectories::GetBitmapFontPath();

	VideoPtr video;
	InputPtr input;
	AudioPtr audio;

	const ETH_WINDOW_ENML_FILE app(resourcePath + L"editor.enml", Platform::FileManagerPtr(new Platform::StdFileManager));
	Vector2i v2Backbuffer(app.width, app.height);

	if ((video = CreateVideo(v2Backbuffer.x, v2Backbuffer.y, app.title, app.windowed, app.vsync,
		defaultBitmapFontPath, GSPF_UNKNOWN, true)))
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
			ETHShaderManagerPtr(new ETHShaderManager(video, resourcePath + ETHDirectories::GetShaderPath(), true)),
			GS_L(""),
			video, audio, input));

		// instantiate and load every editor
		vector<boost::shared_ptr<EditorBase>> editor(nEditors);
		editor[PROJECT] = boost::shared_ptr<EditorBase>(new ProjectManager(provider));
		editor[ENTITY] = boost::shared_ptr<EditorBase>(new EntityEditor(provider));
		editor[SCENE] = boost::shared_ptr<EditorBase>(new SceneEditor(provider));
		editor[PARTICLE] = boost::shared_ptr<EditorBase>(new ParticleEditor(provider));

		// create all editors
		for (unsigned int t=0; t<nEditors; t++)
		{
			editor[t]->LoadEditor();
		}

		// load the arrow sprite used by every editor
		const wstring nextAppButtonPath = resourcePath + L"data/nextapp.png";
		SpritePtr nextAppButton = video->CreateSprite(nextAppButtonPath);

		ETH_STARTUP_RESOURCES_ENML_FILE startup(resourcePath + wstring(L"/") + L"editor.enml", Platform::FileManagerPtr(new Platform::StdFileManager));

		// if the user tried the "open with..." feature, open that project
		if (argc >= 2)
		{
			editor[PROJECT]->SetCurrentProject(argv[1]);
			editor[PROJECT]->SetCurrentFile(argv[1]);
		}
		else if ( !startup.emtprojFilename.empty() )
		{
			// default project opening
			editor[PROJECT]->SetCurrentProject(utf8::c(startup.emtprojFilename).c_str());
			editor[PROJECT]->SetCurrentFile(utf8::c(startup.emtprojFilename).c_str());

			// set resource root for all modules
			for (unsigned int t=1; t<nEditors; t++)
			{
				editor[t]->SetCurrentProject(editor[PROJECT]->GetCurrentProject().c_str());
			}

			// default scene openning
			if ( !startup.escFilename.empty() )
			{
				SceneEditor * sceneEditor = (SceneEditor *)(editor[SCENE].get());
				sceneEditor->OpenByFilename(utf8::c(startup.escFilename).c_str());
			}
		}

		string lastProjectName = editor[PROJECT]->GetCurrentProject();

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
				for (unsigned int t=1; t<nEditors; t++)
				{
					editor[t]->Clear();
				}
				video->SetCameraPos(Vector2(0,0));
			}
			lastProjectName = editor[PROJECT]->GetCurrentProject();

			// updates the current project path so the editors can gather files from that sources
			for (unsigned int t=1; t<nEditors; t++)
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
				arrowClicked = DoNextAppButton(nextApp, nextAppButton, video, input,
					editor[PARTICLE]->GetMenuSize()*2, editor[0]);
			}
			else
			{
				current = PROJECT;
			}

			// if any editor has requested to open the project manager...
			{
				bool requested = false;
				for (unsigned int t=1; t<nEditors; t++)
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
				for (int t=3; t>=0; t--)
				{
					GS_COLOR dwColor = editor[PROJECT]->GetBGColor();
					if (t != current)
						dwColor = GS_COLOR(127, dwColor.r, dwColor.g, dwColor.b);
					if (editor[PROJECT]->DrawTab(video, input, Vector2(v2ScreenF.x-tabWidth*static_cast<float>(t+1)-32.0f,0), tabWidth, g_wcsEditors[t], dwColor))
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

				for (unsigned int t=1; t<nEditors; t++)
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

			// show fps rate
			Vector2 v2FPSPos;
			v2FPSPos.x = video->GetScreenSizeF().x-40.0f;
			v2FPSPos.y = video->GetScreenSizeF().y-24.0f-32.0f;
			wstringstream ss;
			ss << video->GetFPSRate();
			editor[0]->ShadowPrint(v2FPSPos, ss.str().c_str(), L"Verdana24_shadow.fnt", GS_WHITE);

			video->EndSpriteScene();

			// generate lightmaps if needed. It must be outside the drawing process
			editor[SCENE]->UpdateInternalData();
		}
	}
#ifdef _DEBUG
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
	return 0;
}
