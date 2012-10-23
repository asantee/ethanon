#include <SDL/SDL.h>

#include <Video/GLSDL/GLSDLVideo.h>
#include <Platform/macosx/MacOSXFileIOHub.h>
#include <Platform/StdAnsiFileManager.h>

#include "main.h"

void render()
{
	static float angle = 0.1f;
	glRotatef(angle, 0.0f, 0.0f, 1.0f);
    
    glBegin(GL_TRIANGLES);
    {
        glColor3f(1,0,0);
        glVertex2f(0,0);

        glColor3f(0,1,0);
        glVertex2f(.5,0);
        
        glColor3f(0,0,1);
        glVertex2f(.5,.5);
    }
    glEnd();
}

int SDL_main (int argc, char **argv)
{
	Platform::FileManagerPtr fileManager(new Platform::StdAnsiFileManager(""));
	Platform::MacOSXFileIOHubPtr fileIOHub(new Platform::MacOSXFileIOHub(fileManager, "data/"));

	gs2d::VideoPtr video(new gs2d::GLSDLVideo(fileIOHub, 640, 480, "Hello GS2D!", true, false, true));

	video->SetBGColor(0xFF000000);

	gs2d::Video::APP_STATUS status;
	while ((status = video->HandleEvents()) != gs2d::Video::APP_QUIT)
	{
		if (status == gs2d::Video::APP_SKIP)
			continue;

		video->BeginSpriteScene();
		render();
		video->EndSpriteScene();
	}
	return 0;
}
