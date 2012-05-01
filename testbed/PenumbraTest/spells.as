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

bool castSpell(ETHEntity @owner, const string spellName, const int mana, const vector3 spellOffset,
			   const int damage, const float speedMultiplier)
{
	// if there's not enough mana...
	if (owner.CheckCustomData("mp") != DT_NODATA)
	{
		if (owner.GetIntData("mp") < mana)
			return false;
		else
			addToMp(@owner,-mana);
	}

	ETHEntity @handle;
	AddEntity(spellName, owner.GetPosition()+spellOffset, @handle);
	handle.AddFloatData("spellOffsetX", spellOffset.x);
	handle.AddFloatData("spellOffsetY", spellOffset.y);
	handle.AddFloatData("speed", owner.GetFloatData("speed")*speedMultiplier);
	handle.AddUIntData("direction", owner.GetUIntData("currentDir"));
	handle.AddFloatData("ownerX", owner.GetPositionXY().x);
	handle.AddFloatData("ownerY", owner.GetPositionXY().y);
	handle.AddIntData("ownerID", owner.GetID());
	handle.AddIntData("damage", damage);
	
	if (owner.CheckCustomData("playerId") != DT_NODATA)
	{
		handle.AddUIntData("playerId", owner.GetUIntData("playerId"));
	}

	if (isAMainCharacter(owner))
	{
		handle.AddUIntData("mainCharacter", 1);
		if (owner.CheckCustomData("pvpMode") != DT_NODATA)
		{
			handle.AddUIntData("pvpMode", 1);
		}
	}
	
	return true;
}

void ETHCallback_light_spell(ETHEntity @thisEntity)
{
	g_castingLight[thisEntity.GetUIntData("playerId")] = true;
	vector2 bucket = thisEntity.GetCurrentBucket();
	ETHEntityArray entitiesAround;
	GetEntitiesFromBucket(bucket, entitiesAround);
	GetEntitiesFromBucket(bucket+vector2(0,1), entitiesAround);
	GetEntitiesFromBucket(bucket+vector2(1,1), entitiesAround);
	GetEntitiesFromBucket(bucket+vector2(-1,1), entitiesAround);
	GetEntitiesFromBucket(bucket+vector2(1,0), entitiesAround);
	GetEntitiesFromBucket(bucket+vector2(-1,0), entitiesAround);

	vector2 offset;
	offset.x = thisEntity.GetFloatData("spellOffsetX");
	offset.y = thisEntity.GetFloatData("spellOffsetY");
	
	const uint size = entitiesAround.size();
	for (uint t=0; t<size; t++)
	{
		if (isAMainCharacter(entitiesAround[t]))
		{
			if (thisEntity.GetIntData("ownerID") == entitiesAround[t].GetID())
			{
				thisEntity.SetPositionXY(entitiesAround[t].GetPositionXY()+offset);
				break;
			}
		}
	}
}

void ETHCallback_fire_ball(ETHEntity @thisEntity)
{
	manageFireBall(thisEntity, "explosion.ent");
}

void ETHCallback_combo_fire_ball(ETHEntity @thisEntity)
{
	manageFireBall(thisEntity, "big_explosion.ent");
}

void manageFireBall(ETHEntity @thisEntity, const string explosion)
{
	vector2 dir(1,0);
	if (thisEntity.GetUIntData("direction") == LEFT)
	{
		dir.x =-1;
	}
	thisEntity.AddToPositionXY(dir*UnitsPerSecond(thisEntity.GetFloatData("speed"))*1.5f);
	ETHEntity @target = doDamage(thisEntity, 15, false, true, ATTACK_MODE_FIRE, 1);
	if (target !is null)
	{
		bool explode = true;
		if (target.CheckCustomData("fireResistant") != DT_NODATA)
			if (target.GetUIntData("fireResistant") != 0)
				explode = false;
		
		if (explode)
		{
			AddEntity(explosion, thisEntity.GetPosition(), 0.0f);
			g_camera.startEarthquake(20);
		}
		DeleteEntity(thisEntity);
		return;
	}
	if (thisEntity.CheckCustomData("hitBy") != DT_NODATA)
	{
		DeleteEntity(thisEntity);
		return;
	}
}
