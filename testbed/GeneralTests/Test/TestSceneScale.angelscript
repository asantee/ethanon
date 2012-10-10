
class TestSceneScale : Test
{
	string getName()
	{
		return "Scene scale test";
	}

	void start()
	{
		SetFixedHeight(256.0f);
		LoadScene("scenes/sceneScaleTest.esc", PRELOOP, LOOP);
		//SetWindowProperties("Ethanon Engine", 854, 480, true, true, PF32BIT);
		SetBackgroundColor(0xFF000000);
	}
	
	void preLoop()
	{
		SetBorderBucketsDrawing(true);
		AddEntity("barrel.ent", vector3(128.0f, 128.0f, 0.0f));
		AddEntity("barrel_light.ent", vector3(0,0,4), "cursor");
	}
	
	void loop()
	{
		scrollCamera();
		drawSprites();
	}
	
	void drawSprites()
	{
		DrawSprite("entities/asteroid_64.png", Scale(vector2(1)), 0xFFFFFFFF);
		DrawSprite("entities/asteroid_64.png", Scale(vector2(1)) + GetSpriteSize("entities/asteroid_64.png") * 2, 0xFFFF0000, 180.0f);
		
		const vector2 size(128,128);
		SetSpriteOrigin("entities/barril.png", vector2(0));
		DrawShapedSprite("entities/barril.png", GetScreenSize() - size, size, 0xFFFFFFFF, 0.0f);
		DrawShapedSprite("entities/barril.png", GetScreenSize() - size * 0.9f, size, 0xFFFFFFFF, 0.0f);
		DrawShapedSprite("entities/barril.png", GetScreenSize() - size * 0.8f, size, 0xFFFFFFFF, 0.0f);
		DrawShapedSprite("entities/barril.png", GetScreenSize() - size * 0.7f, size, 0xFFFFFFFF, 0.0f);
		DrawShapedSprite("entities/barril.png", GetScreenSize() - size * 0.6f, size, 0xFFFFFFFF, 0.0f);
		
		ETHInput @input = GetInputHandle();
		if (input.GetKeyState(K_LMOUSE) == KS_HIT)
			PlayParticleEffect("fireball.par", input.GetCursorPos(), 0.0f, 1.0f);
		if (input.GetKeyState(K_RMOUSE) == KS_HIT)
			PlayParticleEffect("explosion.par", input.GetCursorPos(), 0.0f, 1.0f);
	}
	
	void scrollCamera()
	{
		ETHInput @input = GetInputHandle();
		const vector2 screenSize = GetScreenSize();
		const vector2 cursorPos = input.GetCursorPos();
		
		if (cursorPos.x > screenSize.x || cursorPos.y > screenSize.y
			|| cursorPos.x < 0 || cursorPos.y < 0)
		{
			return;
		}
		
		const vector2 cameraPos = GetCameraPos();
		const vector2 worldSpaceCursorPos = input.GetCursorPos()+cameraPos;
		
		const float borderScale = 0.15f;
		const float scrollSpeed = 100.0f/GetFPSRate();
		if (worldSpaceCursorPos.x > cameraPos.x+screenSize.x-(screenSize.x*borderScale))
		{
			AddToCameraPos(vector2(scrollSpeed, 0));
		}
		if (worldSpaceCursorPos.x < cameraPos.x+(screenSize.x*borderScale))
		{
			AddToCameraPos(vector2(-scrollSpeed, 0));
		}
		if (worldSpaceCursorPos.y > cameraPos.y+screenSize.y-(screenSize.y*borderScale))
		{
			AddToCameraPos(vector2(0, scrollSpeed));
		}
		if (worldSpaceCursorPos.y < cameraPos.y+(screenSize.y*borderScale))
		{
			AddToCameraPos(vector2(0,-scrollSpeed));
		}
	}

}
