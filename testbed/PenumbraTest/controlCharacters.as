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

void doCharacterCollision(ETHEntity @thisEntity, const bool useNpcInvisibleWalls)
{
	const vector2 force = getCurrentForce(thisEntity);
	const uint dir = findDirection(force);
	
	// assign the current direction to the entity
	thisEntity.AddUIntData("walking", (dir == LEFT || dir == RIGHT) ? 1 : 0);
	
	ETHEntityArray collidableEntities;
	GetEntitiesFromBucket(thisEntity.GetCurrentBucket(), collidableEntities);
	collidableEntities += findDestinationBuckets(thisEntity, force, true);
			
	collisionBox thisBox = thisEntity.GetCollisionBox();
	thisBox.pos += thisEntity.GetPosition();

	const int size = collidableEntities.size();
	bool thinnerBoxHit = false;
	uint collisionCount = 0;
	for (int t=0; t<size; t++)
	{
		if (!collidableEntities[t].Collidable())
			continue;

		if (!useNpcInvisibleWalls)
		{
			if (collidableEntities[t].GetEntityName() == "npc_wall.ent")
			{
				continue;
			}
		}

		if (thisEntity.GetID() == collidableEntities[t].GetID())
			continue;

		// if the target entity's owner is itself (it's probably a sword effect
		if (collidableEntities[t].CheckCustomData("ownerID") != DT_NODATA)
			continue;
		
		collisionBox box = collidableEntities[t].GetCollisionBox();
		box.pos += collidableEntities[t].GetPosition();
		
		if (checkBoxHit(thisBox, box))
		{
			collisionCount++;
			
			if (collidableEntities[t].GetEntityName() == "instant_death.ent"
				|| collidableEntities[t].GetEntityName() == "instant_death2.ent"
				|| collidableEntities[t].GetEntityName() == "instant_death3.ent")
			{
				thisEntity.AddIntData("hp", 0);
				if (isAMainCharacter(thisEntity))
				{
					if (thisEntity.CheckCustomData("pvpMode") != DT_NODATA)
					{
						g_pvpPoints[thisEntity.GetUIntData("playerId")]--;
						thisEntity.AddUIntData("dontFrag", 1);
					}
				}
			}

			if (collidableEntities[t].CheckCustomData("dontCollide") != DT_NODATA)
				continue;
			

			if (collidableEntities[t].CheckCustomData("hp") != DT_NODATA)
			{
				// if both are characters, knock'em back
				if (collidableEntities[t].CheckCustomData("hp") != DT_NODATA
					&& thisEntity.CheckCustomData("hp") != DT_NODATA)
				{
					vector2 thisPos = thisEntity.GetPositionXY();
					vector2 pos = collidableEntities[t].GetPositionXY();
					vector2 half = (thisPos+pos)/2;
					knockBack(thisEntity, normalize(thisPos-pos)*7);
					knockBack(collidableEntities[t], normalize(pos-thisPos)*7);
					AddEntity("jumpfx.ent", vector3(half.x, half.y, 0), 0.0f);
					continue;
				}
			}

			// check collision against a thinner box to prevent "wall grabing"
			collisionBox thinnerBox = thisBox;
			thinnerBox.size.x *= 0.6f;
			thinnerBoxHit = checkBoxHit(thinnerBox, box);

			const vector2 currentPos = thisEntity.GetPositionXY();
			uint collDir = findBoxDirection(thisBox, box);
			if (collDir == RIGHT)
			{
				setForceX(thisEntity, 0);
				thisEntity.SetPositionXY(
					vector2(((box.pos.x - box.size.x/2) - thisBox.size.x/2)-1, currentPos.y));
				
				// refresh box position
				thisBox = thisEntity.GetCollisionBox();
				thisBox.pos += thisEntity.GetPosition();
			}
			else if (collDir == LEFT)
			{
				setForceX(thisEntity, 0);
				thisEntity.SetPositionXY(
					vector2(((box.pos.x + box.size.x/2) + thisBox.size.x/2)+1, currentPos.y));

				// refresh box position
				thisBox = thisEntity.GetCollisionBox();
				thisBox.pos += thisEntity.GetPosition();
			}
			else if (collDir == DOWN)
			{
				// if it's falling...
				if (force.y > 0)
				{
					if (thinnerBoxHit)
					{
						if (force.y > 700.0f)
						{
							PlaySample("soundfx/fall.ogg");
							const vector3 pos(thisBox.pos.x, thisBox.pos.y+thisBox.size.y/2, 0.0f);
							AddEntity("fall.ent", pos, 0);
							g_camera.startEarthquake(10.0f);
						}
						
						setForceY(thisEntity, 0);
						thisEntity.SetPositionXY(
							vector2(currentPos.x, (box.pos.y - box.size.y/2) - thisBox.size.y/2));
						thisEntity.AddUIntData("touchingGround", 1);
					}
				}
			}
			else if (collDir == UP)
			{
				if (thinnerBoxHit)
				{
					setForceY(thisEntity, 0);
					thisEntity.SetPositionXY(
						vector2(currentPos.x, (box.pos.y + box.size.y/2) + thisBox.size.y/2));
				}
			}
		} // if collided...
		
	} // for
	
	// if he's up, he's not touching the ground
	if (force.y < 0)
	{
		thisEntity.AddUIntData("touchingGround", 0);
	}
	else
	{
		if (!thinnerBoxHit)
		{
			thisEntity.AddUIntData("touchingGround", 0);
		}
	}

	// if it touches the ground, zero the jump count
	if (thisEntity.GetUIntData("touchingGround") == 1)
		thisEntity.AddIntData("jumps", 0);

	#if TESTING
	/*if (thisEntity.GetEntityName() == MAIN_CHARACTER_ENTITY0)
	{
		int hp = thisEntity.GetIntData("hp");
		DrawText(vector2(3,60),
			"FPS: " + GetFPSRate() + "\n"
			//+ "Size: " + size + "\n"
			//+ "touchingGround: " + thisEntity.GetUIntData("touchingGround") + "\n"
			//+ "forceX: " + thisEntity.GetFloatData("forceX") + "\n"
			+ "forceY: " + thisEntity.GetFloatData("forceY") + "\n"
			//+ "knockBackX: " + thisEntity.GetFloatData("knockBackX") + "\n"
			//+ "knockBackY: " + thisEntity.GetFloatData("knockBackY") + "\n"
			//+ "direction: " + directionToString(dir) + "\n" + "hp: " + hp + "\n"
			//+ "collisionCount: " + collisionCount + "\n"
			+ "\n",
		"Verdana", 15, 0xFFFFFFFF);
	}*/
	#endif
}

