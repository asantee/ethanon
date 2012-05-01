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

uint g_gameOverStartTime = 0;

void gameOverPreLoop()
{
	LoadSoundEffect("soundfx/thunder.mp3");
	LoadSoundEffect("soundfx/laugh_king.mp3");
	LoadSoundEffect("soundfx/gameover.mp3");

	LoadSprite("entities/gameover.png");

	PlaySample("soundfx/laugh_king.mp3");
	PlaySample("soundfx/gameover.mp3");
	SetBackgroundColor(0xFF000000);
	SetAmbientLight(vector3(0,0,0));
	const vector2 screenMiddle = GetScreenSize()/2;
	AddEntity("bruxo_dead.ent", vector3(screenMiddle.x, screenMiddle.y, 0), 0.0f);
	g_gameOverStartTime = GetTime();
}

void gameOverLoop()
{
	const vector2 spriteSize = GetSpriteSize("entities/gameover.png");
	const vector2 screenMiddle = GetScreenSize()/2;
	DrawSprite("entities/gameover.png", vector2(screenMiddle.x-spriteSize.x/2, screenMiddle.y-200.0f), 0xFFFFFFFF);

	fadeIn(g_gameOverStartTime);
	waitForInputToMenu();
}