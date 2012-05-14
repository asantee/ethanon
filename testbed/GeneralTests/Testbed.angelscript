#include "Test/Test.angelscript"
#include "Test/TestEntity.angelscript"
#include "Test/TestLighting.angelscript"
#include "Test/TestCustomData.angelscript"
#include "Test/TestFile.angelscript"
#include "Test/TestTempEntities.angelscript"
#include "Test/TestRigidBodies.angelscript"
#include "Test/TestSceneScale.angelscript"

class Testbed
{
	Testbed()
	{
		currentTest = 0;
		tests.resize(7);

		TestEntity entity;
		@tests[0] = (@entity);

		TestLighting lighting;
		@tests[1] = (@lighting);

		TestCustomData customData;
		@tests[2] = (@customData);

		TestFile fileTest;
		@tests[3] = (@fileTest);

		TestTempEntities tempTest;
		@tests[4] = (@tempTest);

		TestRigidBodies rigidBodies;
		@tests[5] = (@rigidBodies);

		TestSceneScale sceneScale;
		@tests[6] = (@sceneScale);
	}
	
	void start()
	{
		tests[0].start();
	}
	
	void runCurrentPreLoopFunction()
	{
		tests[currentTest].preLoop();
		SetPositionRoundUp(true);
	}
	
	void runCurrentLoopFunction()
	{
		tests[currentTest].loop();
		showFPSRate();
		showElapsedTime();
		testSelector();
	}
	
	void showFPSRate()
	{
		const string font = "Verdana20.fnt";
		const string fps = ""+GetFPSRate();
		const string text = fps + " (" + GetNumRenderedEntities() + "/" + GetNumEntities() + ")";
		const vector2 size = ComputeTextBoxSize(font, text);
		DrawText(GetScreenSize()-size, text, font, ARGB(150,255,255,255));
	}

	void testSelector()
	{
		uint nextTest = currentTest;
		ETHInput @input = GetInputHandle();
		const vector2 cursorPos = input.GetCursorPos();
		const vector2 screenSize = GetScreenSize();

		const uint fKeyPressed = whichFKey();
		if (fKeyPressed > 0 && fKeyPressed <= tests.length())
			nextTest = fKeyPressed-1;
		
		if (nextTest != currentTest)
		{
			currentTest = nextTest;
			tests[currentTest].start();
		}

		{
			string output;
			for (uint t=0; t<tests.length(); t++)
			{
				output += "F" + (t+1) + ": " + tests[t].getName() + "\n";
			}
			const string font = "Verdana14_shadow.fnt";
			const vector2 size = ComputeTextBoxSize(font, output);
			DrawText(vector2(screenSize.x-size.x,0), output, font, ARGB(150,255,255,255));
		}
	}

	uint whichFKey()
	{
		ETHInput @input = GetInputHandle();
		if (input.GetKeyState(K_F1) == KS_HIT)	return 1;
		if (input.GetKeyState(K_F2) == KS_HIT)	return 2;
		if (input.GetKeyState(K_F3) == KS_HIT)	return 3;
		if (input.GetKeyState(K_F4) == KS_HIT)	return 4;
		if (input.GetKeyState(K_F5) == KS_HIT)	return 5;
		if (input.GetKeyState(K_F6) == KS_HIT)	return 6;
		if (input.GetKeyState(K_F7) == KS_HIT)	return 7;
		if (input.GetKeyState(K_F8) == KS_HIT)	return 8;
		if (input.GetKeyState(K_F9) == KS_HIT)	return 9;
		if (input.GetKeyState(K_F10) == KS_HIT)	return 10;
		return 0;
	}
	
	void showElapsedTime()
	{
		dateTime dt;
		const string str = "\n" + dt.getMonth() + "/" + dt.getDay() + "/" + dt.getYear() + " - " + dt.getHours() + ":" + dt.getMinutes();
		DrawText(vector2(0,0), "Elapsed time: " + (GetTime()/1000) + str, "Verdana14_shadow.fnt", ARGB(150,255,255,255));
	}
	
	Test@[] tests;
	uint currentTest;
}