void animateCharacter(ETHEntity @thisEntity, frameTimer @timer)
{
	if (timer !is null)
	{
		const uint stride = thisEntity.GetUIntData("stride");
		const uint dir = thisEntity.GetUIntData("currentDir");
		const bool touchingGround = (thisEntity.GetUIntData("touchingGround") != 0);
		const vector2 force = getCurrentForce(thisEntity);
		
		if (touchingGround)
		{
			if (force.x < 0)
			{
				timer.Set(4, 7, stride);
			}
			else if (force.x > 0)
			{
				timer.Set(8, 11, stride);
			}
			else
			{
				if (dir == LEFT)
					timer.Set(4, 4, stride);
				else if (dir == RIGHT)
					timer.Set(8, 8, stride);
			}
		}
		else
		{
			if (dir == LEFT)
			{
				if (force.y > 0)
					timer.Set(5, 5, stride);
				else
					timer.Set(4, 4, stride);
			}
			else if (dir == RIGHT)
			{
				if (force.y > 0)
					timer.Set(9, 9, stride);
				else
					timer.Set(8, 8, stride);
			}
		}
		thisEntity.SetFrame(timer.Get());
	}
}

void ETHCallback_bruxo(ETHEntity @thisEntity)
{
	g_camera.setMainCharPos(thisEntity.GetPositionXY(), 0);

	frameTimer @timer;
	
	// if it has no timer yet, instantiate a timer for this one
	if (!g_frameTimers.get("id" + thisEntity.GetID(), @timer))
	{
		frameTimer newTimer;
		newTimer.Set(thisEntity.GetFrame(), thisEntity.GetFrame(), 0);
		g_frameTimers.set("id" + thisEntity.GetID(), @newTimer);
		@timer = newTimer;
	}

	if (g_gameFinished)
	{
		thisEntity.AddIntData("hp", 100);
	}
	if (!isMainCharDead(thisEntity, "fade_out_beam.ent"))
	{
		applyForce(thisEntity, vector2(0,UnitsPerSecond(GRAVITY)));
		controlCharacter(thisEntity, 0);
		move(thisEntity);
		doCharacterCollision(thisEntity, false);
		animateCharacter(thisEntity, timer);
		drawPlayerStatus(thisEntity);
		doMpRecovery(thisEntity, 350, 1);
		
		if (thisEntity.CheckCustomData("pvpMode") == DT_NODATA)
			player1Summoner(thisEntity);
	}
}

