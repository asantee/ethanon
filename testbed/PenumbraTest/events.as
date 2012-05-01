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

void ETHCallback_event01(ETHEntity @thisEntity)
{
	ETHEntityArray entities;
	GetEntitiesFromBucket(thisEntity.GetCurrentBucket(), entities);
	const uint size = entities.size();
	for (uint t=0; t<size; t++)
	{
		if (isAMainCharacter(entities[t]))
		{
			StopSample("soundfx/fase.mp3");
			PlaySample("soundfx/laugh_king.mp3");
			PlaySample("soundfx/chefao.mp3");
			LoopSample("soundfx/chefao.mp3", true);
			AddEntity("summon.ent", thisEntity.GetPosition(), 0.0f);
			ETHEntity @handle;
			AddEntity("king.ent", thisEntity.GetPosition()+vector3(0,0,-10), @handle);
			spawn(@handle, "king");
			DeleteEntity(thisEntity);
			AddEntity("invisible_wall.ent", vector3(10112, 1408, 0), 0.0f);
		}
	}
}

const float SIZE_ADDITION = 6;
void ETHCallback_falling_bridge(ETHEntity @thisEntity)
{
	if (thisEntity.CheckCustomData("falling") == DT_NODATA)
	{
		ETHEntityArray entities;
		GetEntitiesFromBucket(thisEntity.GetCurrentBucket(), entities);
		GetEntitiesFromBucket(thisEntity.GetCurrentBucket()+vector2(0,-1), entities);
		collisionBox stoneBox = getAbsoluteCollisionBox(thisEntity);
		stoneBox.size.y += SIZE_ADDITION;
		stoneBox.size.x *= 0.9f;
		stoneBox.pos.y -= SIZE_ADDITION/2;
		const uint size = entities.size();
		for (uint t=0; t<size; t++)
		{
			if (isAMainCharacter(entities[t]))
			{
				collisionBox charBox = getAbsoluteCollisionBox(entities[t]);
				if (checkBoxHit(charBox, stoneBox))
				{
					thisEntity.AddFloatData("gravity", 0.0f);
					thisEntity.AddUIntData("falling", GetTime());
				}
			}
		}
	}
	else if (GetTime()-thisEntity.GetUIntData("falling") > 600)
	{
		thisEntity.SetCollision(false);
		const float gravity = thisEntity.GetFloatData("gravity");
		if (gravity == 0.0f)
			AddEntity("bridge_fall.ent", thisEntity.GetPosition()+vector3(0,0,-2), 0.0f);
		thisEntity.AddFloatData("gravity", gravity+UnitsPerSecond(GRAVITY));
		const float fps = GetFPSRate() == 0 ? 60 : GetFPSRate();
		thisEntity.AddToPositionXY(vector2(0,thisEntity.GetFloatData("gravity")/fps));
		if (!isInScreen(thisEntity))
		{
			DeleteEntity(thisEntity);
		}
	}
}
