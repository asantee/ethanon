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

#ifdef _DEBUG
 #ifdef WIN32
  #define _CRTDBG_MAP_ALLOC
  #include <stdlib.h>
  #include <crtdbg.h>
 #endif
#endif

#include "../engine/ETHCommon.h"
#include <gs2dframework.h>

#include <Platform/Platform.h>
#include <Platform/StdFileManager.h>
#include "../engine/ETHEngine.h"

#include <conio.h>

using namespace gs2d;
using namespace gs2d::math;

void ProcParams(int argc, wchar_t* argv[], bool& compileAndRun, bool& testing, bool& wait)
{
	compileAndRun = true;
	testing = false;
	wait = true;
	for (int t=0; t<argc; t++)
	{
		if (wcscmp(argv[t], L"-nowait") == 0)
		{
			wait = false;
		}
		if (wcscmp(argv[t], L"-testing") == 0)
		{
			testing = true;
		}
		if (wcscmp(argv[t], L"-norun") == 0)
		{
			compileAndRun = false;
		}
	}
}

int wmain(int argc, wchar_t* argv[])
{
	#ifdef _DEBUG
		std::cout << "\nRunning debug mode" << std::endl << std::endl;
		const str_type::string resourcePath = Platform::GetCurrentDirectoryPath();
	#else
		const std::wstring resourcePath = Platform::GetModulePath();
	#endif

	bool compileAndRun, testing, wait;
	ProcParams(argc, argv, compileAndRun, testing, wait);
	// compileAndRun = false;

	const ETH_WINDOW_ENML_FILE app(resourcePath + ETH_APP_PROPERTIES_FILE, Platform::FileManagerPtr(new Platform::StdFileManager));
	const str_type::string bitmapFontPath = resourcePath + ETHDirectories::GetBitmapFontPath();

	bool aborted;
	{
		ETHEnginePtr application = ETHEnginePtr(new ETHEngine(testing, compileAndRun, resourcePath));
		application->SetHighEndDevice(true); // the PC will always be considered as a high-end device
		VideoPtr video;
		if ((video = CreateVideo(app.width, app.height, app.title.c_str(), app.windowed, app.vsync, bitmapFontPath.c_str(), GSPF_UNKNOWN, false)))
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
		//logger->Log(ss.str(), Platform::Logger::ERROR);
		std::wcerr << ss.str() << std::endl;
	}

	if (!compileAndRun && !aborted)
	{
		ETH_STREAM_DECL(ss) << std::endl << GS_L("Compilation successful: 0 errors");
		//logger->Log(ss.str(), Platform::Logger::INFO);
		std::wcerr << ss.str() << std::endl;
	}

	if (aborted && wait)
	{
		std::wcout << GS_L("Press any key to continue...") << std::endl;
		while(!_getch());
	}

	#ifdef _DEBUG
	 #ifdef WIN32
	  _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	  #endif
	#endif
	return 0;
}