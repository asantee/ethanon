void main()
{
	LoadScene("empty", "", "onSceneUpdate");
	print("External storage path:        " + GetExternalStoragePath());
	print("Global external storage path: " + GetGlobalExternalStoragePath());
	print("Current language: " + GetSharedData("ethanon.system.language"));
}

void onSceneUpdate()
{
	string str;
	str = addKeyStateToString(K_UP, str);
	str = addKeyStateToString(K_DOWN, str);
	str = addKeyStateToString(K_LEFT, str);
	str = addKeyStateToString(K_RIGHT, str);
	str = addKeyStateToString(K_PAGEDOWN, str);
	str = addKeyStateToString(K_PAGEUP, str);
	str = addKeyStateToString(K_SPACE, str);
	str = addKeyStateToString(K_ENTER, str);
	str = addKeyStateToString(K_HOME, str);
	str = addKeyStateToString(K_INSERT, str);
	str = addKeyStateToString(K_ESC, str);
	str = addKeyStateToString(K_TAB, str);
	str = addKeyStateToString(K_F1, str);
	str = addKeyStateToString(K_F2, str);
	str = addKeyStateToString(K_F3, str);
	str = addKeyStateToString(K_F4, str);
	str = addKeyStateToString(K_F5, str);
	str = addKeyStateToString(K_F6, str);
	str = addKeyStateToString(K_F7, str);
	str = addKeyStateToString(K_F8, str);
	str = addKeyStateToString(K_F9, str);
	str = addKeyStateToString(K_F10, str);
	str = addKeyStateToString(K_F11, str);
	str = addKeyStateToString(K_F12, str);
	str = addKeyStateToString(K_A, str);
	str = addKeyStateToString(K_B, str);
	str = addKeyStateToString(K_C, str);
	str = addKeyStateToString(K_D, str);
	str = addKeyStateToString(K_E, str);
	str = addKeyStateToString(K_F, str);
	str = addKeyStateToString(K_G, str);
	str = addKeyStateToString(K_H, str);
	str = addKeyStateToString(K_I, str);
	str = addKeyStateToString(K_J, str);
	str = addKeyStateToString(K_K, str);
	str = addKeyStateToString(K_L, str);
	str = addKeyStateToString(K_M, str);
	str = addKeyStateToString(K_N, str);
	str = addKeyStateToString(K_O, str);
	str = addKeyStateToString(K_P, str);
	str = addKeyStateToString(K_Q, str);
	str = addKeyStateToString(K_R, str);
	str = addKeyStateToString(K_S, str);
	str = addKeyStateToString(K_T, str);
	str = addKeyStateToString(K_U, str);
	str = addKeyStateToString(K_V, str);
	str = addKeyStateToString(K_W, str);
	str = addKeyStateToString(K_X, str);
	str = addKeyStateToString(K_Y, str);
	str = addKeyStateToString(K_Z, str);
	str = addKeyStateToString(K_0, str);
	str = addKeyStateToString(K_1, str);
	str = addKeyStateToString(K_2, str);
	str = addKeyStateToString(K_3, str);
	str = addKeyStateToString(K_4, str);
	str = addKeyStateToString(K_5, str);
	str = addKeyStateToString(K_6, str);
	str = addKeyStateToString(K_7, str);
	str = addKeyStateToString(K_8, str);
	str = addKeyStateToString(K_9, str);
	str = addKeyStateToString(K_CTRL, str);
	str = addKeyStateToString(K_ALT, str);
	str = addKeyStateToString(K_SHIFT, str);
	DrawText(vector2(0), str, "Verdana20_shadow.fnt", 0xFFFFFFFF);
}

string addKeyStateToString(const KEY key, const string str)
{
	ETHInput@ input = GetInputHandle();
	const string add = getKeyState(key);
	if (input.GetKeyState(key) != KS_UP)
		return str + add;
	else
		return str;
}

string getKeyState(const KEY key)
{
	ETHInput@ input = GetInputHandle();
	return (buttonKeyToString(key) + ": " + stateNameToString(input.GetKeyState(key)) + "\n");
}

string buttonKeyToString(const KEY key)
{
	switch (key)
	{
		case K_UP: return "UP";
		case K_DOWN: return "DOWN";
		case K_LEFT: return "LEFT";
		case K_RIGHT: return "RIGHT";
		case K_PAGEDOWN: return "PAGEDOWN";
		case K_PAGEUP: return "PAGEUP";
		case K_SPACE: return "SPACE";
		case K_ENTER: return "ENTER";
		case K_HOME: return "HOME";
		case K_INSERT: return "INSERT";
		case K_PAUSE: return "PAUSE";
		case K_ESC: return "ESC";
		case K_BACK: return "BACK";
		case K_TAB: return "TAB";
		case K_F1: return "F1";
		case K_F2: return "F2";
		case K_F3: return "F3";
		case K_F4: return "F4";
		case K_F5: return "F5";
		case K_F6: return "F6";
		case K_F7: return "F7";
		case K_F8: return "F8";
		case K_F9: return "F9";
		case K_F10: return "F10";
		case K_F11: return "F11";
		case K_F12: return "F12";
		case K_A: return "A";
		case K_B: return "B";
		case K_C: return "C";
		case K_D: return "D";
		case K_E: return "E";
		case K_F: return "F";
		case K_G: return "G";
		case K_H: return "H";
		case K_I: return "I";
		case K_J: return "J";
		case K_K: return "K";
		case K_L: return "L";
		case K_M: return "M";
		case K_N: return "N";
		case K_O: return "O";
		case K_P: return "P";
		case K_Q: return "Q";
		case K_R: return "R";
		case K_S: return "S";
		case K_T: return "T";
		case K_U: return "U";
		case K_V: return "V";
		case K_W: return "W";
		case K_X: return "X";
		case K_Y: return "Y";
		case K_Z: return "Z";
		case K_0: return "0";
		case K_1: return "1";
		case K_2: return "2";
		case K_3: return "3";
		case K_4: return "4";
		case K_5: return "5";
		case K_6: return "6";
		case K_7: return "7";
		case K_8: return "8";
		case K_9: return "9";
		case K_CTRL: return "CTRL";
		case K_ALT: return "ALT";
		case K_SHIFT: return "SHIFT";
	}
	return ("null");
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
