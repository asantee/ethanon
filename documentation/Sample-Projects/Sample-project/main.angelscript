#include "eth_util.angelscript"
#include "Collide.angelscript"

void main()
{
	LoadScene("scenes/level1.esc", "NewGame", "GameLoop");
}

// This function will be executed once when the map is loaded
void NewGame()
{
	ETHEntity @jazag;
	AddEntity("jazag.ent", vector3(200.0f, 200.0f, 0.0f), jazag); // file name, position, out handle
	
	// if it added the character successfuly, add some custom data to it
	if (jazag !is null)
	{
		jazag.SetVector2("lastDirection", vector2(0.0f, 1.0f));
	}

	// load audio resources
	LoadSoundEffect("soundfx/explosion.mp3");
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
		const bool willGoWindowed = !Windowed();
		print(willGoWindowed ? "going to windows" : "going to fullscreen");

		const videoMode bestVideoMode = GetVideoMode(GetVideoModeCount() - 1);

		SetWindowProperties("Ethanon Engine",
							willGoWindowed ? 1024 : bestVideoMode.width,
							willGoWindowed ?  768 : bestVideoMode.height,
							willGoWindowed, true, PF32BIT);
	}

	// Load and save game
	if (input.GetKeyState(K_S) == KS_HIT)
	{
		SaveScene("scenes/saved.esc");
	}
	if (input.GetKeyState(K_L) == KS_HIT)
	{
		LoadScene("scenes/saved.esc", "", "GameLoop");
	}

	const float fpsRate = GetFPSRate();
	DrawText(vector2(0, 0), "FPS: " + fpsRate + "\n" +
							"Scene name: " + GetSceneFileName() + "\n",
							"Verdana20.fnt", ARGB(50,255,255,255));
}

// This function will control the character. The engine automatically assigns this function
// to all entities whose original name equals "jazag.ent".
frameTimer charaTimer;
void ETHCallback_jazag(ETHEntity @character)
{
	ETHInput @input = GetInputHandle();
	vector2 moveDirection(0, 0);

	const float speed = UnitsPerSecond(100.0f);
	const uint stride = 100;

	moveDirection = normalize(getKeyboardDirections() + getTouchDirections(character));
	moveDirection *= speed;

	// stores the lastest direction in the entity so we always know
	// to what direction we should cast the spells
	if (moveDirection.x != 0 or moveDirection.y != 0)
	{
		character.SetVector2("lastDirection", moveDirection);
	}
	
	// if the user presses space, cast the fireball
	if (input.GetKeyState(K_SPACE) == KS_HIT)
	{
		ETHEntity @fireBallEnt = null;
		AddEntity("fireball.ent", character.GetPosition() + vector3(0, 0, 16), fireBallEnt);
		
		if (fireBallEnt !is null)
		{
			// the fireball direction will be the same as our character's last direction
			fireBallEnt.SetVector2("direction", character.GetVector2("lastDirection"));
			fireBallEnt.SetFloat("speed", 250.0f);
		}
	}

	// find the sprite frame row for walking animation
	if (moveDirection != vector2(0, 0))
		character.SetFrame(charaTimer.Set(0,3,stride), findDirectionRowIndex(getAngle(moveDirection)));	

	// move the character
	character.AddToPositionXY(moveDirection);
		
	// if it collides, stop it.
	if (CollideStatic(character))
	{
		character.AddToPositionXY(moveDirection *-1.0f);
	}
	
	// Centralizes the character
	SetCameraPos(character.GetPositionXY() - (GetScreenSize() / 2.0f));

	// cast a weak light around the character
	AddLight(character.GetPosition() + vector3(0, 0, 32), vector3(0.26f,0.2f,0.26f), 100.0f, true);
}

// Animate the fire balls (this callback function will be automatically assigned to all
// fireball.ent entities)
void ETHCallback_fireball(ETHEntity @fireball)
{
	vector2 dir = fireball.GetVector2("direction");
	const float speed = UnitsPerSecond(fireball.GetFloat("speed"));
	fireball.AddToPositionXY(dir * speed);
	
	ETHEntity @ent;
	if (Collide(fireball, ent))
	{
		if (ent.GetEntityName() != "jazag.ent")
		{
			vector3 pos = fireball.GetPosition();
			AddEntity("explosion.ent", pos, 0);
			PlaySample("soundfx/explosion.mp3");
			@fireball = DeleteEntity(fireball);
			return;
		}
	}
}

// Compute character movement direction from the keyboard
vector2 getKeyboardDirections()
{
	ETHInput @input = GetInputHandle();
	vector2 dir(0, 0);

	if (input.KeyDown(K_LEFT))
		dir.x +=-1;

	if (input.KeyDown(K_RIGHT))
		dir.x += 1;

	if (input.KeyDown(K_UP))
		dir.y +=-1;

	if (input.KeyDown(K_DOWN))
		dir.y += 1;

	return dir;
}

// Compute character movement direction from the touch screen
vector2 getTouchDirections(ETHEntity@ character)
{
	ETHInput @input = GetInputHandle();
	
	for (uint t = 0; t < input.GetMaxTouchCount(); t++)
	{
		const KEY_STATE state = input.GetTouchState(t);
		if (state == KS_DOWN || state == KS_HIT)
		{
			// find the direction from the character to the touch position
			const vector2 charScreenPos = character.GetPositionXY() - GetCameraPos();
			return (input.GetTouchPos(t) - charScreenPos);
		}
	}
	// no touch, no move.
	return vector2(0, 0);
}

// Find the sprite frame row for walking animation based on the direction angle
const float LOWER_RIGHT_DIRECTION =  PI / 4.0f;
const float UPPER_RIGHT_DIRECTION =  (PI / 2.0f) + (PI / 4.0f);
const float UPPER_LEFT_DIRECTION  =  PI + (PI / 4.0f);
const float LOWER_LEFT_DIRECTION  =  PI + (PI / 4.0f) + (PI / 2.0f);
uint findDirectionRowIndex(const float radianAngle)
{
	if (radianAngle >= LOWER_RIGHT_DIRECTION && radianAngle < UPPER_RIGHT_DIRECTION)
		return 2;
	else if (radianAngle >= UPPER_RIGHT_DIRECTION && radianAngle < UPPER_LEFT_DIRECTION)
		return 3;
	else if (radianAngle >= UPPER_LEFT_DIRECTION && radianAngle < LOWER_LEFT_DIRECTION)
		return 1;
	else
		return 0;
}
