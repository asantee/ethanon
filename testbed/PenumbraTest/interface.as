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

void drawPlayerStatus(ETHEntity @thisEntity)
{
	const float rail = 200.0f;
	const float height = 16.0f;
	const int maxHp = thisEntity.GetIntData("maxHp");
	const int maxMp = thisEntity.GetIntData("maxMp");
	const uint playerId = thisEntity.GetUIntData("playerId");
	const vector2 frameSize = GetSpriteSize("interface/frame.png");
	const vector2 idOffset(frameSize.x*float(playerId), 0);

	int nextExp = 0;
	g_gameData.getInt("global", "lv" + g_charLevel[playerId], nextExp);
	const int maxXp = nextExp;

	const int hp = thisEntity.GetIntData("hp");
	const int mp = thisEntity.GetIntData("mp");
	const float hpLength = (float(hp)/float(maxHp))*rail;
	const float mpLength = (float(mp)/float(maxMp))*rail;
	const float xpLength = (float(g_exp[playerId])/float(maxXp))*rail;
	const float textReturn = 45;
	DrawShapedSprite("interface/rail.png", idOffset+vector2(0,0), vector2(rail,height), 0xFFFFFFFF);
	DrawShapedSprite("interface/hp.png", idOffset+vector2(0,0), vector2(hpLength,height), 0xFFFFFFFF);
	DrawText(idOffset+vector2(hpLength-textReturn, 0), "hp: " + hp, "Arial Narrow", 16.0f, 0xD0000000);

	DrawShapedSprite("interface/rail.png", idOffset+vector2(0,16), vector2(rail,height), 0xFFFFFFFF);
	DrawShapedSprite("interface/mp.png", idOffset+vector2(0,16), vector2(mpLength,height), 0xFFFFFFFF);
	DrawText(idOffset+vector2(mpLength-textReturn, height), "mp: " + mp, "Arial Narrow", 16.0f, 0xD0000000);

	DrawShapedSprite("interface/rail.png", idOffset+vector2(0,32), vector2(rail,height), 0xFFFFFFFF);
	DrawShapedSprite("interface/xp.png", idOffset+vector2(0,32), vector2(xpLength,height), 0xFFFFFFFF);
	DrawText(idOffset+vector2(xpLength-(textReturn*0.6f), height*2), "lv: " + g_charLevel[playerId], "Arial Narrow", 16.0f, 0xD0000000);
	
	const uint textColor = ARGB(200,203,203,228);
	if (thisEntity.CheckCustomData("pvpMode") == DT_NODATA)
	{
		if (playerId == 0)
		{
			DrawSprite("interface/skull_interface.png", vector2(rail*2+48.0f,0), 0xFFFFFFFF);
			DrawText(vector2(rail*2+48.0f+21.5f,1.5f), "" + g_lives, "Arial Black", 17, 0xF0000000);
			DrawText(vector2(rail*2+48.0f+20,0), "" + g_lives, "Arial Black", 17, textColor);
		}
	}
	else
	{
		const vector2 skullPos = vector2(idOffset.x, frameSize.y)+vector2(16,16);
		DrawSprite("interface/skull_interface.png", skullPos, 0xFFFFFFFF);
		const string text = ""+g_pvpPoints[playerId];
		DrawText(skullPos+vector2(30,0), text, "Arial Black", 17, 0xF0000000);
		DrawText(skullPos+vector2(30,0), text, "Arial Black", 17, textColor);
	}

	DrawSprite("interface/frame.png", idOffset+vector2(0,0), 0xA0FFFFFF);
	DrawSprite("interface/blend.png", idOffset+vector2(0,0), textColor);
}

void doMpRecovery(ETHEntity @thisEntity, const uint stride, const int incr)
{
	if (thisEntity.CheckCustomData("lastMpIncr") == DT_NODATA)
	{
		thisEntity.AddUIntData("lastMpIncr", GetTime());
	}
	if (GetTime()-thisEntity.GetUIntData("lastMpIncr") > stride)
	{
		addToMp(thisEntity, incr);
		thisEntity.AddUIntData("lastMpIncr", GetTime());
	}
}