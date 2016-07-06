class MyCustomObject
{
	MyCustomObject()
	{
		f = 0.0f;
		ui = 0;
		i = 0;
		str = "";
		@ent = null;
	}

	MyCustomObject(float _f, uint _ui, int _i, string _str, ETHEntity@ _ent)
	{
		f = _f;
		ui = _ui;
		i = _i;
		str = _str;
		@ent = @_ent;
	}
	
	void setColor(vector3 _color)
	{
		color = _color;
	}
	
	float f;
	uint ui;
	int i;
	string str;
	ETHEntity@ ent;
	vector3 color;
}

class TestCustomData : Test
{
	string getName()
	{
		return "Custom data test";
	}

	void start()
	{
		SetScaleFactor(1.0f);
		LoadScene("empty", PRELOOP, LOOP);
		HideCursor(false);
		SetWindowProperties("Ethanon Engine", 800, 800, true, true, PF32BIT);
		SetBackgroundColor(0xFF000000);
	}
	
	void preLoop()
	{
		AddEntity("barrel_on_fire.ent", vector3(GetScreenSize(), 0.0f), @barrelOnFire);
	
		SetAmbientLight(vector3(1,1,1));
		for (uint t=0; t<10; t++)
		{
			AddEntity("raw_barrel.ent", vector3(randF(GetScreenSize().x), randF(GetScreenSize().y), 0), "entity");
		}
		entities.clear();
		GetEntityArray("entity", entities);

		mathTest();

		AddFloatData("entity", "direction", 0.0f);
		checkData(DT_FLOAT, "direction");
		
		AddIntData("entity", "direction", 0);
		checkData(DT_INT, "direction");

		AddUIntData("entity", "direction", 0);
		checkData(DT_UINT, "direction");
		
		AddStringData("entity", "direction", "0");
		checkData(DT_STRING, "direction");
		
		AddVector3Data("entity", "direction", vector3(0,0,0));
		checkData(DT_VECTOR3, "direction");
		
		AddVector2Data("entity", "direction", vector2(0,0));
		checkData(DT_VECTOR2, "direction");
		
		for (uint t=0; t<entities.size(); t++)
		{
			const float angle = randF(PI*2.0f);
			entities[t].SetVector2("direction", vector2(sin(angle),cos(angle))*2.0f);
			{
				const float f = randF(10.0f);
				const uint ui = rand(10);
				const int i = rand(10);
				const string str = "id equals " + entities[t].GetID();
				entities[t].SetObject("myObj", MyCustomObject(f, ui, i, str, @barrelOnFire));

				// set a reference to an object
				{
					MyCustomObject object(66.6f, 666, -666, "666", @barrelOnFire);
					object.setColor(vector3(randF(1), randF(1), randF(1)));
					entities[t].SetObject("myObjRef", @object);
				}

				{
					MyCustomObject co;
					if (!entities[t].GetObject("myObj", co))
						print("GetObject returned null FAILED\x07");
					if (co.f != f)
						print("GetObject float value test FAILED\x07 " + f);
					if (co.ui != ui)
						print("GetObject uint value test FAILED\x07 " + ui);
					if (co.i != i)
						print("GetObject int value test FAILED\x07 " + i);
					if (co.str != str)
						print("GetObject string value test FAILED\x07 " + str);
					if (@co.ent != @barrelOnFire)
						print("GetObject reference value test FAILED\x07");
				}
				{
					MyCustomObject @co = null;
					if (!entities[t].GetObject("myObjRef", @co))
						print("GetObject returned null. Test FAILED\x07");

					if (@co is null)
						print("GetObject reference return is null. Test FAILED\x07");

					if (co.f != 66.6f)
						print("GetObject float value test FAILED\x07");
					else
						print("GetObject succeded... value is " + co.f);

					if (co.ui != 666)
						print("GetObject uint value test FAILED\x07");
					else
						print("GetObject succeded... value is " + co.ui);
						
					if (co.i != -666)
						print("GetObject int value test FAILED\x07");
					else
						print("GetObject succeded... value is " + co.i);
						
					if (co.str != "666")
						print("GetObject string value test FAILED\x07");
					else
						print("GetObject succeded... value is " + co.str);
						
					if (@co.ent !is @barrelOnFire)
						print("GetObject reference value test FAILED\x07");
					else
						print("GetObject succeded... value is barrelOnFire");
				}
			}
		}
		
		{
			MyCustomObject @co = null;
			entities[0].GetObject("myObjRef", @co);
			co.ent.SetPositionXY(GetScreenSize()/2.0f);
			co.ent.Scale(vector2(2, 2));
		}
		
		DrawFadingText(vector2(10,100), "Check the console for test results", "Verdana14_shadow.fnt", 0xFFFFFFFF, 5000);
	}
	