void ETHCallback_princess(ETHEntity @thisEntity)
{
	frameTimer @timer;
	
	// if it has no timer yet, instantiate a timer for this one
	if (!g_frameTimers.get("id" + thisEntity.GetID(), @timer))
	{
		frameTimer newTimer;
		newTimer.Set(thisEntity.GetFrame(), thisEntity.GetFrame(), 0);
		g_frameTimers.set("id" + thisEntity.GetID(), @newTimer);
		@timer = newTimer;
	}

	if (g_gameFinished)
	{
		thisEntity.AddIntData("hp", 100);
	}
	
	const bool pvpMode = (thisEntity.CheckCustomData("pvpMode") != DT_NODATA);
	
	bool dead = false;
	if (pvpMode)
		dead = isMainCharDead(thisEntity, "fade_out_beam.ent");
	else
		dead = isDead(thisEntity, "fade_out_beam.ent");
	
	if (!dead)
	{
		g_camera.setMainCharPos(thisEntity.GetPositionXY(), 1);

		applyForce(thisEntity, vector2(0,UnitsPerSecond(GRAVITY)));
		controlCharacter(thisEntity, 1);
		move(thisEntity);
		doCharacterCollision(thisEntity, false);
		animateCharacter(thisEntity, timer);
		drawPlayerStatus(thisEntity);
		doMpRecovery(thisEntity, 350, 1);

		if (!pvpMode && thisEntity.CheckCustomData("lastTimeAlive") != DT_NODATA)
		{
			if (GetTime()-thisEntity.GetUIntData("lastTimeAlive") < 4000)
			{
				if (!isInScreen(thisEntity))
				{
					g_messages.addMessage("A criatura invocada não pode sair do campo de visão da tela");
					if (GetTime()-thisEntity.GetUIntData("vanishTime") > 3000)
					{
						thisEntity.AddIntData("hp", 0);
						g_camera.startEarthquake(20.0f);
					}
				}
				else
				{
					thisEntity.AddUIntData("vanishTime", GetTime());
				}
			}
		}
	}
	else
	{
		g_camera.setMainCharPos(vector2(0,0), 1);
	}
	thisEntity.AddUIntData("lastTimeAlive", GetTime());
}

bool isDead(ETHEntity @thisEntity, const string beamEffect)
{
	if (thisEntity.GetIntData("hp") <= 0)
	{
		const uint now = GetTime();
		if (thisEntity.CheckCustomData("deathTime") == DT_NODATA)
		{
			if (beamEffect != "")
				AddEntity(beamEffect, thisEntity.GetPosition()+vector3(0,0,4), 0.0f);
			if (thisEntity.HasParticleSystem(0))
				thisEntity.KillParticleSystem(0);
			thisEntity.AddUIntData("deathTime", now);
			thisEntity.SetColor(vector3(0,0,0));
			thisEntity.SetCollision(false);

			if (thisEntity.CheckCustomData("pvpMode") == DT_NODATA && !isAMainCharacter(thisEntity))
			{
				// add death to exp
				addToExp(0, thisEntity.GetIntData("expGiven"));
				addToExp(1, thisEntity.GetIntData("expGiven"));
			}
		}
		const uint deathTime = thisEntity.GetUIntData("deathTime");
		const uint deadTime = now-deathTime;
		
		if (deadTime >= DEAD_FADE_OUT_TIME)
		{
			DeleteEntity(thisEntity);
			return true;
		}
		thisEntity.SetAlpha(1.0f-(float(deadTime)/float(DEAD_FADE_OUT_TIME)));

		return true;
	}
	return false;
}

