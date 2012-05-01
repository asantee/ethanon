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

void rangedCharacterAI(ETHEntity @thisEntity, const string effectEntity, const float speedMultiplier)
{
	if (thisEntity.CheckCustomData("lastRangedAttack") == DT_NODATA)
	{
		thisEntity.AddUIntData("lastRangedAttack", GetTime());
	}

	const float speed = (thisEntity.GetFloatData("speed"));
	const uint action = thisEntity.GetUIntData("action");
	const float viewRadius = thisEntity.GetFloatData("viewRadius");
	const float attackRadius = thisEntity.GetFloatData("attackRadius");
	ETHEntity @mainChar = findMainCharNeighbours(thisEntity, true);
	if (mainChar !is null)
	{
		if (mainChar.GetIntData("hp") <= 0)
			@mainChar = null;
	}

	vector2 pos = thisEntity.GetPositionXY();
	
	float dist = 0;

	if (mainChar !is null)
	{
		dist = getDist(thisEntity.GetPositionXY(), mainChar.GetPositionXY());
	}

	if (mainChar !is null)
	{
		const vector2 mainCharPos = mainChar.GetPositionXY();

		bool running = false;
		// if our character is too close, run like hell
		if (dist < attackRadius)
		{
			if (pos.x < mainCharPos.x)
			{
				thisEntity.AddUIntData("currentDir", LEFT);
				setForceX(thisEntity,-speed);
			}
			else
			{
				thisEntity.AddUIntData("currentDir", RIGHT);
				setForceX(thisEntity, speed);
			}
			running = true;
		}
		else
		{
			if (pos.x > mainCharPos.x)
			{
				thisEntity.AddUIntData("currentDir", LEFT);
			}
			else
			{
				thisEntity.AddUIntData("currentDir", RIGHT);
			}
			setForceX(thisEntity, 0);
		}
		if (!running)
		{
			if (isOnSight(@thisEntity, @mainChar, 1.0f))
			{
				if (GetTime()-thisEntity.GetUIntData("lastRangedAttack") > thisEntity.GetUIntData("coolDown"))
				{
					castSpell(thisEntity, effectEntity, 0, vector3(0,0,0), thisEntity.GetIntData("damage"), speedMultiplier);
					thisEntity.AddUIntData("lastRangedAttack", GetTime());
					PlaySample("soundfx/cast_fire_spell.ogg");
				}
			}
		}
	}
}

void meleeCharacterAI(ETHEntity @thisEntity, const string swordEffect)
{
	const float speed = (thisEntity.GetFloatData("speed"));
	const uint action = thisEntity.GetUIntData("action");
	const float viewRadius = thisEntity.GetFloatData("viewRadius");
	const float attackRadius = thisEntity.GetFloatData("attackRadius");
	ETHEntity @mainChar = findMainCharNeighbours(thisEntity, true);
	if (mainChar !is null)
	{
		if (mainChar.GetIntData("hp") <= 0)
			@mainChar = null;
	}
	
	vector2 pos = thisEntity.GetPositionXY();
	
	float dist = 0;

	if (mainChar !is null)
	{
		dist = getDist(thisEntity.GetPositionXY(), mainChar.GetPositionXY());
	}
	
	if (dist >= viewRadius*2 || mainChar is null)
	{
		thisEntity.AddUIntData("action", STANDING);
	}

	if (action == STANDING)
	{
		setForceX(thisEntity, 0);
		if (mainChar !is null)
		{
			if (dist < viewRadius)
			{
				thisEntity.AddUIntData("action", CHASING);
				if (thisEntity.CheckCustomData("chaseSfx") != DT_NODATA)
				{
					const string sample = thisEntity.GetStringData("chaseSfx");
					if (!IsSamplePlaying(sample))
					{
						PlaySample(sample);
					}
				}
			}
		}
	}
	else if (action == CHASING)
	{
		if (mainChar !is null)
		{
			const vector2 mainCharPos = mainChar.GetPositionXY();

			// chase the main character
			if (dist > attackRadius && isOnSight(thisEntity, mainChar, 2.0f)
				&& abs(pos.x-mainCharPos.x) > 10 && getLength(getKnockBackVector(thisEntity)) == 0)
			{
				if (pos.x < mainCharPos.x)
				{
					thisEntity.AddUIntData("currentDir", RIGHT);
					setForceX(thisEntity, speed);
				}
				else
				{
					thisEntity.AddUIntData("currentDir", LEFT);
					setForceX(thisEntity,-speed);
				}
			}
			else
			{
				setForceX(thisEntity, 0);
			}
			if (swordAttack(thisEntity, swordEffect, "enemy_sword_beam.ent", (dist <= attackRadius)))
			{
				if (thisEntity.GetUIntData("jumpBackAfterAttack") != 0)
				{
					const uint dir = thisEntity.GetUIntData("currentDir");
					const float jumpBack = (dir == LEFT) ? 12.0f : -12.0f;
					knockBack(thisEntity, vector2(jumpBack,-12));
				}
			}
		}
	}
}

void mixedCharacterAI(ETHEntity @thisEntity, const string meleeFx, const string rangedFx, const float distance,
					  const float speedMultiplier)
{
	ETHEntity @mainChar = findMainCharNeighbours(thisEntity, true);
	if (mainChar !is null)
	{
		if (getDist(mainChar.GetPositionXY(), thisEntity.GetPositionXY()) > distance)
		{
			rangedCharacterAI(thisEntity, rangedFx, speedMultiplier);
		}
		else
		{
			meleeCharacterAI(thisEntity, meleeFx);
		}
	}
}
