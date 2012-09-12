#include "ETHFramework/IncludeModules.angelscript"
#include "Menu.angelscript"
#include "Game.angelscript"
#include "Gator.angelscript"
#include "Den.angelscript"
#include "EarthquakeGenerator.angelscript"
#include "Player.angelscript"
#include "GameOverLayer.angelscript"
#include "SoundManager.angelscript"
#include "CallBack_heart.angelscript"
#include "ScoreManager.angelscript"
#include "LoadScreen.angelscript"



void main()
{
	g_scale.updateScaleFactor(DEFAULT_SCALE_HEIGHT);
	@g_gameStateFactory = SMyStateFactory();
	
	//g_stateManager.setState(g_gameStateFactory.createMenuState());
	LoadScreen @ screen = LoadScreen();
	g_stateManager.setState(@screen);
	SetPersistentResources(true);
}

class MyChooser : ItemChooser
{
	bool performAction(const uint itemIdx)
	{
		g_stateManager.setState(g_gameStateFactory.createGameState(itemIdx));
		return true;
	}

	bool validateItem(const uint itemIdx)
	{
		return ((itemIdx > 15) ? false : true);
	}

	void itemDrawCallback(const uint index, const vector2 &in pos, const vector2 &in offset)
	{
	}
	void performDenialAction(const uint index)
	{
	}
}

class SMyStateFactory : SGameStateFactory
{
	State@ createMenuState()
	{
		return Menu();
	}

	State@ createLevelSelectState()
	{
		return createGameState(1);

	}

	State@ createGameState(const uint idx)
	{
		return Game(idx);
	}
}

void loop()
{
	g_stateManager.runCurrentLoopFunction();
	
	#if TESTING
		DrawText(V2_ZERO, "" + GetFPSRate(), "Verdana20_shadow.fnt", COLOR_WHITE);
	#endif
}

void preLoop()
{
	g_scale.updateScaleFactor(512,true);
	g_stateManager.runCurrentPreLoopFunction();
	g_soundmanager.loadSounds();
	SetZBuffer(false);
	SetPositionRoundUp(false);
	SetFastGarbageCollector(false);
	SetNumIterations(8, 3);
}

// called when the program is paused and then resumed (all resources were cleared, need reload)
void onResume()
{
	g_stateManager.runCurrentOnResumeFunction();
	g_soundmanager.onResume();
}
