#include <SDL/SDL.h>

#include <Video.h>
#include <Input.h>
#include <Audio.h>
#include <Platform/macosx/MacOSXFileIOHub.h>
#include <Platform/StdFileManager.h>
#include <Math/Randomizer.h>

#include "main.h"

int SDL_main (int argc, char **argv)
{
	Platform::FileManagerPtr fileManager(new Platform::StdFileManager());
	Platform::FileIOHubPtr fileIOHub = Platform::CreateFileIOHub(fileManager, "resources/fonts/");

	using namespace gs2d;
	using namespace gs2d::math;

	gs2d::VideoPtr video = gs2d::CreateVideo(1280, 720, "Hello GS2D!", true, true, fileIOHub, gs2d::Texture::PF_UNKNOWN, true);
	gs2d::InputPtr input = gs2d::CreateInput(0, true);
	gs2d::AudioPtr audio = gs2d::CreateAudio(0);
	{
		gs2d::SpritePtr tileset(video->CreateSprite(fileIOHub->GetResourceDirectory() + "resources/tileset.png"));
		tileset->SetupSpriteRects(2, 2);
		tileset->SetRect(2);

		gs2d::SpritePtr skull(video->CreateSprite(fileIOHub->GetResourceDirectory() + "resources/cool_skull.png"));
		gs2d::SpritePtr planets(video->CreateSprite(fileIOHub->GetResourceDirectory() + "resources/planets.png"));
		gs2d::SpritePtr black(video->CreateSprite(fileIOHub->GetResourceDirectory() + "resources/planets_black.png"));
		gs2d::SpritePtr road(video->CreateSprite(fileIOHub->GetResourceDirectory() + "resources/road.jpg"));
	
		gs2d::ShaderPtr ps = video->LoadShaderFromFile(fileIOHub->GetResourceDirectory() + "resources/shaders/minimalPixelShader.cg", Shader::SF_PIXEL);
	
		gs2d::Video::APP_STATUS status;
		while ((status = video->HandleEvents()) != gs2d::Video::APP_QUIT)
		{
			if (status == gs2d::Video::APP_SKIP)
				continue;

			input->Update();

            const Vector2 screenSize(video->GetScreenSizeF());

			video->BeginSpriteScene();

			video->SetPixelShader(ps);
			road->Draw(Vector2(0.0f, 0.0f));
			
			video->SetPixelShader(ShaderPtr());
			skull->Draw(screenSize * 0.5f);
			planets->Draw(screenSize * Vector2(0.5f, 0.0f));

			video->DrawRectangle(screenSize * 0.8f, Vector2(100,200), 0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFF00);
            
			video->EndSpriteScene();
		}
	}
	return 0;
}
