void main()
{
	LoadScene("empty", "", "onSceneUpdate");
	print("External storage path:        " + GetExternalStoragePath());
	print("Global external storage path: " + GetGlobalExternalStoragePath());
	print("Current language: " + GetSharedData("ethanon.system.language"));
}

void onSceneUpdate()
{
	DrawText(vector2(0), "Oi meu nome é André\näãáàéèëïíìóòöõúùüñÄÃÁÀÉÈËÏÍÌÓÒÖÕÚÙÜÑ", "Verdana20_shadow.fnt", 0xFFFFFFFF);
}