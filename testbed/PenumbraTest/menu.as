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

void loopMenuSong()
{
	LoadMusic("soundfx/menu.mp3");
	PlaySample("soundfx/menu.mp3");
	LoopSample("soundfx/menu.mp3", true);
}

void menuPreLoop()
{
	loopMenuSong();
	
	LoadSoundEffect("soundfx/help.mp3");
	SetSampleVolume("soundfx/help.mp3", 0.3f);
	
	LoadSoundEffect("soundfx/newgame.mp3");
	LoadSoundEffect("soundfx/fail.ogg");
	
	LoadSprite("interface/joystick.png");

	SetBorderBucketsDrawing(false);

	drawRect(0xFF000000);
	
	ETHEntity @cursor = SeekEntity("cursor.ent");
	if (cursor !is null)
	{
		cursor.AddStringData("lastButton", "none");
		cursor.AddUIntData("menuStartTime", GetTime());
	}
}

void detectJoysticks()
{
	const vector2 screenSize = GetScreenSize();
	const vector2 joySpriteSize = GetSpriteSize("interface/joystick.png");
	ETHInput @input = GetInputHandle();
	input.DetectJoysticks();
	if (input.GetJoystickStatus(0) == JS_DETECTED)
	{
		DrawSprite("interface/joystick.png", vector2(screenSize.x-joySpriteSize.x, 0),
				   ARGB(150,255,255,255));
	}
	if (input.GetJoystickStatus(1) == JS_DETECTED)
	{
		DrawSprite("interface/joystick.png", vector2(screenSize.x-joySpriteSize.x, joySpriteSize.y),
				   ARGB(150,255,255,255));
	}
}

void menuLoop()
{
	waitForInputToMenu();
}

void showToggleFullscreenMessage()
{
	const vector2 screenSize = GetScreenSize();
	ETHInput @input = GetInputHandle();
	shadowText(vector2(0,screenSize.y-15), "Pressione Alt+Enter para trocar entre fullscreen e modo janela", "Arial Narrow", 15.0f, 255,203,203,228);
	
	bool toggle = false;
	if ((Windowed() && g_windowed.getCurrent() == 1)
		|| !Windowed() && g_windowed.getCurrent() == 0)
		toggle = true;
	
	if (input.GetKeyState(K_ALT) == KS_DOWN && input.GetKeyState(K_RETURN) == KS_HIT || toggle)
	{
		const bool windowed = !Windowed();
		SetWindowProperties(APPLICATION_TITLE, uint(screenSize.x), uint(screenSize.y), windowed, true, PF32BIT);
		if (windowed)
		{
			g_windowed.setCurrent(0);
		}
		else
		{
			g_windowed.setCurrent(1);
		}
	}
}

const string como_jogar = """
Controles
 Movimento: < e >
 Pulo: ^ ou CTRL (joystick 3)
 Ataque/espada: S (joystick 4)
 Ataque/fogo: D (joystick 2)
 Acionar Luz: ESPAÇO (joystick 1)

 Detectar joysticks: segure J

2 jogadores:
 Se houver um joystick plugado,
 o personagem principal ganha 
 a habilidade de invocar a
 criatura que pode ser controlada
 pelo 2º jogador.

 Para invocar a criatura basta pres-
 sionar START no 2º controle.

 A mágica de invocação custa 50 mana
 e uma vida, mas possibilitará que 2
 jogadores lutem lado-a-lado.

 Somente o jogador 1 pode pegar
 checkpoints e passar de fase.
""";

const string config = """
Ajuste as opções de vídeo como
resolução de tela e uso de shaders.

Configure também o uso do joystick
pelo segundo jogador.
""";

const string creditos = """
André Santee
 -Programação
 -Scripting e mecânica do jogo
 -Efeitos especiais
 -Game design

Arthur Santee
 -Modelagem 3D
 -Game design

Gabriel Duarte
 -Trilha sonora
  gabrielduarte.wordpress.com

Approaching Thunderstorm
 -www.freesoundtrackmusic.com

Agradecimentos especiais:
-James Hastings-Trew por ter nos
 cedido algumas de suas texturas
  planetpixelemporium.com
-José Rodolfo Ortale
-Rafael "Pet" Alencar
-Taina Monclaire
""";

const string novo_jogo = """
Penumbra não é um bom lugar
para se viver. Quando acaba a
neblina, chega a tempestade.

Eras atrás, um mago muito
poderoso tomou o controle da
sombria terra chamada Penumbra,
nomeando-se o rei deste mundo.

Por diversos séculos, a Ordem dos
Bruxos de Penumbra têm buscado
derrubar o tirano.

Após incontáveis anos treinando
as artes da mágica e da luta você
foi escolhido pela Ordem para 
ir ao Castelo das Sombras
de Penumbra e assassinar o rei,
acabando com um reinado que dura
anos.
""";

const string versus = 
"Escolha uma arena e dispute uma\n"
"partida contra outro jogador.\n\n"
"-Quem derrotar o outro ganha 1 ponto\n"
"-Vence quem fizer " + MAX_PVP_POINTS + " pontos primeiro\n"
"-A partida acaba se a diferença no\n"
"placar exceder " + MAX_PVP_POINTS + " pontos";

