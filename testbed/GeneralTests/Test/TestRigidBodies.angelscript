#include "../plane_math.angelscript"

class TestRigidBodies : Test
{
	private ETHEntity@ picked;
	private ETHEntity@ selected;
	
	TestRigidBodies()
	{
		@picked = null;
	}

	string getName()
	{
		return "Rigid body test";
	}

	void start()
	{
		LoadScene("scenes/physicsTest.esc", PRELOOP, LOOP);
		SetBackgroundColor(ARGB(255, 0, 100, 150));
	}
	
	void preLoop()
	{
		SetPositionRoundUp(false);
		SetBorderBucketsDrawing(true);
	}
	
	void pickEntity()
	{
		ETHInput @input = GetInputHandle();
		const vector2 cursor = input.GetCursorPos();
		ETHEntityArray ents;
		GetAllEntitiesInScene(ents);
		@picked = null;
		for (uint t = 0; t < ents.size(); t++)
		{
			ETHPhysicsController@ sim = ents[t].GetPhysicsController();
			if (sim is null)
				continue;

			Shape shape(ents[t].GetPositionXY(), ents[t].GetSize(), ents[t].GetAngle());
			if (shape.overlapPoint(cursor))
			{
				@picked = @ents[t];
				DrawText(cursor, ents[t].GetEntityName() + " " + ents[t].GetID(), "Verdana14_shadow.fnt", 0xFFFFFFFF);
				if (input.GetKeyState(K_LMOUSE) == KS_HIT)
				{
					@selected = @picked;
				}
				break;
			}
		}
	}
	
	void movePickedEntity()
	{
		if (picked is null)
			return;
			
		ETHInput @input = GetInputHandle();
		if (input.KeyDown(K_SPACE))
		{
			picked.SetPositionXY(vector2(GetScreenSize().x * 0.5f, 0));
		}
	}
	
	void controlSelectedEntity()
	{
		if (selected is null)
			return;

		ETHPhysicsController@ controller = selected.GetPhysicsController();
		
		if (controller is null)
		{
			print("controller wasn't supposed to be null\x07");
			return;
		}
		
		ETHInput @input = GetInputHandle();
		const float speed = 2;
		const float horizontalVelocity = controller.GetLinearVelocity().x;
		
		const bool isCharacter = selected.GetEntityName() == "character.ent";
		if (isCharacter)
			controller.SetGravityScale(2.0f);
	
		if (input.KeyDown(K_UP))
		{
			controller.SetLinearVelocity(vector2(0.0f,-3 * speed));
		}
		if (input.KeyDown(K_DOWN))
		{
			controller.SetLinearVelocity(vector2(0.0f, 1 * speed));
		}
		if (input.KeyDown(K_LEFT))
		{
			if (isCharacter)
				controller.SetFriction(0.0f);
			controller.SetLinearVelocity(vector2(-1 * speed, controller.GetLinearVelocity().y));
		}
		else if (input.KeyDown(K_RIGHT))
		{
			if (isCharacter)
				controller.SetFriction(0.0f);
			controller.SetLinearVelocity(vector2(1 * speed, controller.GetLinearVelocity().y));
		}
		else
		{
			if (isCharacter)
			{
				controller.SetFriction(1.0f);
				controller.SetLinearVelocity(vector2(0.0f, controller.GetLinearVelocity().y));
			}
		}
	}

	private vector2 pointA;
	void testRayCast()
	{
		ETHInput@ input = GetInputHandle();
		if (input.GetKeyState(K_R) == KS_HIT)
		{
			pointA = input.GetCursorPos();
		}
		if (input.GetKeyState(K_R) == KS_DOWN)
		{
			const vector2 a(pointA);
			const vector2 b(input.GetCursorPos());
			vector2 point, normal;
			ETHEntity@ ent = GetClosestContact(a, b, point, normal);
			DrawLine(a, b, 0xFFFFFFFF, 0xFFFFFFFF, 3.0f);
			if (ent !is null)
			{
				SetSpriteOrigin("sprite/sphere.png", vector2(0.5f, 0.5f));
				DrawShapedSprite("sprite/sphere.png", ent.GetPositionXY(), vector2(40, 40), 0xC000FF00);
				DrawShapedSprite("sprite/sphere.png", point, vector2(8, 8), 0xFFFF0000);
				DrawLine(point, point + (normal * 30.0f), 0xFFFFFF00, 0xFFFFFF00, 2.0f);
			}
			ETHEntityArray entities;
			const bool found = GetContactEntities(a, b, @entities);
			if (found)
			{
				for (uint t = 0; t < entities.size(); t++)
				{
					DrawShapedSprite("sprite/sphere.png", entities[t].GetPositionXY(), vector2(32, 32), 0x80FF0000);
				}
			}
		}
	}