bool isMainCharDead(ETHEntity @thisEntity, const string beamEffect)
{
	const bool pvpMode = (thisEntity.CheckCustomData("pvpMode") != DT_NODATA);
	const uint playerId = thisEntity.GetUIntData("playerId");
	
	if (pvpMode && g_gameFinished)
		return true;
	
	if (thisEntity.GetIntData("hp") <= 0)
	{
		const uint now = GetTime();
		if (thisEntity.CheckCustomData("deathTime") == DT_NODATA)
		{
			if (beamEffect != "")
				AddEntity(beamEffect, thisEntity.GetPosition(), 0.0f);
			if (thisEntity.HasParticleSystem(0))
				thisEntity.KillParticleSystem(0);
			thisEntity.AddUIntData("deathTime", now);
			thisEntity.SetColor(vector3(0,0,0));
			thisEntity.SetCollision(false);
		}
		const uint deathTime = thisEntity.GetUIntData("deathTime");
		const uint deadTime = now-deathTime;
		
		if (deadTime >= DEAD_FADE_OUT_TIME)
		{
			g_lives--;
			if (g_lives >= 0 || pvpMode)
			{
				if (thisEntity.CheckCustomData("hasCheckpoint") == DT_NODATA)
				{
					if (!pvpMode)
						LoadScene(GetSceneFileName(), "setupScene", "levelLoop");
					else
						LoadScene(GetSceneFileName(), "setupScene", "pvpLoop");
				}
				else
				{
					LoadScene("scenes/checkpoint.esc", "setupScene", "levelLoop");
				}
			}
			else
			{
				LoadScene("scenes/gameover.esc", "gameOverPreLoop", "gameOverLoop");
			}
		}

		const float alpha = (float(deadTime)/float(DEAD_FADE_OUT_TIME));
		if (IsSamplePlaying("soundfx/fase.mp3"))
			SetSampleVolume("soundfx/fase.mp3", 1.0f-alpha);
		
		thisEntity.SetAlpha(1.0f-alpha);
		const uint rectColor = ARGB(uint8(alpha*255.0f),0,0,0);
		DrawRectangle(vector2(0,0), GetScreenSize(), rectColor, rectColor, rectColor, rectColor);

		if (alpha > 0.7f && g_lives >= 0)
		{
			loadingMessage();
		}
		
		return true;
	}
	return false;
}

void ETHCallback_warrior(ETHEntity @thisEntity)
{
	g_numNpcs++;
	frameTimer @timer;

	// if it has no timer yet, instantiate a timer for this one
	if (!g_frameTimers.get("id" + thisEntity.GetID(), @timer))
	{
		frameTimer newTimer;
		newTimer.Set(thisEntity.GetFrame(), thisEntity.GetFrame(), 0);
		g_frameTimers.set("id" + thisEntity.GetID(), @newTimer);
		@timer = newTimer;
	}
	
	if (!isDead(thisEntity, "fade_out_beam.ent"))
	{
		applyForce(thisEntity, vector2(0,UnitsPerSecond(GRAVITY)));
		meleeCharacterAI(thisEntity, "enemy_sword.ent");
		move(thisEntity);
		doCharacterCollision(thisEntity, true);
		animateCharacter(thisEntity, timer);
	}
}

void ETHCallback_minion(ETHEntity @thisEntity)
{
	g_numNpcs++;
	frameTimer @timer;

	// if it has no timer yet, instantiate a timer for this one
	if (!g_frameTimers.get("id" + thisEntity.GetID(), @timer))
	{
		frameTimer newTimer;
		newTimer.Set(thisEntity.GetFrame(), thisEntity.GetFrame(), 0);
		g_frameTimers.set("id" + thisEntity.GetID(), @newTimer);
		@timer = newTimer;
	}
	
	if (!isDead(thisEntity, "fade_out_beam.ent"))
	{
		applyForce(thisEntity, vector2(0,UnitsPerSecond(GRAVITY)));
		meleeCharacterAI(thisEntity, "enemy_sword.ent");
		move(thisEntity);
		doCharacterCollision(thisEntity, true);
		animateCharacter(thisEntity, timer);
	}
}

