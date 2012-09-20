void main()
{
	LoadScene("empty", "", "onSceneUpdate");
	print("External storage path:        " + GetExternalStoragePath());
	print("Global external storage path: " + GetGlobalExternalStoragePath());
}

uint time = 0;

void onSceneUpdate()
{
	time += GetLastFrameElapsedTime();
	if (time > 2000)
	{
		const vector2 screenSize = GetScreenSize();
		DrawFadingText(vector2(randF(screenSize.x), randF(screenSize.y)), "#3", "Verdana20_shadow.fnt", 0xFFFFFFFF, 1000, 4.0f);
		time = 0;
	}
	DrawText(vector2(0), "" + GetFPSRate() + "\n" + GetTime() + "\n" + GetTimeF(), "Verdana20_shadow.fnt", 0xFFFFFFFF);
}