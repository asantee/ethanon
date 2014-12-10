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
}