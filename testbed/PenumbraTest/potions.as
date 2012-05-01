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

void doPotion(ETHEntity @thisEntity)
{
	ETHEntityArray entityArray;
	GetEntitiesFromBucket(thisEntity.GetCurrentBucket(), entityArray);
	for (uint t=0; t<entityArray.size(); t++)	
	{
		if (getDist(entityArray[t].GetPositionXY(), thisEntity.GetPositionXY()) < 20.0f)
		{
			if (isAMainCharacter(entityArray[t]))
			{
				if (entityArray[t].GetIntData("hp") >= entityArray[t].GetIntData("maxHp"))
					return;
				addToHp(entityArray[t], thisEntity.GetIntData("hp"));
				DeleteEntity(thisEntity);
				AddEntity("potion_pick.ent", thisEntity.GetPosition()+vector3(0,0,16), 0.0f);
			}
		}
	}
}

void ETHCallback_potion(ETHEntity @thisEntity)
{
	doPotion(thisEntity);
}

void ETHCallback_potion_small(ETHEntity @thisEntity)
{
	doPotion(thisEntity);
}

void ETHCallback_potion_large(ETHEntity @thisEntity)
{
	doPotion(thisEntity);
}