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

void ETHCallback_shooter(ETHEntity @thisEntity)
{
	if (thisEntity.CheckCustomData("lastShoot") == DT_NODATA)
	{
		thisEntity.AddUIntData("lastShoot", GetTime());
		thisEntity.AddUIntData("coolDown", 1000+rand(thisEntity.GetUIntData("coolDown")));
	}
	if (GetTime()-thisEntity.GetUIntData("lastShoot") >= thisEntity.GetUIntData("coolDown"))
	{
		vector3 pos = thisEntity.GetPosition();
		ETHEntity @handle;
		AddEntity("fire_shoot.ent", pos+vector3(0,0,15), @handle);
		handle.AddFloatData("force", 500+randF(thisEntity.GetFloatData("force")));
		handle.AddFloatData("ownerX", thisEntity.GetPositionXY().x);
		handle.AddFloatData("ownerY", thisEntity.GetPositionXY().y);
		handle.AddIntData("damage", 18);
		thisEntity.AddUIntData("lastShoot", GetTime());
		PlaySample("soundfx/cast_fire_spell.ogg");
	}
}

void ETHCallback_fire_shoot(ETHEntity @thisEntity)
{
	float force = thisEntity.GetFloatData("force");
	thisEntity.AddFloatData("force", force-(UnitsPerSecond(GRAVITY))*0.6f);
	force = thisEntity.GetFloatData("force");
	
	const float fps = GetFPSRate() == 0 ? 60 : GetFPSRate();
	thisEntity.AddToPositionXY(vector2(0,(-force/fps)*0.6f));
	
	const vector2 bucket = thisEntity.GetCurrentBucket();
	ETHEntityArray entities;
	GetEntitiesFromBucket(bucket, entities);
	GetEntitiesFromBucket(bucket+vector2(0,1), entities);
	GetEntitiesFromBucket(bucket+vector2(0,-1), entities);

	const vector2 ownerPos = vector2(thisEntity.GetFloatData("ownerX"), thisEntity.GetFloatData("ownerY"));
	collisionBox thisBox = getAbsoluteCollisionBox(thisEntity);

	const uint size = entities.size();
	for (uint t=0; t<size; t++)
	{
		if (!entities[t].Collidable())
			continue;

		if (entities[t].GetID() == thisEntity.GetID())
			continue;

		if (entities[t].GetEntityName() == thisEntity.GetEntityName())
			continue;

		if (entities[t].GetEntityName() == "instant_death.ent" ||
			entities[t].GetEntityName() == "instant_death2.ent" ||
			entities[t].GetEntityName() == "instant_death3.ent")
			continue;

		if (entities[t].GetEntityName() == "npc_wall.ent")
			continue;

		collisionBox box = getAbsoluteCollisionBox(entities[t]);

		if (checkBoxHit(box, thisBox))
		{
			if (entities[t].CheckCustomData("hp") != DT_NODATA)
			{
				addToHp(entities[t],-thisEntity.GetIntData("damage"));

				const vector2 backVector = normalize(entities[t].GetPositionXY()-ownerPos)*15;
				float pushBackBias = 1;
				if (entities[t].CheckCustomData("pushBackBias") != DT_NODATA)
					pushBackBias = entities[t].GetFloatData("pushBackBias");
				knockBack(entities[t], backVector*pushBackBias);
			}
			AddEntity("explosion.ent", thisEntity.GetPosition(), 0.0f);
			g_camera.startEarthquake(20);
			DeleteEntity(thisEntity);
			return;
		}
	}

	if (thisEntity.GetPositionXY().y > ownerPos.y)
	{
		DeleteEntity(thisEntity);
		AddEntity("lava_drops.ent", vector3(ownerPos.x, ownerPos.y-thisBox.size.y, thisEntity.GetPositionXY().y), 0.0f);
	}
}
