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

#if defined(MACOSX) || defined(LINUX)
 #define GS2D_USE_SDL
 #include <SDL/SDL.h>
#endif

#include <Video.h>
#include <Audio.h>
#include <Input.h>
#include <Platform/Platform.h>
#include <Platform/StdFileManager.h>
#include <Platform/FileIOHub.h>

using namespace gs2d;
using namespace gs2d::math;

#ifdef GS2D_USE_SDL
 int SDL_main(int argc, char **argv)
#else
 int wmain(int argc, gs2d::str_type::char_t* argv[])
#endif
{
	Platform::FileManagerPtr fileManager(new Platform::StdFileManager());

	Platform::FileIOHubPtr fileIOHub = Platform::CreateFileIOHub(fileManager, GS_L("data/"));

	VideoPtr video;
	if ((video = CreateVideo(1024, 768, GS_L("temp"), true, true, fileIOHub, Texture::PF_UNKNOWN, false)))
	{
		InputPtr input = CreateInput(0, false);
		AudioPtr audio = CreateAudio(0);

		Video::APP_STATUS status;
		while ((status = video->HandleEvents()) != Video::APP_QUIT)
		{
			if (status == Video::APP_SKIP)
				continue;

			input->Update();
			video->BeginSpriteScene();
			video->EndSpriteScene();
		}
	}
	return 0;
}