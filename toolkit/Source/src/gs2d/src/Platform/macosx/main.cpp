#include <SDL/SDL.h>

#include <Video.h>
#include <Platform/macosx/MacOSXFileIOHub.h>
#include <Platform/StdAnsiFileManager.h>

#include "main.h"

int SDL_main (int argc, char **argv)
{
	Platform::FileManagerPtr fileManager(new Platform::StdAnsiFileManager(""));
	Platform::MacOSXFileIOHubPtr fileIOHub(new Platform::MacOSXFileIOHub(fileManager, "data/"));

	gs2d::VideoPtr video = gs2d::CreateVideo(1280, 800, "Hello GS2D!", true, true, fileIOHub, gs2d::Texture::PF_UNKNOWN, true);

	video->SetBGColor(0xFF000000);

	gs2d::Video::APP_STATUS status;
	while ((status = video->HandleEvents()) != gs2d::Video::APP_QUIT)
	{
		if (status == gs2d::Video::APP_SKIP)
			continue;

		video->BeginSpriteScene();

		video->DrawRectangle(
			gs2d::math::Vector2(10,10),
			gs2d::math::Vector2(32,32),
			gs2d::constant::BLUE, gs2d::constant::GREEN,
			gs2d::constant::RED, gs2d::constant::YELLOW);

		video->EndSpriteScene();
	}
	return 0;
}
