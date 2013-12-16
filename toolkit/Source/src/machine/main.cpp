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

#if defined(MACOSX) || defined(LINUX)
 #define GS2D_USE_SDL
 #include <SDL2/SDL.h>
#endif

#ifdef WIN32
 #if defined(_DEBUG) || defined(DEBUG)
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

	wait = Application::GetPlatformName() == GS_L("macosx") ? false : true;

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

str_type::string FindResourceDir(const int argc, gs2d::str_type::char_t* argv[])
{
	for (int t = 0; t < argc; t++)
	{
		const str_type::string argStr = (argv[t]);
		if (argStr.substr(0, 4) == GS_L("dir="))
		{
			const std::vector<str_type::string> pieces = Platform::SplitString(argStr, GS_L("="));
			if (pieces.size() >= 2)
			{
				str_type::string dir = Platform::AddLastSlash(pieces[1]);
				return Platform::FixSlashes(dir);
			}
		}
	}
	return GS_L("");
}

#if WIN32
 #include <windows.h>
 int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR lpCmdLine, int nCmdShow)
#else
 int main(int argc, char** argv)
#endif
{
	// convert args to multibyte char
	#ifdef WIN32
		GS2D_UNUSED_ARGUMENT(hInstance);
		GS2D_UNUSED_ARGUMENT(nCmdShow);
		int argc = 0;
		LPWSTR* wargv = CommandLineToArgvW(lpCmdLine, &argc);

		LPSTR* argv = new LPSTR [argc];

		for (int t = 0; t < argc; t++)
		{
			std::size_t convertCount = 0;
			const std::size_t bufferSize = 4096;
			argv[t] = new CHAR [bufferSize];
			wcstombs_s(&convertCount, argv[t], bufferSize, wargv[t], wcslen(wargv[t]) + 1);
		}
	#endif

	// start engine runtime
	bool compileAndRun, testing, wait;
	ProcParams(argc, argv, compileAndRun, testing, wait);
	ETHScriptWrapper::SetArgc(argc);
	ETHScriptWrapper::SetArgv(argv);

	Platform::FileManagerPtr fileManager(new Platform::StdFileManager());

	Platform::FileIOHubPtr fileIOHub = Platform::CreateFileIOHub(fileManager, ETHDirectories::GetBitmapFontDirectory());
	{
		const str_type::string resourceDirectory = FindResourceDir(argc, argv);
		if (!resourceDirectory.empty())
			fileIOHub->SetResourceDirectory(resourceDirectory);
	}
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
					if (application->Update(Min(1000.0f, ComputeElapsedTimeF(video))) == Application::APP_QUIT)
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

	#if defined(_DEBUG) || defined(DEBUG)
	 #ifdef WIN32
	  _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	  LocalFree(wargv);
	  for (int t = 0; t < argc; t++)
	  {
		  delete [] argv[t];
	  }
	  delete [] argv;
	  #endif
	#endif
	return 0;
}