	void loop()
	{
		testRayCast();
		pickEntity();
		ETHInput @input = GetInputHandle();
		if (picked is null)
		{
			if (input.GetKeyState(K_LMOUSE) == KS_HIT)
			{
				AddEntity("metal_crate_body.ent", vector3(input.GetCursorPos(), 0.0f), randF(360.0f));
			}
			if (input.GetKeyState(K_RMOUSE) == KS_HIT)
			{
				AddEntity("asteroid_body.ent", vector3(input.GetCursorPos(), 0.0f), randF(360.0f));
			}
			if (input.GetKeyState(K_MMOUSE) == KS_HIT)
			{
				AddEntity("scaled_metal_crate_body.ent", vector3(input.GetCursorPos(), 0.0f), 180.0f);
			}
			if (input.GetKeyState(K_C) == KS_HIT)
			{
				AddEntity("character.ent", vector3(input.GetCursorPos(), 0.0f), 0.0f);
			}
			if (input.GetKeyState(K_S) == KS_HIT)
			{
				AddEntity("sensor.ent", vector3(input.GetCursorPos(), 0.0f), 0.0f);
			}
			if (input.GetKeyState(K_H) == KS_HIT)
			{
				AddEntity("half_metal_crate.ent", vector3(input.GetCursorPos(), 0.0f), 0.0f);
			}
			if (input.GetKeyState(K_U) == KS_HIT)
			{
				AddEntity("ushape.ent", vector3(input.GetCursorPos(), 0.0f));
			}
			if (input.GetKeyState(K_L) == KS_HIT)
			{
				AddEntity("lolipop.ent", vector3(input.GetCursorPos(), 0.0f));
			}
		}
		else
		{
			if (input.GetKeyState(K_1) == KS_HIT)
			{
				print("Scaled down");
				picked.Scale(0.5f);
			}
			if (input.GetKeyState(K_2) == KS_HIT)
			{
				print("Scaled up");
				picked.Scale(2);
			}
			if (input.GetKeyState(K_R) == KS_HIT)
			{
				ETHPhysicsController@ controller = picked.GetPhysicsController();
				if (controller !is null)
				{
					const float restitution = controller.GetRestitution() + 0.5f;
					controller.SetRestitution(restitution);
					print("Restitution set to " + restitution);
				}
				else
				{
					print("controller wasn't supposed to be null\x07");
				}
			}
		}
		if (input.GetKeyState(K_DELETE) == KS_HIT)
		{
			if (picked !is null)
			{
				print("Entity deleted: " + picked.GetEntityName() + " " + picked.GetID());
				DeleteEntity(picked);
				if (@picked == @selected)
				{
					@selected = null;
				}
				@picked = null;
			}
		}
		movePickedEntity();
		controlSelectedEntity();
		const string str =
			"-Click anywhere to add dynamic bodies\n"
			"-Press C to add a character to scene\n"
			"-Click any dynamic body to select it and move it with arrow keys\n"
			"-Hover+Delete to remove bodies from scene\n"
			"-Hover+R to increase 0.5 restitution\n";
		const vector2 tbSize = ComputeTextBoxSize("Verdana14_shadow.fnt", str);
		DrawText(vector2(0,GetScreenSize().y - tbSize.y), str, "Verdana14_shadow.fnt", 0xFFFFFFFF);
	}
}

void ETHBeginContactCallback_character(ETHEntity@ thisEntity, ETHEntity@ other, vector2 p0, vector2 p1, vector2 normal)
{
	print(thisEntity.GetEntityName() + " begin contact with " + other.GetEntityName());
	thisEntity.SetColor(vector3(1,0,0));
	other.SetColor(vector3(0,0,1));
	//DrawFadingText(vector2(10,300), vector2ToString(normal), "Verdana14_shadow.fnt", 0xFFFFFFFF, 3000);
}

void ETHEndContactCallback_character(ETHEntity@ thisEntity, ETHEntity@ other, vector2 p0, vector2 p1, vector2 normal)
{
	print(thisEntity.GetEntityName() + " end contact with " + other.GetEntityName());
	thisEntity.SetColor(vector3(1,1,1));
	other.SetColor(vector3(1,1,1));
	//DrawFadingText(vector2(10,300), vector2ToString(normal), "Verdana14_shadow.fnt", 0xFFFFFFFF, 3000);
}

void ETHCallback_motor(ETHEntity@ thisEntity)
{
	ETHPhysicsController@ controller = thisEntity.GetPhysicsController();
	ETHRevoluteJoint@ joint = controller.GetRevoluteJoint(0);
	ETHInput@ input = GetInputHandle();
	if (input.GetLeftClickState() == KS_DOWN)
	{
		joint.SetMotorSpeed((thisEntity.GetPositionXY().x - input.GetCursorPos().x) / 100.0f);
	}
}

void ETHCallback_character(ETHEntity@ thisEntity)
{
	DrawText(thisEntity.GetPositionXY(), "Running callback", "Verdana14_shadow.fnt", 0xFFFFFFFF);
}
