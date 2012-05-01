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

bool swordAttack(ETHEntity @thisEntity, const string swordName, const string beamName, const bool isInRange)
{
	if (!isInRange)
	{
		thisEntity.AddUIntData("lastTimeDidntSee", GetTime());
		return false;
	}
	
	if (thisEntity.GetUIntData("waitBeforeAttack") == 0)
	{
		if ((GetTime()-thisEntity.GetUIntData("lastSwordAttack")) < thisEntity.GetUIntData("coolDown"))
		{
			return false;
		}
	}
	else
	{
		if ((GetTime()-thisEntity.GetUIntData("lastTimeDidntSee")) < thisEntity.GetUIntData("coolDown"))
		{
			return false;
		}
	}
	
	vector3 pos = thisEntity.GetPosition();
	const vector2 force(0,0);// = getCurrentForce(thisEntity);
	vector3 swordPos = pos+vector3(force.x, force.y, 0);
	uint dir = thisEntity.GetUIntData("currentDir");
	if (dir == LEFT)
	{
		swordPos += vector3(-28,0,10);
	}
	else
	{
		swordPos += vector3(28,0,10);
	}

	ETHEntity @sword;
	ETHEntity @beam;
	AddEntity(swordName, swordPos+vector3(0,0,8), @sword);
	AddEntity(beamName, pos+vector3(0,0,8), @beam);
	if (dir == LEFT)
	{
		sword.MirrorParticleSystemX(0, true);
		beam.MirrorParticleSystemX(0, true);
	}
	sword.AddFloatData("ownerX", thisEntity.GetPositionXY().x);
	sword.AddFloatData("ownerY", thisEntity.GetPositionXY().y);
	sword.AddIntData("ownerID", thisEntity.GetID());
	sword.AddUIntData("direction", dir);
	sword.AddIntData("damage", thisEntity.GetIntData("damage"));
	
	if (isAMainCharacter(thisEntity))
	{
		sword.AddUIntData("mainCharacter", 1);
		if (thisEntity.CheckCustomData("pvpMode") != DT_NODATA)
		{
			sword.AddUIntData("pvpMode", 1);
		}
		sword.AddUIntData("playerId", thisEntity.GetUIntData("playerId"));
	}

	thisEntity.AddUIntData("lastTimeDidntSee", GetTime());
	thisEntity.AddUIntData("lastSwordAttack", GetTime());
	return true;
}

void ETHCallback_enemy_sword(ETHEntity @thisEntity)
{
	doDamage(thisEntity, 4, true, false, ATTACK_MODE_PHYSICAL, 1);
}

void ETHCallback_dark_sword(ETHEntity @thisEntity)
{
	doDamage(thisEntity, 12, true, false, ATTACK_MODE_PHYSICAL, 1);
}

//main character swords
void ETHCallback_sword0(ETHEntity @thisEntity)
{
	doDamage(thisEntity, 4, true, false, ATTACK_MODE_PHYSICAL, 1);
}
void ETHCallback_sword1(ETHEntity @thisEntity)
{
	doDamage(thisEntity, 4, true, false, ATTACK_MODE_PHYSICAL, 1);
}
void ETHCallback_combo_sword(ETHEntity @thisEntity)
{
	doDamage(thisEntity, 25, true, false, ATTACK_MODE_PHYSICAL, 5);
}

void ETHCallback_paladin_sword(ETHEntity @thisEntity)
{
	doDamage(thisEntity, 15, true, false, ATTACK_MODE_PHYSICAL, 1);
}
