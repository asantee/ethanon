#include <SDL/SDL.h>

#include <Video.h>
#include <Platform/macosx/MacOSXFileIOHub.h>
#include <Platform/StdAnsiFileManager.h>

#include "main.h"

int SDL_main (int argc, char **argv)
{
	Platform::FileManagerPtr fileManager(new Platform::StdAnsiFileManager(""));
	Platform::MacOSXFileIOHubPtr fileIOHub(new Platform::MacOSXFileIOHub(fileManager, "data/"));

	gs2d::VideoPtr video = gs2d::CreateVideo(1280, 720, "Hello GS2D!", true, true, fileIOHub, gs2d::Texture::PF_UNKNOWN, true);
	//gs2d:: texture = video->LoadTextureFromFile(fileIOHub->GetResourceDirectory() + "resources/tileset.png", 0x0);
	
	gs2d::SpritePtr tileset(video->CreateSprite(fileIOHub->GetResourceDirectory() + "resources/tileset.png"));
	tileset->SetupSpriteRects(2, 2);
	tileset->SetRect(2);

	video->SetBGColor(0xFF000000);

	gs2d::Video::APP_STATUS status;
	while ((status = video->HandleEvents()) != gs2d::Video::APP_QUIT)
	{
		if (status == gs2d::Video::APP_SKIP)
			continue;

		video->BeginSpriteScene();

		tileset->SetRect(3);
		tileset->Draw(gs2d::math::Vector2(600, 600));

		video->DrawRectangle(
			gs2d::math::Vector2(10,10),
			gs2d::math::Vector2(32,32),
			gs2d::constant::BLUE, gs2d::constant::GREEN,
			gs2d::constant::RED, gs2d::constant::YELLOW);

		video->DrawRectangle(
			gs2d::math::Vector2(800,10),
			gs2d::math::Vector2(64,256),
			gs2d::constant::YELLOW);

		tileset->SetRect(0);
		tileset->Draw(gs2d::math::Vector2(600, 200));

		video->DrawRectangle(
			gs2d::math::Vector2(20,42),
			gs2d::math::Vector2(64,64),
			0xFFFF00FF, gs2d::constant::WHITE,
			gs2d::constant::GREEN, gs2d::constant::BLACK);

		const gs2d::math::Vector2 screenSize(video->GetScreenSizeF());
		video->SetLineWidth(1.0f);
		video->DrawLine(gs2d::math::Vector2(0,0), video->GetScreenSizeF(), 0xFFFFFFFF, 0xFFFF00FF);
		video->DrawLine(gs2d::math::Vector2(100,0), gs2d::math::Vector2(200,100), 0xFFFFFFFF, 0xFFFF00FF);
		video->SetLineWidth(3);
		video->DrawLine(gs2d::math::Vector2(screenSize.x/2,0), gs2d::math::Vector2(screenSize.x/2,screenSize.y), 0xFFFFFFFF, 0xFFFF00FF);
		video->SetLineWidth(10);
		video->DrawLine(gs2d::math::Vector2(screenSize.x,0), gs2d::math::Vector2(0,screenSize.y), 0xFF0000FF, 0xFF00FF00);

		static float angle = 0.0f; angle += 1.0f;
		video->DrawRectangle(
			gs2d::math::Vector2(200,400),
			gs2d::math::Vector2(96,32),
			gs2d::constant::BLUE,
			angle);

		tileset->SetRect(1);
		tileset->Draw(gs2d::math::Vector2(200, 600));

		video->DrawRectangle(
			gs2d::math::Vector2(600,300),
			gs2d::math::Vector2(100,10),
			gs2d::constant::RED, gs2d::constant::YELLOW,
			gs2d::constant::BLUE, gs2d::constant::GREEN,
			45.0f);

		video->DrawRectangle(
			gs2d::math::Vector2(300,600),
			gs2d::math::Vector2(100,10),
			gs2d::constant::RED, gs2d::constant::YELLOW,
			gs2d::constant::BLUE, gs2d::constant::GREEN,
			200.0f);

		tileset->SetRect(2);
		tileset->Draw(screenSize * 0.5f);

		video->EndSpriteScene();
	}
	return 0;
}
