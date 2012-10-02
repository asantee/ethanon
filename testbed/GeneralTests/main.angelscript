#include "eth_util.angelscript"
#include "Testbed.angelscript"
#include "globalCallbacks.angelscript"
#include "nonCoreTests.angelscript"

Testbed testbed;

void main()
{
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
}

const string LOOP = "loop";
const string PRELOOP = "preLoop";

void loop()
{
	testbed.runCurrentLoopFunction();
}

void preLoop()
{
	testPacking();
	SetBorderBucketsDrawing(false);
	testbed.runCurrentPreLoopFunction();
}