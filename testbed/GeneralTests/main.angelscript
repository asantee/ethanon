#include "eth_util.angelscript"
#include "Testbed.angelscript"
#include "globalCallbacks.angelscript"
#include "nonCoreTests.angelscript"

#include "fileEncodedWithoutUTF8BOM.angelscript"

Testbed testbed;

void main()
{
	SetScaleFactor(1.0f);

	testbed.start();
	runMathTests();
	runDictionaryTests();
	runStringTests();

	// test words defined in the app.enml
	int counter = 0;
	#if IN_TESTBED
	++counter;
	#endif

	#if ETHANON_SAMPLE
	counter++;
	#endif

	#if RUNNING_ON_WINDOWS
	++counter;
	#endif

	#if RUNNING_ON_MACOSX
	++counter;
	#endif

	#if DUPLICATE_DIRECTIVE
	counter++;
	#endif

	if (counter != 4)
	{
		print("app.enml defined words test failed!\x07");
	}
	else
	{
		print("app.enml defined words test PASSED!");
	}

	codeInsideFileWithoutBOM::print();
}

const string LOOP = "loop";
const string PRELOOP = "preLoop";

void preLoop()
{
	EnablePreLoadedLightmapsFromFile(false);
	testPacking();
	SetBorderBucketsDrawing(false);
	testbed.runCurrentPreLoopFunction();
}

void loop()
{
	testbed.runCurrentLoopFunction();
}