void ETHCallback_knight(ETHEntity @thisEntity)
{
	g_numNpcs++;
	frameTimer @timer;

	// if it has no timer yet, instantiate a timer for this one
	if (!g_frameTimers.get("id" + thisEntity.GetID(), @timer))
	{
		frameTimer newTimer;
		newTimer.Set(thisEntity.GetFrame(), thisEntity.GetFrame(), 0);
		g_frameTimers.set("id" + thisEntity.GetID(), @newTimer);
		@timer = newTimer;
	}
	
	if (!isDead(thisEntity, "fade_out_beam.ent"))
	{
		applyForce(thisEntity, vector2(0,UnitsPerSecond(GRAVITY)));
		meleeCharacterAI(thisEntity, "enemy_sword.ent");
		move(thisEntity);
		doCharacterCollision(thisEntity, true);
		animateCharacter(thisEntity, timer);
	}
}

void ETHCallback_impy(ETHEntity @thisEntity)
{
	g_numNpcs++;
	frameTimer @timer;

	// if it has no timer yet, instantiate a timer for this one
	if (!g_frameTimers.get("id" + thisEntity.GetID(), @timer))
	{
		frameTimer newTimer;
		newTimer.Set(thisEntity.GetFrame(), thisEntity.GetFrame(), 0);
		g_frameTimers.set("id" + thisEntity.GetID(), @newTimer);
		@timer = newTimer;
	}
	
	if (!isDead(thisEntity, "fade_out_beam.ent"))
	{
		applyForce(thisEntity, vector2(0,UnitsPerSecond(GRAVITY)));
		rangedCharacterAI(thisEntity, "fire_ball.ent", 1);
		move(thisEntity);
		doCharacterCollision(thisEntity, true);
		animateCharacter(thisEntity, timer);
	}
}

void ETHCallback_master_knight(ETHEntity @thisEntity)
{
	g_numNpcs++;
	frameTimer @timer;

	// if it has no timer yet, instantiate a timer for this one
	if (!g_frameTimers.get("id" + thisEntity.GetID(), @timer))
	{
		frameTimer newTimer;
		newTimer.Set(thisEntity.GetFrame(), thisEntity.GetFrame(), 0);
		g_frameTimers.set("id" + thisEntity.GetID(), @newTimer);
		@timer = newTimer;
	}
	
	if (!isDead(thisEntity, "fade_out_beam_large.ent"))
	{
		applyForce(thisEntity, vector2(0,UnitsPerSecond(GRAVITY)));
		meleeCharacterAI(thisEntity, "dark_sword.ent");
		move(thisEntity);
		doCharacterCollision(thisEntity, true);
		animateCharacter(thisEntity, timer);
	}
}

void ETHCallback_paladin(ETHEntity @thisEntity)
{
	g_numNpcs++;
	frameTimer @timer;

	// if it has no timer yet, instantiate a timer for this one
	if (!g_frameTimers.get("id" + thisEntity.GetID(), @timer))
	{
		frameTimer newTimer;
		newTimer.Set(thisEntity.GetFrame(), thisEntity.GetFrame(), 0);
		g_frameTimers.set("id" + thisEntity.GetID(), @newTimer);
		@timer = newTimer;
	}
	
	if (!isDead(thisEntity, "fade_out_beam.ent"))
	{
		applyForce(thisEntity, vector2(0,UnitsPerSecond(GRAVITY)));
		meleeCharacterAI(thisEntity, "paladin_sword.ent");
		move(thisEntity);
		doCharacterCollision(thisEntity, true);
		animateCharacter(thisEntity, timer);
	}
}

