void main()
{
	LoadScene("empty");
	print("External storage path:        " + GetExternalStoragePath());
	print("Global external storage path: " + GetGlobalExternalStoragePath());
	print("Current language: " + GetSharedData("ethanon.system.language"));
}
