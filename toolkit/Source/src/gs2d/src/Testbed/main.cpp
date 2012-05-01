#include "../gs2d.h"
#include "../gs2dinput.h"
#include "../gs2daudio.h"
#include "../gs2dutil.h"

#include "../Platform/Platform.h"

using namespace gs2d;
using namespace gs2d::math;

const float TILE_WIDTH  = 64.0f;
const float TILE_HEIGHT = 64.0f;

void DrawToTarget(VideoPtr video, SpritePtr target, SpritePtr tileset, const unsigned int width, const unsigned int height, const int* tiles)
{
	video->SetRenderTarget(target);
	video->BeginTargetScene(0x00FFFFFF, true);
	video->SetAlphaMode(GSAM_PIXEL);

	unsigned int i = 0;
	for (float y = 0.0f; y < TILE_HEIGHT * height; y += 64.0f)
	{
		for (float x = 0.0f; x < TILE_WIDTH * width; x += 64.0f)
		{
			if (tiles[i] !=-1)
			{
				tileset->SetRect(tiles[i]);
				tileset->Draw(Vector2(x, y));
			}
			i++;
		}
	}

	video->EndTargetScene();
	video->SetRenderTarget(SpritePtr());
	target->SaveBitmap(L"map.png", GSBF_PNG);
}

int main()
{
	VideoPtr video;

	if ((video = CreateVideo(800, 600, L"Primeira janela", true, true, L"fonts/")))
	{
		InputPtr input = CreateInput(0, true);
		AudioPtr audio = CreateAudio(0);

		SpritePtr tileset = video->CreateSprite(GS_L("tileset.png"));
		tileset->SetupSpriteRects(4, 4);

		SpritePtr target = video->CreateRenderTarget(512, 512, GSTF_ARGB);

		const int tiles[] = { -1,2,6,6,6,6,6,3,-1,7,-1,-1,-1,-1,-1,7,-1,0,6,6,6,3,-1,4,-1,-1,-1,-1,-1,7,-1,-1,-1,2,6,6,6,1,-1,-1,-1,7,-1,-1,-1,-1,-1,-1,-1,7,-1,-1,-1,-1,-1,-1,6,1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 };
		DrawToTarget(video, target, tileset, 8, 10, tiles);

		Video::APP_STATUS status;
		while ((status = video->HandleEvents()) != Video::APP_QUIT)
		{
			if (status == Video::APP_SKIP)
				continue;

			input->Update();

			video->BeginSpriteScene(GS_BLUE);

			target->Draw(Vector2(0, 0));

			video->EndSpriteScene();
		}
	}
	return 0;
}