void summoner(ETHEntity @thisEntity, const string creature,
			  const string summonOriginEntity, const uint interval)
{
	if (thisEntity.CheckCustomData("lastSummon") == DT_NODATA)
		thisEntity.AddUIntData("lastSummon", GetTime());
	
	if (GetTime()-thisEntity.GetUIntData("lastSummon") >= interval)
	{
		ETHEntityArray entities;
		if (thisEntity.GetUIntData("currentDir") == LEFT)
			GetEntitiesFromBucket(thisEntity.GetCurrentBucket()+vector2(-1,0), entities);
		else
			GetEntitiesFromBucket(thisEntity.GetCurrentBucket()+vector2(1,0), entities);
		
		const uint size = entities.size();
		for (uint t=0; t<size; t++)
		{
			if (entities[t].GetEntityName() == summonOriginEntity)
			{
				AddEntity("summon.ent", entities[t].GetPosition(), 0.0f);
				ETHEntity @handle;
				AddEntity(creature+".ent", entities[t].GetPosition()+vector3(0,0,-10)+vector3(randF(6),0,0), @handle);
				spawn(@handle, creature);
				break;
			}
		}
		thisEntity.AddUIntData("lastSummon", GetTime());
	}
}

void ETHCallback_king(ETHEntity @thisEntity)
{
	g_numNpcs++;
	frameTimer @timer;

	// if it has no timer yet, instantiate a timer for this one
	if (!g_frameTimers.get("id" + thisEntity.GetID(), @timer))
	{
		frameTimer newTimer;
		newTimer.Set(thisEntity.GetFrame(), thisEntity.GetFrame(), 0);
		g_frameTimers.set("id" + thisEntity.GetID(), @newTimer);
		@timer = newTimer;
	}

	if (!isDead(thisEntity, "fade_out_beam.ent"))
	{
		applyForce(thisEntity, vector2(0,UnitsPerSecond(GRAVITY)));
		mixedCharacterAI(thisEntity, "paladin_sword.ent", "fire_ball.ent", 100, 1.5f);
		move(thisEntity);
		doCharacterCollision(thisEntity, true);
		animateCharacter(thisEntity, timer);
		summoner(thisEntity, "warrior", "summon", 5000);
	}
	else
	{
		g_gameFinished = true;
	}
}

void player1Summoner(ETHEntity @player0)
{
	if (!hasASecondController())
		return;

	if (getConfirmButtonStatus(1) == KS_HIT)
	{
		if (player0.GetIntData("mp") <= 50)
		{
			g_messages.addMessage("É necessário de 50 mana para invocar a criatura");
			return;
		}
		
		ETHEntityArray visible;
		GetVisibleEntities(visible);
		{
			uint size = visible.size();
			for(uint t=0; t<size; t++)
			{
				if (visible[t].GetEntityName() == MAIN_CHARACTER_ENTITY1)
				{
					g_messages.addMessage("Não é possível invocar 2 criaturas ao mesmo tempo");
					return;
				}
			}
		}
		const vector2 currentBucket = player0.GetCurrentBucket();
		ETHEntityArray entitiesAround;
		GetEntitiesFromBucket(currentBucket, entitiesAround);
		GetEntitiesFromBucket(currentBucket+vector2(1,0), entitiesAround);
		collisionBox playerBox = getAbsoluteCollisionBox(player0);
		playerBox.pos += vector3(playerBox.size.x+1,-6, 0);
		uint size = entitiesAround.size();
		bool maySummon = true;
		for(uint t=0; t<size; t++)
		{
			if (!entitiesAround[t].Collidable())
				continue;
			collisionBox box = getAbsoluteCollisionBox(entitiesAround[t]);
			if (checkBoxHit(box, playerBox))
			{
				maySummon = false;
				break;
			}
		}
		if (maySummon)
		{
			addToMp(player0,-50);
			g_messages.addMessage("Criatura mágica invocada");
			AddEntity("summon.ent", playerBox.pos, 0.0f);
			AddEntity("princess.ent", playerBox.pos, 0.0f);
			g_lives--;
		}
		else
		{
			g_messages.addMessage("Impossível invocar criatura daqui");
		}
	}
}
