string str;

void main()
{
	LoadScene("empty", "", "onSceneUpdate");
	str +=        ("External storage path:        " + GetExternalStorageDirectory());
	str += "\n" + ("Global external storage path: " + GetGlobalExternalStorageDirectory());
	str += "\n" + ("Current language: " + GetSharedData("ethanon.system.language"));
	str += "\n" + (GetScreenSize().x + "x" + GetScreenSize().y);
}

void onSceneUpdate()
{
	DrawText(vector2(0), str, "Verdana20_shadow.fnt", 0xFFFFFFFF);

	ETHInput@ input = GetInputHandle();
	for (uint t = 0; t < input.GetMaxTouchCount(); t++)
	{
		if (input.GetTouchState(t) != KS_UP)
		{
			DrawText(input.GetTouchPos(t), "*     -> " + (t + 1), "Verdana20_shadow.fnt", 0xFFFFFFFF);
		}
	}

	updateJoysticks();
}

void updateJoysticks()
{
	string output;
	output += "FPS rate: " + GetFPSRate() + "\n";
	output += "Press SPACE or click/touch the screen to detect devices\n";

	ETHInput@ input = GetInputHandle();

	if (input.GetKeyState(K_SPACE) == KS_HIT || input.GetTouchState(0) == KS_HIT)
	{
		input.DetectJoysticks();
	}

	for (uint j = 0; j < input.GetMaxJoysticks(); j++)
	{
		if (input.GetJoystickStatus(j) == JS_DETECTED)
		{
			output += "Joystick #" + j + ": ";
			for (uint b = 1; b <= input.GetNumJoyButtons(j); b++)
			{
				const J_KEY button = uintToButtonKey(b);
				output += generateButtonStatusString(j, button);
			}

			output += generateButtonStatusString(j, JK_LEFT);
			output += generateButtonStatusString(j, JK_RIGHT);
			output += generateButtonStatusString(j, JK_UP);
			output += generateButtonStatusString(j, JK_DOWN);

			output += "\n";
		}
	}

	DrawText(vector2(0.0f, 128.0f), output, "Verdana20_shadow.fnt", 0xFFFFFFFF);
}

string generateButtonStatusString(const uint j, const J_KEY button)
{
	ETHInput@ input = GetInputHandle();
	const KEY_STATE buttonState = input.JoyButtonState(j, button);
	if (buttonState != KS_UP)
	{
		return "K_" + buttonKeyToString(button) + "-" + stateNameToString(buttonState) + ", ";
	}
	else
	{
		return "";
	}
}

string stateNameToString(const KEY_STATE state)
{
	switch (state)
	{
		case KS_HIT: return "HIT";
		case KS_RELEASE: return "RELEASE";
		case KS_UP: return "UP";
		case KS_DOWN: return "DOWN";
	}
	return "UNKNOWN";
}

J_KEY uintToButtonKey(const uint button)
{
	switch (button)
	{
		case 1: return JK_01;
		case 2: return JK_02;
		case 3: return JK_03;
		case 4: return JK_04;
		case 5: return JK_05;
		case 6: return JK_06;
		case 7: return JK_07;
		case 8: return JK_08;
		case 9: return JK_09;
		case 10: return JK_10;
		case 11: return JK_11;
		case 12: return JK_12;
		case 13: return JK_13;
		case 14: return JK_14;
		case 15: return JK_15;
		case 16: return JK_16;
	}
	return JK_NONE;
}

uint buttonKeyToUint(const uint button)
{
	switch (button)
	{
		case JK_01: return 1;
		case JK_02: return 2;
		case JK_03: return 3;
		case JK_04: return 4;
		case JK_05: return 5;
		case JK_06: return 6;
		case JK_07: return 7;
		case JK_08: return 8;
		case JK_09: return 9;
		case JK_10: return 10;
		case JK_11: return 11;
		case JK_12: return 12;
		case JK_13: return 13;
		case JK_14: return 14;
		case JK_15: return 15;
		case JK_16: return 16;
	}
	return 0;
}

string buttonKeyToString(const J_KEY button)
{
	switch (button)
	{
		case JK_UP: return "UP";
		case JK_DOWN: return "DOWN";
		case JK_LEFT: return "LEFT";
		case JK_RIGHT: return "RIGHT";
	}
	return ("" + buttonKeyToUint(button));
}
