#if defined(MACOSX) || defined(LINUX) || defined (_WIN32)
#define GS2D_USE_SDL
#endif

#include <Resource/ETHDirectories.h>
#include <BaseApplication.h>

#include <Platform/Platform.h>
#include <Platform/StdFileManager.h>
#include <Platform/FileIOHub.h>
#include <ETHEngine.h>
#include <Platform/ETHAppEnmlFile.h>

#ifdef _WIN32
#if defined(_DEBUG) || defined(DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif
#define NOMINMAX
#include <windows.h>
#endif

using namespace gs2d;
using namespace gs2d::math;

void ProcParams(const int argc, char* argv[], bool& compileAndRun, bool& testing, bool& wait)
{
	compileAndRun = true;
	testing = false;

	wait = Application::GetPlatformName() == "macos" ? false : true;

	for (int t = 0; t < argc; t++)
	{
		const std::string arg = argv[t];
		if (arg == "-nowait")
		{
			wait = false;
		}
		if (arg == "-testing")
		{
			testing = true;
		}
		if (arg == "-norun")
		{
			compileAndRun = false;
		}
	}
}

std::string FindResourceDir(const int argc, char* argv[])
{
	for (int t = 0; t < argc; t++)
	{
		const std::string argStr = (argv[t]);
		if (argStr.substr(0, 4) == "dir=")
		{
			const std::vector<std::string> pieces = Platform::SplitString(argStr, "=");
			if (pieces.size() >= 2)
			{
				std::string dir = Platform::AddLastSlash(pieces[1]);
				return Platform::FixSlashes(dir);
			}
		}
	}
	return GetModuleDirectory();
}

int main(int argc, char* argv[])
{

	// start engine runtime
	bool compileAndRun, testing, wait;
	ProcParams(argc, argv, compileAndRun, testing, wait);
	ETHScriptWrapper::SetArgc(argc);
	ETHScriptWrapper::SetArgv(argv);

	Platform::FileManagerPtr fileManager(new Platform::StdFileManager());

	Platform::FileIOHubPtr fileIOHub = Platform::CreateFileIOHub(fileManager, ETHDirectories::GetBitmapFontDirectory());
	{
		const std::string resourceDirectory = FindResourceDir(argc, argv);
		if (!resourceDirectory.empty())
		{
			fileIOHub->SetResourceDirectory(resourceDirectory);
		}
		else
		{
			std::cout << "Could not find resources directory parameter. Exiting...";
			return 22; //EINVAL, invalid argument
		}
	}

	const std::string resourceDirectory = fileIOHub->GetResourceDirectory();

	const ETHAppEnmlFile app(
		resourceDirectory + ETH_APP_PROPERTIES_FILE,
		Platform::FileManagerPtr(new Platform::StdFileManager),
		Application::GetPlatformName(),
		fileIOHub->GetExternalStorageDirectory());

	const std::string bitmapFontPath = resourceDirectory + ETHDirectories::GetBitmapFontDirectory();

	bool aborted = false;
	{
		ETHEnginePtr application = ETHEnginePtr(new ETHEngine(testing, compileAndRun, true /*autoStartScriptEngine*/));
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
			InputPtr input = CreateInput(false);
			AudioPtr audio = CreateAudio(0);

			application->Start(video, input, audio);

			if (compileAndRun)
			{
				Video::APP_STATUS status;
				while ((status = video->HandleEvents()) != Video::APP_QUIT)
				{
					if (status == Video::APP_SKIP)
						continue;

					audio->Update();
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
		ETH_STREAM_DECL(ss) << std::endl << "The program executed an ilegal operation and was aborted";
		std::cerr << ss.str() << std::endl;
	}

	if (!compileAndRun && !aborted)
	{
		ETH_STREAM_DECL(ss) << std::endl << "Compilation successful: 0 errors";
		std::cerr << ss.str() << std::endl;
	}

	if (aborted && wait)
	{
		std::cerr << "Press any key to continue..." << "\n";
		std::cin.get();
	}

	return aborted;
}
