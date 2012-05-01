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

Switch g_enablePS("Ativa pixel shaders", "Desativa pixel shaders");
Switch g_windowed("Janela", "Tela-cheia");
Switch g_controls("2º joystick para jogador 2", "interface/input_options1.png", "1º joystick para jogador 1", "interface/input_options2.png");

string videoModeToString(const videoMode vm)
{
	return ""+vm.width+"x"+vm.height+"x"+(vm.format == PF32BIT ? 32 : 16);
}

void screenModesPreLoop()
{
	loopMenuSong();
	LoadSprite("interface/arrow_button.png");
}

void ETHCallback_picker(ETHEntity @thisEntity)
{
	ETHInput @input = GetInputHandle();
	input.SetCursorPos(input.GetCursorAbsolutePos()+getPlayerXYAxis(0)*5);
	thisEntity.SetPositionXY(input.GetCursorPos());
}

bool putBackButton(const vector2 cursor)
{
	bool r = false;
	ETHInput @input = GetInputHandle();
	const vector2 mouseCursor = input.GetCursorPos();
	const vector2 spriteSize = GetSpriteSize("interface/arrow_button.png");
	uint8 alpha = 100;
	if (mouseCursor.x > cursor.x && mouseCursor.y > cursor.y
		&& mouseCursor.x < cursor.x+spriteSize.x && mouseCursor.y < cursor.y+spriteSize.y)
	{
		alpha = 255;
		if (getConfirmButtonStatus(0) == KS_HIT)
		{
			r = true;
		}
	}		
	DrawSprite("interface/arrow_button.png", cursor, ARGB(alpha, 203,203,228));
	return r;
}

void screenModesLoop()
{
	const vector2 titlePos(30,30);
	const vector2 origin(30,100);
	shadowText(titlePos, "Opções de vídeo", "Arial Narrow", 40.0f, 255,203,203,228);

	ETHInput @input = GetInputHandle();
	const vector2 mousePos(input.GetCursorPos());
	const vector2 cursorOrigin = origin;
	vector2 cursor = cursorOrigin;
	const float fontSize = 25;
	const float textWidth = 200.0f;
	for (uint t=0; t<GetVideoModeCount(); t++)
	{
		const videoMode mode = GetVideoMode(t);
		if (mode.format != PF32BIT || mode.width < 800 || mode.height < 600)
			continue;

		uint8 alpha = 100;
		if (mousePos.x > cursor.x && mousePos.x < cursor.x+textWidth
			&& mousePos.y > cursor.y && mousePos.y < cursor.y+fontSize)
		{
			alpha = 255;
			if (getConfirmButtonStatus(0) == KS_HIT)
			{
				SetWindowProperties(APPLICATION_TITLE, mode.width, mode.height, Windowed(), true, PF32BIT);
			}
		}

		shadowText(cursor, videoModeToString(mode), "Arial Narrow", fontSize, alpha, 203,203,228);

		cursor.y += fontSize;
		if (cursor.y > GetScreenSize().y)
		{
			cursor.x += textWidth;
			cursor.y = cursorOrigin.y;
		}
	}
	
	// adjust pixel shader states	
	g_enablePS.put(vector2(255, origin.y), "Arial Narrow", fontSize, 256);
	UsePixelShaders(g_enablePS.getCurrent() == 0);

	g_windowed.put(vector2(255, origin.y+70), "Arial Narrow", fontSize, 256);
	g_controls.put(vector2(255, origin.y+160), "Arial Narrow", fontSize, 256);

	showToggleFullscreenMessage();
	waitForInputToMenu();

	if(putBackButton(vector2(500, 40)))
		goToMenu();
}