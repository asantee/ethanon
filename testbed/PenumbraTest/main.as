/*
PORTUGUÊS
---------

Este arquivo é parte do Penumbra.

Penumbra é software livre; você pode redistribuí-lo e/ou
modificá-lo sob os termos da Licença Pública Menos Geral (LGPL)
GNU, conforme publicada pela Free Software Foundation; tanto a
versão 3 da Licença como (a seu critério) qualquer versão mais
nova.

Penumbra é distribuído na expectativa de ser útil, mas
SEM QUALQUER GARANTIA; sem mesmo a garantia implícita de
COMERCIALIZAÇÃO ou de ADEQUAÇÃO A QUALQUER PROPÓSITO EM
PARTICULAR. Consulte a Licença Pública Geral Menor (LGPL) GNU
para obter mais detalhes.

Você deve ter recebido uma cópia da Licença Pública Menos Geral
GNU junto com este programa; se não, veja em
<http://www.gnu.org/licenses/>.

ENGLISH
-------

This file is part of Penumbra.

Penumbra is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

Penumbra is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with Ethanon Engine. If not, see
<http://www.gnu.org/licenses/>.
*/

dictionary g_frameTimers;
enmlFile g_gameData;
ETHEntityArray g_spawn;
ETHEntityArray g_sounds;
MessageManager g_messages;
CameraManager g_camera;
Timer g_timer;

int g_lives = 0;
uint g_numNpcs = 0;
uint g_levelStartTime = 0;
bool g_gameFinished = false;
int[] g_exp(2, 0);
int[] g_charLevel(2, 8);
uint g_newRecordTime = 0;
bool[] g_castingLight(2, false);
int[] g_pvpPoints(2, 0);
Combo[] g_comboManager(2);

void resetData()
{
	g_exp[0] = g_exp[1] = 0;
	g_charLevel[0] = g_charLevel[1] = 1;
	g_gameData.getInt("global", "lives", g_lives);
	g_timer.start();
	g_gameFinished = false;
	g_newRecordTime = 0;
	g_castingLight[0] = g_castingLight[1] = false;
	g_pvpPoints[0] = g_pvpPoints[1] = 0;
}

#include "eth_util.as"
#include "messageManager.as"
#include "cameraManager.as"
#include "constants.as"
#include "util.as"
#include "characterAI.as"
#include "controlCharacters.as"
#include "doDamage.as"
#include "swords.as"
#include "interface.as"
#include "spells.as"
#include "potions.as"
#include "lavaShooter.as"
#include "environment.as"
#include "timer.as"
#include "scores.as"
#include "events.as"
#include "menu.as"
#include "gameover.as"
#include "videoModes.as"
#include "playerInput.as"
#include "combo.as"
#include "setupScene.as"
#include "switch.as"

void newGame(const string sceneName)
{
	resetData();
	
	if (IsPixelShaderSupported())
		UsePixelShaders(true);
	
	if (sceneName == "CAMPAIGN")
	{
		int level = 1;
		ETHInput @input = GetInputHandle();
		if (input.GetKeyState(K_2) == KS_DOWN)
			level = 2;
		if (input.GetKeyState(K_3) == KS_DOWN)
			level = 3;
		if (input.GetKeyState(K_PAGEUP) == KS_DOWN)
			g_charLevel[0] = g_charLevel[1] = 15;
		LoadScene("scenes/level" + level + ".esc", "setupScene", "levelLoop");
	}
	else
	{
		LoadScene("scenes/" + sceneName, "setupScene", "pvpLoop");
	}
}

void main()
{
	LoadScene("scenes/menu.esc", "menuPreLoop", "menuLoop", vector2(1024,256));
	//LoadScene("scenes/checkpoint.esc", "setupScene", "levelLoop");

	bool hideCursor = true;
	#if TESTING
		hideCursor = true;
	#endif
	HideCursor(hideCursor);

	const string str = GetStringFromFile(GetAbsolutePath("data.enml"));
	g_gameData.parseString(str);
	g_gameData.getInt("global", "lives", g_lives);

	bool windowed = true;
	#if TESTING
		windowed = true;
	#endif

	SetWindowProperties(APPLICATION_TITLE, 1024, 768, windowed, true, PF32BIT);
}

void ETHCallback_help(ETHEntity @thisEntity)
{
	ETHEntityArray entityArray;
	GetEntitiesFromBucket(thisEntity.GetCurrentBucket(), entityArray);
	for (uint t=0; t<entityArray.size(); t++)	
	{
		if (getDist(entityArray[t].GetPositionXY(), thisEntity.GetPositionXY()) < 30.0f)
		{
			if (isAMainCharacter(entityArray[t]))
			{
				g_messages.addMessage(thisEntity.GetStringData("message"));
			}
		}
	}
}

void ETHCallback_story(ETHEntity @thisEntity)
{
	shadowText(thisEntity.GetPositionXY()-GetCameraPos(),
		g_gameData.get("global", thisEntity.GetStringData("name")),
		"Arial Narrow", 16.0f, 100,203,203,228);
}

void ETHCallback_checkpoint(ETHEntity @thisEntity)
{
	ETHEntityArray entityArray;
	GetEntitiesFromBucket(thisEntity.GetCurrentBucket(),entityArray);
	for (uint t=0; t<entityArray.size(); t++)	
	{
		if (getDist(entityArray[t].GetPositionXY(), thisEntity.GetPositionXY()) < 30.0f)
		{
			if (isAMainCharacter(entityArray[t]))
			{
				if (entityArray[t].GetUIntData("playerId") == 0)
				{
					entityArray[t].AddUIntData("hasCheckpoint", 1);
					entityArray[t].AddIntData("lives", g_lives);
					SaveScene("scenes/checkpoint.esc");
					g_messages.addMessage("Checkpoint...");
					DeleteEntity(thisEntity);
					AddEntity("checkpoint_effect.ent", thisEntity.GetPosition()+vector3(0,0,10), 0.0f);
				}
			}
		}
	}
}

void ETHCallback_next_level(ETHEntity @thisEntity)
{
	if (thisEntity.CheckCustomData("fadeOut") == DT_NODATA)
	{
		ETHEntityArray entityArray;
		GetEntitiesFromBucket(thisEntity.GetCurrentBucket(),entityArray);
		for (uint t=0; t<entityArray.size(); t++)	
		{
			if (getDist(entityArray[t].GetPositionXY(), thisEntity.GetPositionXY()) < 80.0f)
			{
				if (isAMainCharacter(entityArray[t]))
				{
					if (entityArray[t].GetUIntData("playerId") == 0)
					{
						if (getPlayerXYAxis(0).y > 0)
						{
							thisEntity.AddUIntData("fadeOut", GetTime());
						}
					}
				}
			}
		}
	}
	else
	{
		float bias = 0;
		if (fadeOut(thisEntity.GetUIntData("fadeOut"), bias))
		{
			if (IsPixelShaderSupported())
				UsePixelShaders(true);
			LoadScene("scenes/" + thisEntity.GetStringData("name"), "setupScene", "levelLoop");
		}
		SetSampleVolume("soundfx/fase.mp3", 1.0f-bias);
		loadingMessage();
	}
}
