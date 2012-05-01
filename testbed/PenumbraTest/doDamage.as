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

// knockFromOwner: if true, the "knock back" will push the target from the attacker's position
// instead of pushing from the hit entity effect
ETHEntity @doDamage(ETHEntity @thisEntity, const float knockForce, const bool knockFromOwner,
			  const bool collideEverything, const uint attackMode, const float damageMultiplier)
{
	ETHEntity @r = null;
	const uint dir = thisEntity.GetUIntData("direction");
	vector2 bucket = thisEntity.GetCurrentBucket();
	const bool mainChar = (thisEntity.CheckCustomData("mainCharacter") != DT_NODATA);
	
	ETHEntityArray entities;
	GetEntitiesFromBucket(bucket, entities);

	if (dir == LEFT)
	{
		GetEntitiesFromBucket(bucket+vector2(-1,0), entities);
		
		if (mainChar)
		{
			GetEntitiesFromBucket(bucket+vector2(0,1), entities);
			GetEntitiesFromBucket(bucket+vector2(0,-1), entities);
		}
	}
	else
	{
		GetEntitiesFromBucket(bucket+vector2(1,0), entities);
		
		if (mainChar)
		{
			GetEntitiesFromBucket(bucket+vector2(0,1), entities);
			GetEntitiesFromBucket(bucket+vector2(0,-1), entities);
		}
	}

	bool isResistant = false;
	bool pvpMode = (thisEntity.CheckCustomData("pvpMode") != DT_NODATA);
	
	
	int damage = 0;
	if (mainChar)
	{
		uint ownerPlayerId = MAX_PLAYERS;
		if (thisEntity.CheckCustomData("playerId") != DT_NODATA)
			ownerPlayerId = thisEntity.GetUIntData("playerId");
		damage = ((thisEntity.GetIntData("damage")) + int(float(thisEntity.GetIntData("damage"))*(float(g_charLevel[ownerPlayerId])/4))) * int(damageMultiplier);
	}
	else
	{
		damage = thisEntity.GetIntData("damage") * int(damageMultiplier);
	}
	
	const int ownerID = thisEntity.GetIntData("ownerID");

	collisionBox thisBox = getAbsoluteCollisionBox(thisEntity);
	const uint size = entities.size();	
	for (uint t=0; t<size; t++)
	{
		if (!entities[t].Collidable())
			continue;
		
		if (entities[t].GetID() == thisEntity.GetID())
			continue;
		
		if (thisEntity.GetIntData("ownerID") == entities[t].GetID())
			continue;
		
		// prevent two blows from the same character to hit each other
		if (entities[t].CheckCustomData("ownerID") != DT_NODATA)
			if (thisEntity.GetIntData("ownerID") == entities[t].GetIntData("ownerID"))
				continue;

		if (entities[t].CheckCustomData("hp") == DT_NODATA && !collideEverything)
			continue;

		if (collideEverything)
		{
			if (entities[t].GetEntityName() == "npc_wall.ent")
				continue;
		}
		
		// don't let friendly fire happen
		if (!pvpMode)
			if (isAMainCharacter(entities[t]) && thisEntity.CheckCustomData("mainCharacter") != DT_NODATA)
				continue;

		// don't let it hit friend's attacks
		if (!pvpMode)
			if (entities[t].CheckCustomData("mainCharacter") != DT_NODATA && thisEntity.CheckCustomData("mainCharacter") != DT_NODATA)
				continue;
		
		collisionBox box = getAbsoluteCollisionBox(entities[t]);

		if (checkBoxHit(box, thisBox))
		{
			@r = entities[t];
			entities[t].AddStringData("hitBy", thisEntity.GetEntityName());

			if (isAMainCharacter(entities[t]))
				g_camera.startEarthquake(min(10.0f, float(damage)));

			if (entities[t].CheckCustomData("hp") == DT_NODATA)
				continue;

			// adds a custom data whose name equals its id so we can easily
			// check what entities it has hit
			const string dataName = "id" + entities[t].GetID();
			if (thisEntity.CheckCustomData(dataName) == DT_NODATA)
			{
				@r = entities[t];
				
				// check it the target is resistant to this kind of blow
				if (entities[t].CheckCustomData("fireResistant") != DT_NODATA)
				{
					if (attackMode == ATTACK_MODE_FIRE && entities[t].GetUIntData("fireResistant") != 0)
					{
						isResistant = true;
					}
				}

				// give damage and attach attacker'ss ID to the target
				thisEntity.AddIntData(dataName, 0);
				damage = (isResistant) ? damage/5 : damage;
				addToHp(entities[t],-damage);
				g_messages.addMessage(-damage, entities[t].GetPositionXY());

				// if we're on pvp mode and the character died, assign a PVP point to the opponent
				if (entities[t].CheckCustomData("pvpMode") != DT_NODATA)
				{
					if (entities[t].GetIntData("hp") <= 0)
					{
						if (isAMainCharacter(entities[t]))
						{
							ETHEntity @attacker = SeekEntity(ownerID);
							if (isAMainCharacter(attacker))
							{
								if (attacker !is null)
								{
									const uint playerId = attacker.GetUIntData("playerId");
									g_pvpPoints[playerId]++;
								}
							}
							else
							{
								const uint playerId = entities[t].GetUIntData("playerId");
								g_pvpPoints[playerId]--;
							}
						}
						else
						{
							ETHEntity @attacker = SeekEntity(ownerID);
							if (isAMainCharacter(attacker))
							{
								const uint playerId = attacker.GetUIntData("playerId");
								addToExp(playerId, entities[t].GetIntData("expGiven"));
							}
							
						}
					}
				}

				// knock back
				vector2 ownerPos;
				if (knockFromOwner)
					ownerPos = vector2(thisEntity.GetFloatData("ownerX"), thisEntity.GetFloatData("ownerY"));
				else
					ownerPos = thisEntity.GetPositionXY();

				const vector2 backVector = normalize(entities[t].GetPositionXY()-ownerPos)*knockForce;
				float pushBackBias = (isResistant) ? 0.2f : 1;
				if (entities[t].CheckCustomData("pushBackBias") != DT_NODATA)
					pushBackBias = entities[t].GetFloatData("pushBackBias");

				knockBack(entities[t], backVector*pushBackBias);

				collisionBox hitBox = getAbsoluteCollisionBox(thisEntity);
				hitBox.pos = thisEntity.GetPosition()+vector3((dir == RIGHT) ? hitBox.size.x/2 : -hitBox.size.x/2,0,0);

				if (!isResistant)
				{
					if (thisEntity.CheckCustomData("hit") != DT_NODATA)
					{
						ETHEntity @particle;
						AddEntity(thisEntity.GetStringData("hit"), hitBox.pos, @particle);
						if (backVector.x > 0)
						{
							particle.MirrorParticleSystemX(0, true);
						}
					}
				}
				else
				{
					AddEntity("hit_fail.ent", hitBox.pos, 0.0f);
				}
				entities[t].AddUIntData("action", CHASING);
				//return r;
			}
		}
	}
	return r;
}

