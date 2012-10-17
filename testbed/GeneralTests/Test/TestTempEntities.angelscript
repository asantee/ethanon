
class TestTempEntities : Test
{
	TestTempEntities()
	{
		errorMessage = "ERROR! TEMP ENTITIES AREN'T SUPPOSED TO BE RETRIEVED WITH SEEK ENTITY\x07\n";
		successMessage = "Success!\n";
	}

	string getName()
	{
		return "Temporary entity test";
	}
	
	void start()
	{
		SetScaleFactor(1.0f);
		LoadScene("scenes/temporary_entity_test.esc", PRELOOP, LOOP);
		HideCursor(false);
		SetBackgroundColor(0xFF000000);
	}

	void preLoop()
	{
		@cursor = SeekEntity("cursor");
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

		if (input.GetLeftClickState() == KS_HIT)
		{
			ETHEntity@ entity = null;
			AddEntity("explosion_single_particle.ent", vector3(input.GetCursorPos(), 0), @entity);
			if (entity !is null)
				print(successMessage);
		}

		if (input.GetRightClickState() == KS_HIT)
		{
			ETHEntity@ entity = null;
			AddEntity("explosion1.ent", vector3(input.GetCursorPos(), 0), @entity);
			if (entity !is null)
				print(successMessage);
		}
		
		if (input.GetKeyState(K_SPACE) == KS_HIT)
		{
			AddEntity("fireball.ent", vector3(input.GetCursorPos(), 0), "testFireBall");
		}

		ETHEntity@ entity;
		@entity = SeekEntity("explosion1.ent");
		if (entity !is null)
			print(errorMessage);
		@entity = SeekEntity("explosion_single_particle.ent");
		if (entity !is null)
			print(errorMessage);

		DrawText(GetCameraPos()+vector2(0, 50),
				"Press UP and DOWN to move the cursor light\n"
				"Press P and V to toggle pixel shaders\n"
				"Left/right click to add explosions",
				"Verdana14_shadow.fnt");
	}
	
	ETHEntity @cursor;
	string errorMessage;
	string successMessage;
}

void ETHCallback_testFireBall(ETHEntity@ entity)
{
	entity.AddToPositionXY(vector2(0,-UnitsPerSecond(200.0f)));
}