#include <SDL/SDL.h>

#include <Video.h>
#include <Input.h>
#include <Audio.h>
#include <Platform/macosx/MacOSXFileIOHub.h>
#include <Platform/StdAnsiFileManager.h>
#include <Math/Randomizer.h>

int SDL_main (int argc, char **argv)
{
	Platform::FileManagerPtr fileManager(new Platform::StdAnsiFileManager(""));
	Platform::MacOSXFileIOHubPtr fileIOHub(new Platform::MacOSXFileIOHub(fileManager, "resources/fonts/"));

	using namespace gs2d;
	using namespace gs2d::math;

	gs2d::VideoPtr video = gs2d::CreateVideo(1024, 720, "Ethanon machine", true, true, fileIOHub, gs2d::Texture::PF_UNKNOWN, true);
	gs2d::InputPtr input = gs2d::CreateInput(0, true);
	gs2d::AudioPtr audio = gs2d::CreateAudio(0);
	{
		gs2d::Video::APP_STATUS status;
		while ((status = video->HandleEvents()) != gs2d::Video::APP_QUIT)
		{
			if (status == gs2d::Video::APP_SKIP)
				continue;

			input->Update();
			video->BeginSpriteScene();

			video->EndSpriteScene();
		}
	}
	return 0;
}
