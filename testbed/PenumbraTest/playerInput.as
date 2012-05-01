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

uint getPlayerJoystick(const uint player)
{
	switch (player)
	{
		case 0:
			return (g_controls.getCurrent() == 0 ? 1 : 0);
		case 1:
			return (g_controls.getCurrent() == 0 ? 0 : 1);
	};
	return 2;
}

KEY_STATE getLeftButtonStatus(const uint player)
{
	ETHInput @input = GetInputHandle();
	
	if (player == 0)
	{
		if (input.KeyDown(K_LEFT))
		{
			return input.GetKeyState(K_LEFT);
		}
	}

	if (input.GetJoystickStatus(getPlayerJoystick(player)) == JS_DETECTED)
	{
		return input.JoyButtonState(getPlayerJoystick(player), JK_LEFT);
	}

	return KS_UP;
}

KEY_STATE getRightButtonStatus(const uint player)
{
	ETHInput @input = GetInputHandle();
	
	if (player == 0)
	{
		if (input.KeyDown(K_RIGHT))
		{
			return input.GetKeyState(K_RIGHT);
		}
	}

	if (input.GetJoystickStatus(getPlayerJoystick(player)) == JS_DETECTED)
	{
		return input.JoyButtonState(getPlayerJoystick(player), JK_RIGHT);
	}

	return KS_UP;
}

KEY_STATE getUpButtonStatus(const uint player)
{
	ETHInput @input = GetInputHandle();
	
	if (player == 0)
	{
		if (input.KeyDown(K_UP))
		{
			return input.GetKeyState(K_UP);
		}
	}

	if (input.GetJoystickStatus(getPlayerJoystick(player)) == JS_DETECTED)
	{
		return input.JoyButtonState(getPlayerJoystick(player), JK_UP);
	}

	return KS_UP;
}

KEY_STATE getDownButtonStatus(const uint player)
{
	ETHInput @input = GetInputHandle();
	
	if (player == 0)
	{
		if (input.KeyDown(K_DOWN))
		{
			return input.GetKeyState(K_DOWN);
		}
	}

	if (input.GetJoystickStatus(getPlayerJoystick(player)) == JS_DETECTED)
	{
		return input.JoyButtonState(getPlayerJoystick(player), JK_DOWN);
	}

	return KS_UP;
}

vector2 getPlayerXYAxis(const uint player)
{
	vector2 r(0,0);
	ETHInput @input = GetInputHandle();
	
	if (player == 0)
	{
		if (input.KeyDown(K_LEFT))
			r.x=-1;
		if (input.KeyDown(K_RIGHT))
			r.x=1;
		if (input.KeyDown(K_UP))
			r.y=-1;
		if (input.KeyDown(K_DOWN))
			r.y=1;
	}
	
	if (input.GetJoystickStatus(getPlayerJoystick(player)) == JS_DETECTED)
	{
		r += input.GetJoystickXY(getPlayerJoystick(player));
	}
	return r;
}

bool hasASecondController()
{
	ETHInput @input = GetInputHandle();
	return ((g_controls.getCurrent() == 1 && input.GetJoystickStatus(1) == JS_DETECTED)
			|| (g_controls.getCurrent() == 0 && input.GetJoystickStatus(0) == JS_DETECTED));
}

uint getInputDirection(const uint player)
{
	vector2 xy = getPlayerXYAxis(player);
	if (xy.x > 0)
		return RIGHT;
	else if (xy.x < 0)
		return LEFT;
	return DOWN;
}

KEY_STATE getJumpButtonStatus(const uint player)
{
	ETHInput @input = GetInputHandle();
	
	if (player == 0)
	{
		if (input.KeyDown(K_UP))
		{
			return input.GetKeyState(K_UP);
		}
		
		if (input.KeyDown(K_CTRL))
		{
			return input.GetKeyState(K_CTRL);
		}
	}

	if (input.GetJoystickStatus(getPlayerJoystick(player)) == JS_DETECTED)
	{
		return input.JoyButtonState(getPlayerJoystick(player), JK_03);
	}

	return KS_UP;
}

KEY_STATE getAttack01ButtonStatus(const uint player)
{
	ETHInput @input = GetInputHandle();
	
	if (player == 0)
	{
		if (input.KeyDown(K_S))
		{
			return input.GetKeyState(K_S);
		}
	}

	if (input.GetJoystickStatus(getPlayerJoystick(player)) == JS_DETECTED)
	{
		return input.JoyButtonState(getPlayerJoystick(player), JK_04);
	}

	return KS_UP;
}

KEY_STATE getAttack02ButtonStatus(const uint player)
{
	ETHInput @input = GetInputHandle();
	
	if (player == 0)
	{
		if (input.KeyDown(K_D))
		{
			return input.GetKeyState(K_D);
		}
	}

	if (input.GetJoystickStatus(getPlayerJoystick(player)) == JS_DETECTED)
	{
		return input.JoyButtonState(getPlayerJoystick(player), JK_02);
	}

	return KS_UP;
}

KEY_STATE getAttack03ButtonStatus(const uint player)
{
	ETHInput @input = GetInputHandle();
	
	if (player == 0)
	{
		if (input.KeyDown(K_SPACE))
		{
			return input.GetKeyState(K_SPACE);
		}
	}

	if (input.GetJoystickStatus(getPlayerJoystick(player)) == JS_DETECTED)
	{
		return input.JoyButtonState(getPlayerJoystick(player), JK_01);
	}

	return KS_UP;
}