void showData(const string title, const string content)
{
	const vector2 screenSize = GetScreenSize();
	const vector2 rectSize(screenSize.x*(1-0.618f), screenSize.y);
	const uint rectColor0 = ARGB(190,0,0,0);
	const uint rectColor1 = ARGB(55,0,0,0);
	const vector2 rectPos(screenSize.x-rectSize.x, 0);

	const float textSize = screenSize.y <= 700 ? 20.0f : 25.0f;
	DrawRectangle(rectPos, rectSize,
				  rectColor0, rectColor0, rectColor1, rectColor1);
	shadowText(rectPos+vector2(10,20), title, "Arial Narrow", 40.0f, 255,203,203,228);
	shadowText(rectPos+vector2(10,70), content, "Arial Narrow", textSize, 255,203,203,228);
}

void ETHCallback_cursor(ETHEntity @thisEntity)
{
	detectJoysticks();

	const vector2 screenSize = GetScreenSize();
	ETHInput @input = GetInputHandle();
	const vector2 cursorPos = input.GetCursorPos();
	input.SetCursorPos(input.GetCursorAbsolutePos()+getPlayerXYAxis(0)*5);
	thisEntity.SetPositionXY(cursorPos);
	
	ETHEntity @handle;
	
	if (thisEntity.CheckCustomData("newGame") == DT_NODATA)
	{
		if (CollideDynamic(thisEntity, handle))
		{
			const string entityName = handle.GetEntityName();
			const bool confirmed = getConfirmButtonStatus(0) == KS_HIT;
			if (entityName == "creditos")
			{
				showData("Créditos", creditos);
			} else
			if (entityName == "melhores_tempos")
			{
				showData("Melhores tempos", getRecordTimeList());
			} else
			if (entityName == "como_jogar")
			{
				showData("Como Jogar", como_jogar);
			} else
			if (entityName == "versus")
			{
				if (hasASecondController())
				{
					showData("Jogador versus Jogador", versus);
					if (confirmed)
					{
						goToPvp();
					}
				}
				else
				{
					if (confirmed)
					{
						PlaySample("soundfx/fail.ogg");
					}
					string message = "É necessário ao menos um joystick\n para jogar neste modo.";
					if (input.GetJoystickStatus(0) == JS_DETECTED)
					{
						message += endl + endl + "Já há um joystick plugado." + endl
						+ "Mude as opções de entrada no menu" + endl + "de configurações para poder" + endl
						+ "utilizar o teclado e o joystick" + endl + "por 2 jogadores.";
					}
					showData("Jogador versus Jogador", message);
				}
			} else
			if (entityName == "novo_jogo")
			{
				showData("Novo jogo", novo_jogo);
				if (confirmed)
				{
					thisEntity.AddUIntData("newGame", GetTime());
					thisEntity.AddStringData("scene", "CAMPAIGN");
					PlaySample("soundfx/newgame.mp3");
				}
			} else
			if (entityName == "sair")
			{
				showData("Sair do jogo", "");
				if (confirmed)
					Exit();
			} else
			if (entityName == "opcoes_de_video")
			{
				showData("Configurações", config);
				if (confirmed)
				{
					LoadScene("scenes/videoModes.esc", "screenModesPreLoop", "screenModesLoop");
				}
			} else if (entityName == "thumbnail")
			{
				const string number = handle.GetStringData("name");
				const string title = handle.GetStringData("title");
				string extra = "";
				bool allow = true;
				
				if (handle.CheckCustomData("score") != DT_NODATA)
				{
					if (handle.GetUIntData("score") <= getGetBestTime())
					{
						allow = false;
						extra = "\n\nÉ necessário terminar o jogo\nem menos de " +
							getTimeString(handle.GetUIntData("score")) + " para\nliberar esta arena.";
					}
				}
				
				showData(title, g_gameData.get("global", "arena"+number)+extra);

				if (confirmed && allow)
				{
					thisEntity.AddUIntData("newGame", GetTime());
					thisEntity.AddStringData("scene", "pvp_lv" + number + ".esc");
					PlaySample("soundfx/newgame.mp3");
				} else if (confirmed && !allow)
				{
					PlaySample("soundfx/fail.ogg");
				}
			}
			
			if (entityName != thisEntity.GetStringData("lastButton"))
			{
				PlaySample("soundfx/help.mp3");
			}
			thisEntity.AddStringData("lastButton", entityName);
		}
		showToggleFullscreenMessage();
	}
	else
	{
		float bias = 0;
		if (fadeOut(thisEntity.GetUIntData("newGame"), bias))
		{
			newGame(thisEntity.GetStringData("scene"));
		}
		SetSampleVolume("soundfx/menu.mp3", 1.0f-bias);
		loadingMessage();
	}
	fadeIn(thisEntity.GetUIntData("menuStartTime"));
}

void ETHCallback_thumbnail(ETHEntity @thisEntity)
{
	if (thisEntity.CheckCustomData("score") != DT_NODATA)
	{
		if (thisEntity.GetUIntData("score") <= getGetBestTime())
		{
			thisEntity.SetColor(vector3(0.05f, 0.05f, 0.05f));
			//thisEntity.SetAlpha(0.8f);
		}
	}
}

bool waitForInputToMenu()
{
	if (getCancelButtonStatus(0) == KS_HIT)
	{
		return goToMenu();
	}
	return false;
}

bool goToMenu()
{
	if (GetSceneFileName() != "scenes/menu.esc")
	{
		LoadScene("scenes/menu.esc", "menuPreLoop", "menuLoop", vector2(1024,256));
		return true;
	}
	return false;
}

bool escToGoToMenu()
{
	ETHInput @input = GetInputHandle();
	if (input.GetKeyState(K_ESC) == KS_HIT)
	{
		LoadScene("scenes/menu.esc", "menuPreLoop", "menuLoop", vector2(1024,256));
		return true;
	}
	return false;
}
