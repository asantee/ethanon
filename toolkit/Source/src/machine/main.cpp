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

#ifdef MACOSX
 #define GS2D_USE_SDL
 #include <SDL/SDL.h>
#endif

#ifdef WIN32
 #ifdef _DEBUG
  #define _CRTDBG_MAP_ALLOC
  #include <stdlib.h>
  #include <crtdbg.h>
 #endif
#endif

#include "../engine/ETHTypes.h"
#include "../engine/Resource/ETHDirectories.h"
#include <BaseApplication.h>

#include <Platform/Platform.h>
#include <Platform/StdFileManager.h>
#include <Platform/FileIOHub.h>
#include "../engine/ETHEngine.h"
#include "../engine/Platform/ETHAppEnmlFile.h"

using namespace gs2d;
using namespace gs2d::math;

void ProcParams(const int argc, gs2d::str_type::char_t* argv[], bool& compileAndRun, bool& testing, bool& wait)
{
	compileAndRun = true;
	testing = false;

	wait = Application::GetPlatformName() == "macosx" ? false : true;

	for (int t = 0; t < argc; t++)
	{
		const gs2d::str_type::string arg = argv[t];
		if (arg == GS_L("-nowait"))
		{
			wait = false;
		}
		if (arg == GS_L("-testing"))
		{
			testing = true;
		}
		if (arg == GS_L("-norun"))
		{
			compileAndRun = false;
		}
	}
}

#ifdef GS2D_USE_SDL
 int SDL_main(int argc, char **argv)
#else
 int wmain(int argc, gs2d::str_type::char_t* argv[])
#endif
{
	bool compileAndRun, testing, wait;
	ProcParams(argc, argv, compileAndRun, testing, wait);
	ETHScriptWrapper::SetArgc(argc);
	ETHScriptWrapper::SetArgv(argv);

	Platform::FileManagerPtr fileManager(new Platform::StdFileManager());

	Platform::FileIOHubPtr fileIOHub = Platform::CreateFileIOHub(fileManager, ETHDirectories::GetBitmapFontDirectory());
	const str_type::string resourceDirectory = fileIOHub->GetResourceDirectory(); 

	const ETHAppEnmlFile app(resourceDirectory + ETH_APP_PROPERTIES_FILE, Platform::FileManagerPtr(new Platform::StdFileManager), Application::GetPlatformName());
	const str_type::string bitmapFontPath = resourceDirectory + ETHDirectories::GetBitmapFontDirectory();

	bool aborted;
	{
		ETHEnginePtr application = ETHEnginePtr(new ETHEngine(testing, compileAndRun));
		application->SetHighEndDevice(true);

		VideoPtr video;
		if ((video = CreateVideo(
			app.GetWidth(),
			app.GetHeight(),
			app.GetTitle(),
			app.IsWindowed(),
			app.IsVsyncEnabled(),
			fileIOHub,
			Texture::PF_UNKNOWN,
			false)))
		{
			InputPtr input = CreateInput(0, false);
			AudioPtr audio = CreateAudio(0);

			application->Start(video, input, audio);

			if (compileAndRun)
			{
				Video::APP_STATUS status;
				while ((status = video->HandleEvents()) != Video::APP_QUIT)
				{
					if (status == Video::APP_SKIP)
						continue;

					input->Update();
					if (application->Update(Min(static_cast<unsigned long>(1000), ComputeElapsedTime(video))) == Application::APP_QUIT)
					{
						break;
					}
					application->RenderFrame();
				}
			}
		}
		application->Destroy();	
		aborted = application->Aborted();
	}

	if (aborted)
	{
		ETH_STREAM_DECL(ss) << std::endl << GS_L("The program executed an ilegal operation and was aborted");
		GS2D_CERR << ss.str() << std::endl;
	}

	if (!compileAndRun && !aborted)
	{
		ETH_STREAM_DECL(ss) << std::endl << GS_L("Compilation successful: 0 errors");
		GS2D_CERR << ss.str() << std::endl;
	}

	if (aborted && wait)
	{
		GS2D_COUT << GS_L("Press any key to continue...") << GS_L("\n");
		std::cin.get();
	}

	#ifdef _DEBUG
	 #ifdef WIN32
	  _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	  #endif
	#endif
	return 0;
}