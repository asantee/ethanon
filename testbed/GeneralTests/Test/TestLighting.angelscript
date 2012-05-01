
class TestLighting : Test
{
	string getName()
	{
		return "Lighting test";
	}
	
	void start()
	{
		LoadScene("empty", PRELOOP, LOOP);
		HideCursor(false);
		SetBackgroundColor(0xFF000000);
	}
	
	void preLoop()
	{
		AddEntity("pillar_fire.ent", vector3(0,0,30), "cursor");
		@cursor = SeekEntity("cursor");
		cursor.AddToPosition(vector3(0, 0, cursor.GetPivotAdjust().y));
	
		const vector2 screenSize = GetScreenSize();
		for (uint t=0; t<60; t++)
		{
			ETHEntity@ ent;
			AddEntity("small_bump_test.ent",
				vector3(rand(screenSize.x),randF(screenSize.y),randF(128.0f)),
				randF(360.0f), @ent, "small_bump_surface", 0.5f + randF(1.5f));
		}
		for (uint t=0; t<15; t++)
		{
			AddEntity("palm.ent",
				vector3(rand(screenSize.x),randF(screenSize.y),0), 0);
		}
		
		SetAmbientLight(vector3(0.1f,0.1f,0.1f));
	}
	
	void loop()
	{
		ETHInput @input = GetInputHandle();
		const float moveSpeed = 60/GetFPSRate();
		if (input.GetKeyState(K_UP) == KS_DOWN)
		{
			cursor.AddToPosition(vector3(0, 0, moveSpeed));
		}
		if (input.GetKeyState(K_DOWN) == KS_DOWN)
		{
			cursor.AddToPosition(vector3(0, 0,-moveSpeed));
		}
		
		if (input.GetKeyState(K_P) == KS_HIT)
		{
			UsePixelShaders(true);
		}

		if (input.GetKeyState(K_V) == KS_HIT)
		{
			UsePixelShaders(false);
		}

		DrawText(cursor.GetPositionXY()-GetCameraPos(),
				"Press UP and DOWN to move the cursor light\n"
				"Press P and V to toggle pixel shaders",
				"Verdana14_shadow.fnt", ARGB(100,255,255,255));
	}
	
	ETHEntity @cursor;
}

