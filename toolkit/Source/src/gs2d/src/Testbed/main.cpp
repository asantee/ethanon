#include "../Video.h"
#include "../Input.h"
#include "../Audio.h"

#include "../Platform/Platform.h"
#include "../Platform/StdFileManager.h"
#include "../Platform/windows/WindowsFileIOHub.h"

using namespace gs2d;
using namespace gs2d::math;
using namespace Platform;

const float TILE_WIDTH  = 64.0f;
const float TILE_HEIGHT = 64.0f;

void DrawTileMap(VideoPtr video, SpritePtr tileset, const unsigned int width, const unsigned int height, const int* tiles)
{
	video->SetAlphaMode(Video::AM_PIXEL);
	unsigned int i = 0;
	for (float y = 0.0f; y < TILE_HEIGHT * height; y += TILE_HEIGHT)
	{
		for (float x = 0.0f; x < TILE_WIDTH * width; x += TILE_WIDTH)
		{
			if (tiles[i] !=-1)
			{
				tileset->SetRect(tiles[i]);
				tileset->Draw(Vector2(x, y));
			}
			i++;
		}
	}
}

int main()
{
	VideoPtr video;

	FileManagerPtr fileManager(new StdFileManager());
	FileIOHubPtr fileIOHub(new WindowsFileIOHub(fileManager, L"fonts/", Platform::GetModuleDirectory() + L"resources/"));

	if ((video = CreateVideo(768, 512, L"Primeira  janela", true, true, fileIOHub)))
	{
		InputPtr input = CreateInput(0, true);
		AudioPtr audio = CreateAudio(0);

		SpritePtr tileset = video->CreateSprite(fileIOHub->GetResourceDirectory() + GS_L("tileset.png"));
		tileset->SetupSpriteRects(2, 2);

		const int tiles[] = {
			-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
			-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
			-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
			-1,-1,-1,-1,-1, 2,-1,-1,-1,-1,-1,-1,
			-1, 3,-1,-1, 2, 2,-1,-1, 3, 0, 0, 3,
			-1,-1,-1, 2, 2, 2,-1,-1,-1,-1,-1,-1,
			-1,-1, 2, 2, 2, 2,-1,-1,-1,-1,-1,-1,
			 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 };

		Video::APP_STATUS status;
		while ((status = video->HandleEvents()) != Video::APP_QUIT)
		{
			if (status == Video::APP_SKIP)
				continue;

			input->Update();

			video->BeginSpriteScene();

			DrawTileMap(video, tileset, 12, 8, tiles);
			video->DrawBitmapText(Vector2(0,0), L"Hello (Super Mario) world!", L"Verdana20_shadow.fnt", Color(0xFFFFFFFF));

			video->EndSpriteScene();
		}
	}
	return 0;
}
