class MyClass
{
	float yeah;
}

bool PACKING_TEST_PERFORMED = false;
void testPacking()
{
	if (IsPackLoadingEnabled())
	{
		print("IsPackLoadingEnabled function test 01 failed!\x07");
		return;
	}

	EnablePackLoading("entities.zip", "quickbrownfox");

	if (!IsPackLoadingEnabled())
	{
		print("IsPackLoadingEnabled function test 02 failed!\x07");
		DisablePackLoading();
		return;
	}

	LoadSprite("entities/arvore1.png");
	LoadSprite("entities/pilar_ct_small.png");

	DisablePackLoading();
	PACKING_TEST_PERFORMED = true;
}

void runDictionaryTests()
{
	dictionary dict;
	MyClass object;
	object.yeah = 666;

	MyClass mc;
	mc.yeah = 666;
	MyClass@ handle = @mc;

	dict.set("one", int(1));
	dict.set("object", object);
	dict.set("handle", @handle);

	if (dict.exists("one"))
	{
		const bool found = dict.get("handle", @handle);
		if (!found)
		{
			print("dictionary::get test FAILED\x07\n");
		}
	}
	else
	{
		print("dictionary::exists test FAILED\x07\n");
	}
	
	{
		int value = 0;
		dict.get("one", value);
		if (value != 1)
		{
			print("dictionary::get int test FAILED\x07\n");
		}
	}
	{
		MyClass obj;
		dict.get("object", obj);
		if (obj.yeah != 666)
		{
			print("dictionary::get object test FAILED\x07\n");
		}
	}
	{
		MyClass @h;
		dict.get("handle", @h);
		if (h.yeah != 666)
		{
			print("dictionary::get handle test " + h.yeah + " FAILED\x07\n");
		}
	}
}

void runStringTests()
{
	print("Platform: " + GetPlatformName());

	const string a = "andre";
	const string b = a.substr(2, NPOS);
	if (b != "dre")
	{
		print("string::substr test FAILED\x07\n");
	}
	const string c = a.substr(0, 2);
	if (c != "an")
	{
		print("string::substr test FAILED\x07 " + c + "\n");
	}
	
	if (a.find("n") != 1)
	{
		print("string::find test FAILED\x07\n");
	}
	if (a.find("d") != 2)
	{
		print("string::find test FAILED\x07\n");
	}
	if (a.find("r") != 3)
	{
		print("string::find test FAILED\x07\n");
	}
	if (a.find("e") != 4)
	{
		print("string::find test FAILED\x07\n");
	}
	
	const string ta = "Robert Plant, Jimmy Page, Angus Young, Brian Johnson";
	if (ta.find("Plant") != 7)
	{
		print("string::find test FAILED\x07\n");
	}
}

void runMathTests()
{
	print("Length of vector2(1,1) equals " + length(vector2(1,1)) + "\n");
	print("Length of vector3(4,4,2) equals " + length(vector3(4,4,2)) + "\n");
	print("Distance between vector2(1,1) and origin equals " + distance(vector2(0,0),vector2(1,1)) + "\n");
	print("Distance between vector3(4,4,2) and origin equals " + distance(vector3(0,0,0),vector3(4,4,2)) + "\n");
	print("Sign of -3.0f is " + sign(-3.0f) + "\n");
	print("Sign of 9786578.0f is " + sign(9786578.0f) + "\n");
	print("Sign of 0.0f is " + sign(0.0f) + "\n");
	print("Sign of 3 is " + sign(3) + "\n");
	print("Sign of -3 is " + sign(-3) + "\n");
	print("Sign of 0 is " + sign(0) + "\n");
	
	vector3 v3Temp(10,10,10);
	const vector3 v3Test = (3*v3Temp*5)+v3Temp-vector3(20,20,20);
	vector2 one(1,1);
	v3Temp += vector3(one,1);
	v3Temp *= 3;
	v3Temp /= 6;
	v3Temp -= vector3(vector2(1,1),1);
	if (v3Test == vector3(140,140,140))
		print("vector3 test passed\n");
	if (v3Test != vector3(140,140,140))
		print("vector3 test FAILED\x07\n");

	vector2 v2Temp(10,10);
	const vector2 v2Test = (3*v2Temp*5)+v2Temp-vector2(20,20);
	v2Temp += vector2(1,1);
	v2Temp *= 3;
	v2Temp /= 6;
	v2Temp -= vector2(1,1);
	if (v2Test == vector2(140,140))
		print("vector2 test passed\n");
	if (v2Test != vector2(140,140))
		print("vector2 test FAILED\x07\n");
		
	collisionBox box0();
	collisionBox box1(vector3(1,2,3), vector3(4,5,6));
	collisionBox box2(box1);
	
	if (box0.pos != vector3(0,0,0))
		print("box0.pos test FAILED\x07\n");
	if (box0.size != vector3(0,0,0))
		print("box0.size test FAILED\x07\n");

	if (box1.pos != vector3(1,2,3))
		print("box1.pos test FAILED\x07\n");
	if (box1.size != vector3(4,5,6))
		print("box1.size test FAILED\x07\n");

	if (box2.pos != box1.pos)
		print("box2.pos test FAILED\x07\n");
	if (box2.size != box1.size)
		print("box2.size test FAILED\x07\n");

	// matrix tests
	{
		matrix4x4 m, t;
		m = scale(2.0f,3.0f,4.0f);
		m = translate(2.0f,3.0f,4.0f);
		t = rotateX(1.0f);
		t = rotateY(2.0f);
		t = rotateZ(3.0f);
		m = multiply(m, t);

		{
			vector3 v(0,0,0);
			v = multiply(v, m);
		}

		const float an = getAngle(vector2(1, 2));
		
		m.set(2, 3, 666.0f);
		if (m.get(2, 3) != 666.0f)
			print("m.get test failed\x07\n");
	}
	
	// vector single param constructors
	{
		vector2 a(0.4f);
		vector3 b(PI);
		if (a != vector2(0.4f, 0.4f))
			print("vector2 single param test failed\x07\n");
		if (b != vector3(PI, PI, PI))
			print("vector3 single param test failed\x07\n");
	
	}
}