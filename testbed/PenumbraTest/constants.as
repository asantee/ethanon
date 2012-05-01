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

const float PI = 3.141592654f;
const float PI2 = 3.141592654f*2;
const float PIb = 1.570796327f;

const string endl = "\n";
const string APPLICATION_TITLE = "Penumbra e o Castelo das Sombras - Ethanon Engine";

const float GRAVITY = 1200.0f;

const uint STANDING = 0;
const uint CHASING = 1;
const uint COOLDOWN = 2;

const uint DEAD_FADE_OUT_TIME = 3000;
const uint LIVE_FADE_IN_TIME = 3000;

const uint PVP_MODE = 1;
const uint CAMPAIGN = 2;

const float DEFAULT_CHARSPRITE_CUTX = 4.0f;
const float DEFAULT_CHARSPRITE_CUTY = 4.0f;

const float SIZE_TOLERANCE = 76;

const uint ATTACK_MODE_PHYSICAL = 0;
const uint ATTACK_MODE_FIRE = 1;

const uint MAX_PLAYERS = 2;
const int MAX_PVP_POINTS = 3;

string actionToString(const uint action)
{
	switch (action)
	{
	case STANDING:
		return "STANDING";
	case CHASING:
		return "CHASING";
	case COOLDOWN:
		return "COOLDOWN";
	}
	return "";
}

const string MAIN_CHARACTER_ENTITY0 = "bruxo.ent";
const string MAIN_CHARACTER_ENTITY1 = "princess.ent";

bool isAMainCharacter(ETHEntity @thisEntity)
{
	if (thisEntity.GetEntityName() == MAIN_CHARACTER_ENTITY0)
		return true;
	if (thisEntity.GetEntityName() == MAIN_CHARACTER_ENTITY1)
		return true;
	return false;
}