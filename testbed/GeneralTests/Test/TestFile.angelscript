
class TestFile : Test
{
	string getName()
	{
		return "File r/w test";
	}

	void start()
	{
		SetScaleFactor(1.0f);
		LoadScene("empty", PRELOOP, LOOP);
		HideCursor(false);
		SetBackgroundColor(0xFFFFFFFF);
	}

	void preLoop()
	{
		output = "";

		const string fileName = "test.bin";
		const float f = PI;
		const int64 i64 =-9999;
		const uint64 ui64 = 8888;
		const int i =-512;
		const uint ui = 1024;
		const double d = 1.7976931348623158e+308;
		string str = "David Gilmour";
		{

			file fileWrite;
			fileWrite.open(fileName, "w");
			fileWrite.writeString(str);
			fileWrite.writeInt(i, 4);
			fileWrite.writeUInt(ui, 4);
			fileWrite.writeInt(i64, 8);
			fileWrite.writeUInt(ui64, 8);
			fileWrite.writeDouble(d);
			fileWrite.writeFloat(f);
			fileWrite.close();
		}
		{

			file fileRead;
			fileRead.open(fileName, "r");

			{
				string temp;
				fileRead.readString(str.length(), temp);
				if (temp == str)
					output += "string w/r test passed\n";
				else
					output += "string w/r test FAILED\n";
			}

			if (int(fileRead.readInt(4)) == i)
				output += "int w/r test passed\n";
			else
				output += "int w/r test FAILED\n";
			
			if (uint(fileRead.readUInt(4)) == ui)
				output += "uint w/r test passed\n";
			else
				output += "uint w/r test FAILED\n";

			if (fileRead.readInt(8) == i64)
				output += "int64 w/r test passed\n";
			else
				output += "int64 w/r test FAILED\n";

			if (fileRead.readUInt(8) == ui64)
				output += "uint64 w/r test passed\n";
			else
				output += "uint64 w/r test FAILED\n";

			if (fileRead.readDouble() == d)
				output += "double w/r test passed\n";
			else
				output += "double w/r test FAILED\n";

			if (fileRead.readFloat() == f)
				output += "float w/r test passed\n";
			else
				output += "float w/r test FAILED\n";

			fileRead.close();
		}
		string r = testEnml();
		output += r;
		if (r == "")
		{
			output += "All enml tests passed\n";
		}
		
		if (!FileExists("this_file_must_not_exist.txt"))
			output += "FileExists test passed\n";
		else
			output += "FileExists test FAILED\n";

		if (!FileInPackageExists("this_file_must_not_exist.txt"))
			output += "FileInPackageExists test passed\n";
		else
			output += "FileInPackageExists FAILED\n";
	}

	string testEnml()
	{
		string r;
		{
			enmlEntity entity0;
			entity0.add("band", "Pink Floyd");
			entity0.add("color", "red");
			entity0.add("people", "Tommy;Ozzy\\Jimmy;Robert");

			if (entity0.get("people") != "Tommy;Ozzy\\Jimmy;Robert")
				r += "get failed: " + entity0.get("people") + "\n";

			if (entity0.getAttributeNames().length() != 17)
				r += "getAttributeNames failed: " + entity0.getAttributeNames() + "\n";
			entity0.add("double", "0.5");
			entity0.add("float", "10.0f");
			entity0.add("int", "-666");
			entity0.add("uint", "666");

			enmlEntity entity1;
			entity1.add("param01", "value");
			entity1.add("param02", "value_;");

			enmlFile f;
			f.addEntity("ent01_", entity0);
			f.addEntity("myEntity", entity1);
			
			f.writeToFile("test.enml");
		}
		{
			enmlFile f;
			f.parseFromFile("test.enml");

			if (f.getEntityNames() != "ent01_,myEntity")
				r += "getEntityNames failed: " + f.getEntityNames() + "\n";

			double _d = 0;
			f.getDouble("ent01_", "double", _d);
			if (_d != 0.5)
				r += "getDouble failed: " + _d + "\n";
			
			int _i = 0;
			f.getInt("ent01_", "int", _i);
			if (_i != -666)
				r += "getInt failed: " + _i + "\n";

			uint _u = 0;
			f.getUInt("ent01_", "uint", _u);
			if (_u != 666)
				r += "getUInt failed: " + _u + "\n";

			float _f = 0;
			f.getFloat("ent01_", "float", _f);
			if (_f != 10.0f)
				r += "getFloat failed: " + _f + "\n";

			if (f.get("myEntity", "param01") != "value")
				r += "get myEntity param01 failed: " + f.get("myEntity", "param01") + "\n";

			if (f.get("myEntity", "param02") != "value_;")
				r += "get myEntity param02 failed: " + f.get("myEntity", "param02") + "\n";

		}
		{
			{enmlFile f;
			if (f.parseString("this { is a wrong entity }") == 0)
				r += "Should not compile01\n";}

			{enmlFile f;
			if (f.parseString("this { is = }") == 0)
				r += "Should not compile02\n";}

			{enmlFile f;
			if (f.parseString("this { is = wrong; new_entity { a=10; }") == 0)
				r += "Should not compile03\n";}

			{enmlFile f;
			if (f.parseString("this is = wrong; }") == 0)
				r += "Should not compile04\n";}

			{enmlFile f;
			if (f.parseString("this { is = wrong }") == 0)
				r += "Should not compile05\n";}

			{enmlFile f;
			if (f.parseString("this_is{very=weird;}but{must=work;fine=got_it?;}") != 0)
				r += "Should compile01\n";}

			{
				enmlFile f;
				if (f.parseString("a{b=c;}d{e=f;g=h;}") != 0)
					r += "Should compile02\n";
				if (f.get("a", "b") != "c")
					r += "Should match 01\n";
				if (f.get("d", "e") != "f")
					r += "Should match 02\n";
				if (f.get("d", "g") != "h")
					r += "Should match 03\n";
			}

		}
		return r;
	}

	void loop()
	{
		DrawText(vector2(32,32), output, "Verdana20_shadow.fnt", 0xFF000000);
	}
	
	string output;
}
