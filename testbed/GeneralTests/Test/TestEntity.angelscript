
class TestEntity : Test
{
	string getName()
	{
		return "Entity test";
	}

	void start()
	{
		SetScaleFactor(1.0f);
		LoadScene("scenes/TestEntity.esc", PRELOOP, LOOP);
		HideCursor(false);
		SetWindowProperties("Ethanon Engine", 1024, 768, true, true, PF32BIT);
		SetBackgroundColor(0xFF000000);
	}
	
	void preLoop()
	{
		@cursor = SeekEntity("cursor");

		barrels.clear();
		GetEntityArray("barrel.ent", @barrels);
		print("\n\nBarrels found: " + barrels.size() + "\n");
		print("Adding the same barrels to the list\n");
		GetEntityArray("barrel.ent", @barrels);
		print("Barrels found: " + barrels.size() + "\n");
		print("Calling unique method to remove repeated entities\n");
		barrels.unique();
		print("Barrels found: " + barrels.size() + "\n");
		DrawFadingText(vector2(10,300), "If you see this it's\nbecause multipage bitmap\nfonts work just fine",
				"Verdana128.fnt", 0xFFFFFFFF, 6000);

		const int birdId = AddEntity("bird.ent", vector3(GetScreenSize()/2, 10.0f), 0);
		@bird = SeekEntity(birdId);

		print("Message A: an error message regarding duplicate name is already expected");
		LoadSprite("sprite/barril.png");
		print("Message B: If there were no error messages between Messages A and B, something is wrong");
		
		// ad a static entity pretty far away to see if its constructor gets called
		AddEntity("barrel.ent", vector3(17000, 17000, 0), "static_barrel");
	}
	
	void updateBird()
	{
		if (bird.GetFrame() >= bird.GetNumFrames()-1)
		{
			bird.SetFrame(0,0);
		}
		else
		{
			const uint nextFrame = bird.GetFrame() + 1;
			bird.SetFrame(nextFrame);
			if (bird.GetFrame() != nextFrame)
			{
				print("Error while setting bird frame!\x07");
			}
		}
	}
	
	void loop()
	{
		updateBird();
		if (cursor is null)
		{
			print("The cursor handle hasn't been found!\n");
			return;
		}
		else
		{
			ETHInput @input = GetInputHandle();
			const bool rightClick  = (input.GetRightClickState() == KS_HIT);
			const bool middleClick = (input.GetMiddleClickState() == KS_HIT);
			const bool _1Key = (input.GetKeyState(K_1) == KS_HIT);
			const bool _2Key = (input.GetKeyState(K_2) == KS_HIT);
			const bool _3Key = (input.GetKeyState(K_3) == KS_HIT);
			const bool minusKey = (input.GetKeyState(K_MINUS) == KS_HIT);
			if (rightClick or middleClick or _1Key or _2Key or _3Key or minusKey)
			{
				ETHEntityArray pickEntities;
				GetIntersectingEntities(cursor.GetPositionXY(), pickEntities, false);
				for (uint t=0; t<pickEntities.size(); t++)
				{
					if (pickEntities[t].GetEntityName() == "barrel.ent" || pickEntities[t].GetEntityName() == "blood.ent")
					{
						if (rightClick && pickEntities[t].GetEntityName() == "barrel.ent")
						{
							DeleteEntity(pickEntities[t]);
							barrels.removeDeadEntities();
							//barrels.unique();
							DrawFadingText(vector2(10,100), "Current size of entity array: " + barrels.size(),
											"Verdana14_shadow.fnt", 0xFFFFFFFF, 3000);
						}
						if (middleClick)
						{
							pickEntities[t].Scale(1.1f);
						}
						if (_1Key)
						{
							pickEntities[t].SetScale(vector2(1,1));
						}
						if (_2Key)
						{
							pickEntities[t].SetScale(vector2(2,2));
						}
						if (_3Key)
						{
							pickEntities[t].SetScale(vector2(3,3));
						}
						if (minusKey)
						{
							pickEntities[t].Scale(vector2(0.9f,0.9f));
						}
					}
				}
			}
		}
		
		scrollCamera();
		moveDynamicBarrels();
	}
	
	void moveDynamicBarrels()
	{
		vector2 currentBucket = cursor.GetCurrentBucket();
		DrawText(cursor.GetPositionXY()-GetCameraPos(),
				 "Hold the left mouse button\nto move dynamic entities\nto the mouse cursor position\n"
				 + currentBucket.x + ", " + currentBucket.y,
				 "Verdana14_shadow.fnt", ARGB(70,255,255,255), 2.0f);
		
		ETHInput @input = GetInputHandle();
		if (input.GetLeftClickState() == KS_DOWN)
		{
			const float moveSpeed = 60.0f/GetFPSRate();
			const vector2 cameraPos = GetCameraPos();
			const vector2 worldSpaceCursorPos = input.GetCursorPos()+cameraPos;
			for (uint t=0; t<barrels.size(); t++)
			{
				if (barrels[t].IsStatic())
					continue;
				barrels[t].AddToPositionXY(
					normalize(worldSpaceCursorPos-barrels[t].GetPositionXY())*moveSpeed);
			}
		}
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
	
	ETHEntity @cursor;
	ETHEntity @bird;
	ETHEntityArray barrels;
}

void ETHConstructorCallback_bird(ETHEntity@ thisEntity)
{
	thisEntity.Scale(2.0f);
}

void ETHConstructorCallback_static_barrel(ETHEntity@ thisEntity)
{
	print("Static barrel constructor...this shouldn't have happened\x07");
	thisEntity.SetString("test", "test value");
}

void ETHConstructorCallback_barrel(ETHEntity@ thisEntity)
{
	print("Barrel constructor!");
	thisEntity.SetString("test", "test value");
}

void ETHDestructorCallback_barrel(ETHEntity@ thisEntity)
{
	if (thisEntity.GetString("test") == "test value")
	{
		print("Barrel destructor test passed...");
	}
}