KEY_STATE getConfirmButtonStatus(const uint player)
{
	ETHInput @input = GetInputHandle();
	
	if (player == 0)
	{
		if (input.KeyDown(K_RETURN) && input.GetKeyState(K_ALT) == KS_UP)
		{
			return input.GetKeyState(K_RETURN);
		}
		if (input.KeyDown(K_LMOUSE))
		{
			return input.GetKeyState(K_LMOUSE);
		}
		if (input.KeyDown(K_RMOUSE))
		{
			return input.GetKeyState(K_RMOUSE);
		}
	}
	
	if (input.GetJoystickStatus(getPlayerJoystick(player)) == JS_DETECTED)
	{
		return input.JoyButtonState(getPlayerJoystick(player), JK_10);
	}

	return KS_UP;
}

KEY_STATE getCancelButtonStatus(const uint player)
{
	ETHInput @input = GetInputHandle();
	
	if (player == 0)
	{
		if (input.KeyDown(K_ESC))
		{
			return input.GetKeyState(K_ESC);
		}
	}

	if (input.GetJoystickStatus(getPlayerJoystick(player)) == JS_DETECTED)
	{
		return input.JoyButtonState(getPlayerJoystick(player), JK_09);
	}

	return KS_UP;
}

void controlCharacter(ETHEntity @thisEntity, const uint player)
{
	ETHInput @input = GetInputHandle();
	const float speed = thisEntity.GetFloatData("speed");
	const bool touchingGround = (thisEntity.GetUIntData("touchingGround") != 0);
	
	// jump
	if (getJumpButtonStatus(player) == KS_HIT)
	{
		const int numJumps = thisEntity.GetIntData("jumps");
		
		// it can't jump in the air if it has no mana
		const bool mayJump = (!touchingGround && thisEntity.GetIntData("mp") < 4) ? false : true;
		if (numJumps < thisEntity.GetIntData("maxJumps") && mayJump)
		{
			if (mayJump && !touchingGround)
			{
				addToMp(thisEntity,-4);
			}

			setForceY(thisEntity,-thisEntity.GetFloatData("jumpForce"));
			thisEntity.AddIntData("jumps", numJumps+1);
			if (!touchingGround)
			{
				g_camera.startEarthquake(2.5f);
				PlaySample("soundfx/jump0" + (player+1) + ".ogg");
			}
		}
	}

	if (getInputDirection(player) == LEFT)
	{
		thisEntity.AddUIntData("currentDir", LEFT);
		setForceX(thisEntity,-speed);
	}
	else if (getInputDirection(player) == RIGHT)
	{
		thisEntity.AddUIntData("currentDir", RIGHT);
		setForceX(thisEntity, speed);
	}
	else
	{
		setForceX(thisEntity, 0);
	}

	bool didSwordCombo = false;
	bool didSpellCombo = false;
	
	// manage combos
	g_comboManager[player].updateInput(player);
	
	// sword combo
	if (g_comboManager[player].checkSequence(CMD_LEFT, CMD_LEFT, CMD_SWORD)
		|| g_comboManager[player].checkSequence(CMD_RIGHT, CMD_RIGHT, CMD_SWORD))
	{
		if (thisEntity.GetIntData("mp") >= 5)
		{
			addToMp(thisEntity,-5);
			swordAttack(thisEntity, "combo_sword.ent", "sword_beam.ent", true);
			if (thisEntity.GetFloatData("forceY") > 0)
				thisEntity.AddFloatData("forceY", 0.0f);
			didSwordCombo = true;
			g_camera.startEarthquake(7.0f);
		}
		else
		{
			g_messages.addMessage("É necessário ter 5 de mana para realizar este combo");
		}
	}

	// spell combo
	if (g_comboManager[player].checkSequence(CMD_DOWN, CMD_LEFT, CMD_SPELL)
		|| g_comboManager[player].checkSequence(CMD_DOWN, CMD_RIGHT, CMD_SPELL))
	{
		const int mana = 25;
		if (castSpell(thisEntity, "combo_fire_ball.ent", mana, vector3(0,0,0), 225, 1))
		{
			PlaySample("soundfx/blast_attack.ogg");
			if (thisEntity.GetFloatData("forceY") > 0)
				thisEntity.AddFloatData("forceY", 0.0f);
			didSpellCombo = true;
		}
		else
		{
			g_messages.addMessage("É necessário ter " + mana + " mana para este combo");
		}
	}

	// attack
	if (getAttack01ButtonStatus(player) == KS_HIT
		&& (GetTime()-thisEntity.GetUIntData("lastSwordAttack")) > thisEntity.GetUIntData("coolDown")
		&& !didSwordCombo)
	{
		swordAttack(thisEntity, "sword" + player + ".ent", "sword_beam.ent", true);
		if (thisEntity.GetFloatData("forceY") > 0)
			thisEntity.AddFloatData("forceY", 0.0f);
	}		
	
	// light spell
	if (getAttack03ButtonStatus(player) == KS_HIT)
	{
		if (!g_castingLight[player])
		{
			const int mana = 50;
			if (castSpell(thisEntity, "light_spell.ent", mana, vector3(0,-40,14), 0, 1))
				PlaySample("soundfx/light_spell.mp3");
			else
				g_messages.addMessage("É necessário ter " + mana + " mana para esta magia");
		}
		else
		{
			g_messages.addMessage("Esta mágica já está em execução");
		}
	}

	// fire spell
	if (getAttack02ButtonStatus(player) == KS_HIT && !didSpellCombo)
	{
		const int mana = 10;
		if (castSpell(thisEntity, "fire_ball.ent", mana, vector3(0,0,0), 30, 1))
		{
			PlaySample("soundfx/cast_fire_spell.ogg");
			if (thisEntity.GetFloatData("forceY") > 0)
				thisEntity.AddFloatData("forceY", 0.0f);
		}
		else
		{
			g_messages.addMessage("É necessário ter " + mana + " mana para esta magia");
		}
	}		
	g_castingLight[player] = false;
}