	void loop()
	{
		ETHInput @input = GetInputHandle();
		if (input.GetKeyState(K_SPACE) == KS_HIT)
		{
			mathTest();
		}
		if (input.GetKeyState(K_S) == KS_HIT)
		{
			SaveScene("scenes/customDataTestScene.esc");
		}
		if (input.GetKeyState(K_L) == KS_HIT)
		{
			LoadScene("scenes/customDataTestScene.esc", PRELOOP, LOOP);
		}

		for (uint t=0; t<entities.size(); t++)
		{
			entities[t].AddToPositionXY(entities[t].GetVector2("direction"));
			MyCustomObject @co = null;
			entities[t].GetObject("myObjRef", @co);
			entities[t].SetColor(co.color);
			if (!isPointInScreen(entities[t].GetPositionXY()))
			{
				entities[t].SetVector2("direction", entities[t].GetVector2("direction")*-1);
			}
		}
	}

	void checkData(const DATA_TYPE dataType, const string name)
	{
		print("\nBegining type check: \n");
		for (uint t=0; t<entities.size(); t++)
		{
			if (entities[t].CheckCustomData(name) == dataType)
			{
				print("Entity ID" + entities[t].GetID() + " type is OK: " + dtToString(dataType) + "\n");
			}
			else
			{
				print("Entity ID" + entities[t].GetID() + " type test FAILED. It is "
				+ dtToString(entities[t].CheckCustomData(name)) + " instead of " + dtToString(dataType) + "\x07\n");
			}
		}
	}

	string dtToString(const DATA_TYPE dt)
	{
		switch (dt)
		{
			case DT_INT:
				return "int";
			case DT_UINT:
				return "uint";
			case DT_FLOAT:
				return "float";
			case DT_STRING:
				return "string";
			case DT_VECTOR2:
				return "vector2";
			case DT_VECTOR3:
				return "vector3";
		};
		return "UNKNOWN";
	}

