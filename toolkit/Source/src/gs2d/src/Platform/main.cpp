/*

	This source file starts a desktop application that emulates
	a mobile environment

*/


#include "../gs2d.h"
#include "../gs2dinput.h"
#include "../gs2daudio.h"
#include "../gs2dframework.h"

#include "Platform.h"

#ifdef _DEBUG
 #ifdef WIN32
  #define _CRTDBG_MAP_ALLOC
  #include <stdlib.h>
  #include <crtdbg.h>
 #endif
#endif

using namespace gs2d;
using namespace gs2d::math;

int main()
{
	VideoPtr video;
	BaseApplicationPtr application = CreateBaseApplication();

	if ((video = CreateVideo(854, 480, L"GS2D", true, true, L"assets/data/")))
	{
		InputPtr input = CreateInput(0, true);
		AudioPtr audio = CreateAudio(0);

		application->Start(video, input, audio);

		Video::APP_STATUS status;
		while ((status = video->HandleEvents()) != Video::APP_QUIT)
		{
			if (status == Video::APP_SKIP)
				continue;

			input->Update();
			application->Update(Min(static_cast<unsigned long>(1000), ComputeElapsedTime(video)));
			application->RenderFrame();
		}
	}
	application->Destroy();	

	#ifdef _DEBUG
	 #ifdef WIN32
	  _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	  #endif
	#endif
	return 0;
}