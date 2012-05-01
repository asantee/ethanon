#include "eth_util.angelscript"
#include "Collide.angelscript"

void main()
{
	LoadScene("scenes\\level1.esc", "NewGame", "GameLoop");

	HideCursor(true);

	SetWindowProperties("Ethanon Engine", 1024, 768, true, true, PF32BIT);
}

// This function will be executed once when the map is loaded
void NewGame()
{
	ETHEntity @jazag;
	AddEntity("jazag.ent", vector3(200,200,0), jazag); // file name, position, angle
	
	// if it added the character successfuly, add some custom data to it
	if (jazag !is null)
	{
		jazag.SetFloat("lastDirX", 0.0f);
		jazag.SetFloat("lastDirY", 1.0f);
	}
}

// This function will be executed every frame
void GameLoop()
{
	// if the user presses ESC, then quit
	ETHInput @input = GetInputHandle();
	if (input.KeyDown(K_ESC))
	{
		Exit();
	}
	
	// toogle fullscreen
	if (input.GetKeyState(K_ALT) == KS_DOWN && input.GetKeyState(K_RETURN) == KS_HIT)
	{
		SetWindowProperties("Ethanon Engine", 1024, 768, !Windowed(), true, PF32BIT);
	}

	// Load and save game
	if (input.GetKeyState(K_S) == KS_HIT)
	{
		SaveScene("scenes\\saved.esc");
	}
	if (input.GetKeyState(K_L) == KS_HIT)
	{
		LoadScene("scenes\\saved.esc", "", "GameLoop");
	}
	
	//#if DEBUG
	vector3 v3Pos(0,0,0);
	ETHEntity @jazag = SeekEntity("jazag.ent");
	if (jazag !is null)
	{
		v3Pos = jazag.GetPosition();
	}
	
	const float fFPS = GetFPSRate();
	DrawText(vector2(0,0),  "FPS: " + fFPS + "\n"
							"Scene name: " + GetSceneFileName() + "\n"
							"Character pos: " + vector3ToString(v3Pos),
							"Verdana20.fnt", ARGB(50,255,255,255));
	//#endif
}

// This function will control the character. The engine automatically assigns this function
// to all entities whose original name equals "jazag.ent".
frameTimer charaTimer;
void ETHCallback_jazag(ETHEntity @thisEntity)
{
	ETHInput @input = GetInputHandle();
	vector3 v3Dir(0,0,0);

	const float speed = UnitsPerSecond(100.0f);
	const uint stride = 100;
	if (input.KeyDown(K_LEFT))
	{
		thisEntity.SetFrame(charaTimer.Set(0,3,stride),1);	
		v3Dir.x +=-1;
	}
	if (input.KeyDown(K_RIGHT))
	{
		thisEntity.SetFrame(charaTimer.Set(0,3,stride),2);	
		v3Dir.x += 1;
	}
	if (input.KeyDown(K_UP))
	{
		thisEntity.SetFrame(charaTimer.Set(0,3,stride),3);	
		v3Dir.y +=-1;
	}
	if (input.KeyDown(K_DOWN))
	{
		thisEntity.SetFrame(charaTimer.Set(0,3,stride),0);	
		v3Dir.y += 1;
	}
	
	// stores the last direction in the entity so we always know
	// to what direction we should cast the spells
	if (v3Dir.x != 0 or v3Dir.y != 0)
	{
		thisEntity.SetFloat("lastDirX", v3Dir.x);
		thisEntity.SetFloat("lastDirY", v3Dir.y);
	}
	
	// if the user presses space, cast the fireball
	if (input.GetKeyState(K_SPACE) == KS_HIT)
	{
		ETHEntity @fireBallEnt = null;
		AddEntity("fireball.ent", thisEntity.GetPosition()+vector3(0,0,16), fireBallEnt);
		
		if (fireBallEnt !is null)
		{
			// the fireball direction will be the same as our character's last direction
			fireBallEnt.SetFloat("dirX", thisEntity.GetFloat("lastDirX"));
			fireBallEnt.SetFloat("dirY", thisEntity.GetFloat("lastDirY"));
			fireBallEnt.SetFloat("speed", 250.0f);
		}
	}
	
	// move the character
	v3Dir = normalize(v3Dir)*speed;
	thisEntity.AddToPosition(v3Dir);
		
	// if it collides, stop it.
	if (CollideStatic(thisEntity))
	{
		thisEntity.AddToPosition(v3Dir*-1);
	}
	
	// Centralizes the character
	SetCameraPos(thisEntity.GetPositionXY()-(GetScreenSize()/2));
	
	// cast a weak light over the character to let the player see it even in the dark
	AddLight(thisEntity.GetPosition()+vector3(0,0,32), vector3(0.15f,0.1f,0.15f), 100.0f, true);
}

// Animates the fire balls (this callback function will be automatically assigned to all
// fireball.ent entities
void ETHCallback_fireball(ETHEntity @thisEntity)
{
	float dirX = thisEntity.GetFloat("dirX");
	float dirY = thisEntity.GetFloat("dirY");
	const float speed = UnitsPerSecond(thisEntity.GetFloat("speed"));
	thisEntity.AddToPositionXY(vector2(dirX,dirY)*speed);
	
	ETHEntity @ent;
	if (Collide(thisEntity, ent))
	{
		if (ent.GetEntityName() != "jazag.ent")
		{
			vector3 pos = thisEntity.GetPosition();
			AddEntity("explosion.ent", pos, 0);
			@thisEntity = DeleteEntity(thisEntity);
			return;
		}
	}
}