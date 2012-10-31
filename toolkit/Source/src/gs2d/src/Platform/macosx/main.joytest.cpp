#include <SDL/SDL.h>

#include <Video.h>
#include <Input.h>
#include <Platform/macosx/MacOSXFileIOHub.h>
#include <Platform/StdAnsiFileManager.h>
#include <Math/Randomizer.h>

#include "main.h"

int SDL_main (int argc, char **argv)
{
	Platform::FileManagerPtr fileManager(new Platform::StdAnsiFileManager(""));
	Platform::MacOSXFileIOHubPtr fileIOHub(new Platform::MacOSXFileIOHub(fileManager, "resources/fonts/"));

	using namespace gs2d;
	using namespace gs2d::math;

	gs2d::VideoPtr video = gs2d::CreateVideo(1280, 720, "Hello GS2D!", true, true, fileIOHub, gs2d::Texture::PF_UNKNOWN, true);
	gs2d::InputPtr input = gs2d::CreateInput(0, true);
	{
		gs2d::SpritePtr skull(video->CreateSprite(fileIOHub->GetResourceDirectory() + "resources/cool_skull.png"));
		video->SetBGColor(0xFF003366);

		Vector2 v2BallPos(video->GetScreenSizeF() / 2.0f);
		unsigned int gsID = 0;
		float fZAngle = 0.0f;

		gs2d::Video::APP_STATUS status;
		str_type::stringstream inputStr;
		while ((status = video->HandleEvents()) != gs2d::Video::APP_QUIT)
		{
			if (status == gs2d::Video::APP_SKIP)
				continue;

			input->Update();

			video->BeginSpriteScene();

			if (input->GetKeyState(GSK_SPACE) == GSKS_HIT)
			{
				v2BallPos = (video->GetScreenSizeF() / 2.0f);
				input->DetectJoysticks();
			}

			if (input->GetKeyState(GSK_1) == GSKS_HIT && input->GetJoystickStatus(0) == GSJS_DETECTED)
				gsID = 0;
			if (input->GetKeyState(GSK_2) == GSKS_HIT && input->GetJoystickStatus(1) == GSJS_DETECTED)
				gsID = 1;
			if (input->GetKeyState(GSK_3) == GSKS_HIT && input->GetJoystickStatus(2) == GSJS_DETECTED)
				gsID = 2;
			if (input->GetKeyState(GSK_4) == GSKS_HIT && input->GetJoystickStatus(3) == GSJS_DETECTED)
				gsID = 3;

			v2BallPos = v2BallPos + (input->GetJoystickXY(gsID) * 3.0f);
			//fZAngle += input->GetJoystickRudder(gsID) + input->GetJoystickZ(gsID) * 3.0f;

			skull->Draw(v2BallPos, 0xFFFFFFFF, fZAngle);

			str_type::stringstream text; text << "Pressed buttons: ";
			bool bNone = true;
			for (unsigned int t = 0; t < GSB_NUM_BUTTONS; t++)
			{
				if (input->GetJoystickButtonState(gsID, (GS_JOYSTICK_BUTTON)t) == GSKS_DOWN)
				{
					if (!bNone)
						text << (", ");
					bNone = false;
					text << "GSB_" << (t + 1);
				}
			}
			if (bNone)
				text << ("<none>");

			char szNum[1024];
			sprintf(szNum, "\nx = %f\ny = %f\nz = %f\nu = %f\nv = %f\nrudder = %f\n",
					input->GetJoystickXY(gsID).x, input->GetJoystickXY(gsID).y, input->GetJoystickZ(gsID),
					input->GetJoystickUV(gsID).x, input->GetJoystickUV(gsID).y, input->GetJoystickRudder(gsID));
			text << (szNum);

			const GS_JOYSTICK_BUTTON button = input->GetFirstButtonDown(gsID);
			if (button != GSB_NONE)
			{
				text << "\nFirst button detected: " << ((int)button+1) << "\n";
			}

			text << "Maximum number of joysticks supported: ";
			text << input->GetMaxJoysticks() << "\n";
			text << "Detected joysticks:";
			bool bJoystick = false;
			for (unsigned int t = 0; t < input->GetMaxJoysticks(); t++)
			{
				if (input->GetJoystickStatus(t) == GSJS_DETECTED)
				{
					bJoystick = true;
					if (gsID == t)
						text << " JOY" << (t+1) << " -";
					else
						text << " joy" << (t+1) << " -";
				}
			}

			if (bJoystick)
				video->DrawBitmapText(Vector2(0.0f,0.0f), text.str(), "Verdana20_shadow.fnt", 0xFFFFFFFF);
			else
				video->DrawBitmapText(Vector2(0.0f,0.0f),
								  "No joystick detected\nPress SPACE to reset",
								  "Verdana20_shadow.fnt", 0xFFFFFFFF);

			video->EndSpriteScene();
		}
	}
	return 0;
}