	void mathTest()
	{
		string mathTestOutput;
		uint fails = 0;
		for (uint t=0; t<entities.size(); t++)
		{
			const bool hasDataAlready = (entities[t].CheckCustomData("myFloat") != DT_NODATA);
			const int i = rand(1000)-1000;
			const uint ui = rand(1000);
			const float f = randF(1000.0f);
			const vector2 v2(randF(f)-f,randF(f)-f);
			const vector3 v3(randF(f)-f,randF(f)-f, randF(f)-f);

			const int iValue = rand(10)-10;
			const uint uiValue = rand(10);
			const float fValue = randF(10.0f);

			{
				if (entities[t].CheckCustomData("this uint does not exist") != DT_NODATA
				 || entities[t].CheckCustomData("this int does not exist") != DT_NODATA
				 || entities[t].CheckCustomData("this float does not exist") != DT_NODATA
				 || entities[t].CheckCustomData("this string does not exist") != DT_NODATA
				 || entities[t].CheckCustomData("this vector2 does not exist") != DT_NODATA
				 || entities[t].CheckCustomData("this vector3 does not exist") != DT_NODATA)
					mathTestOutput += ("inexistant custom data test FAILED\x07\n");

				if (entities[t].GetUInt("this uint does not exist", 12) != 12)
					mathTestOutput += ("GetUInt default value test FAILED\x07\n");
				else
					mathTestOutput += ("GetUInt default value test PASSED\n");

				if (entities[t].GetInt("this int does not exist", 13) != 13)
					mathTestOutput += ("GetInt default value test FAILED\x07\n");
				else
					mathTestOutput += ("GetInt default value test PASSED\n");

				if (entities[t].GetFloat("this float does not exist", 14.1f) != 14.1f)
					mathTestOutput += ("GetFloat default value test FAILED\x07\n");
				else
					mathTestOutput += ("GetFloat default value test PASSED\n");

				if (entities[t].GetString("this string does not exist", "dadada") != "dadada")
					mathTestOutput += ("GetString default value test FAILED\x07\n");
				else
					mathTestOutput += ("GetString default value test PASSED\n");

				if (entities[t].GetVector2("this vector2 does not exist", vector2(1.1f, 2.3f)) != vector2(1.1f, 2.3f))
					mathTestOutput += ("GetVector2 default value test FAILED\x07\n");
				else
					mathTestOutput += ("GetVector2 default value test PASSED\n");

				if (entities[t].GetVector3("this vector3 does not exist", vector3(2.2f, 3.4f, 4.5f)) != vector3(2.2f, 3.4f, 4.5f))
					mathTestOutput += ("GetVector3 default value test FAILED\x07\n");
				else
					mathTestOutput += ("GetVector3 default value test PASSED\n");
			}

			float fResult;
			int iResult;
			uint uiResult;
			vector2 v2Result;
			vector3 v3Result;

			{
				entities[t].SetFloat("myFloat", f);
				entities[t].SetInt("myInt", i);
				entities[t].SetUInt("myUInt", ui);
				entities[t].SetVector2("myVector2", v2);
				entities[t].SetVector3("myVector3", v3);

				fResult  = entities[t].MultiplyFloat("myFloat", fValue);
				iResult  = entities[t].MultiplyInt("myInt", iValue);
				uiResult = entities[t].MultiplyUInt("myUInt", uiValue);
				v2Result = entities[t].MultiplyVector2("myVector2", fValue);
				v3Result = entities[t].MultiplyVector3("myVector3", fValue);
			}

			if (entities[t].GetFloat("myFloat") == f*fValue && fResult == f*fValue)
			{
				mathTestOutput += "Float multiply test passed\n";
			}
			else
			{
				fails++;
				mathTestOutput += "Float multiply test FAILED!\x07\n";
			}

			if (entities[t].GetInt("myInt") == i*iValue && iResult == i*iValue)
			{
				mathTestOutput += "Int multiply test passed\n";
			}
			else
			{
				fails++;
				mathTestOutput += "Int multiply test FAILED!\x07\n";
			}
			
			if (entities[t].GetUInt("myUInt") == ui*uiValue && uiResult == ui*uiValue)
			{
				mathTestOutput += "UInt multiply test passed\n";
			}
			else
			{
				fails++;
				mathTestOutput += "UInt multiply test FAILED!\x07\n";
			}

			if (entities[t].GetVector2("myVector2") == v2*fValue && v2Result == v2*fValue)
			{
				mathTestOutput += "Vector2 multiply test passed\n";
			}
			else
			{
				fails++;
				mathTestOutput += "Vector2 multiply test FAILED!\x07\n";
			}

			if (entities[t].GetVector3("myVector3") == v3*fValue && v3Result == v3*fValue)
			{
				mathTestOutput += "Vector3 multiply test passed\n";
			}
			else
			{
				fails++;
				mathTestOutput += "Vector3 multiply test FAILED!\x07\n";
			}

			{
				fResult  = entities[t].AddToFloat("myFloat", fValue);
				iResult  = entities[t].AddToInt("myInt", iValue);
				uiResult = entities[t].AddToUInt("myUInt", uiValue);
				v2Result = entities[t].AddToVector2("myVector2", v2);
				v3Result = entities[t].AddToVector3("myVector3", v3);
			}

			if (entities[t].GetFloat("myFloat") == f*fValue+fValue && fResult == f*fValue+fValue)
			{
				mathTestOutput += "Float add test passed\n";
			}
			else
			{
				fails++;
				mathTestOutput += "Float add test FAILED!\x07\n";
			}

			if (entities[t].GetInt("myInt") == i*iValue+iValue && iResult == i*iValue+iValue)
			{
				mathTestOutput += "Int add test passed\n";
			}
			else
			{
				fails++;
				mathTestOutput += "Int add test FAILED!\x07\n";
			}
			
			if (entities[t].GetUInt("myUInt") == ui*uiValue+uiValue && uiResult == ui*uiValue+uiValue)
			{
				mathTestOutput += "UInt add test passed\n";
			}
			else
			{
				fails++;
				mathTestOutput += "UInt add test FAILED!\x07\n";
			}

			if (entities[t].GetVector2("myVector2") == v2*fValue+v2 && v2Result == v2*fValue+v2)
			{
				mathTestOutput += "Vector2 add test passed\n";
			}
			else
			{
				fails++;
				mathTestOutput += "Vector2 add test FAILED!\x07\n";
			}

			if (entities[t].GetVector3("myVector3") == v3*fValue+v3 && v3Result == v3*fValue+v3)
			{
				mathTestOutput += "Vector3 add test passed\n";
			}
			else
			{
				fails++;
				mathTestOutput += "Vector3 add test FAILED!\x07\n";
			}
			
			print(mathTestOutput + "\n");
		}
		print("Custom data tests finished - " + fails + " fails\n\n");
	}
	
	ETHEntityArray entities;
	ETHEntity@ barrelOnFire;
}
