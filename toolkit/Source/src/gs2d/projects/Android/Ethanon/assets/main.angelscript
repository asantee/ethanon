void main()
{
	LoadScene("empty", "", "");
	print("External storage path:        " + GetExternalStoragePath());
	print("Global external storage path: " + GetGlobalExternalStoragePath());
}
