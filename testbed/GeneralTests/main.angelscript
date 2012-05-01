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
}

const string LOOP = "loop";
const string PRELOOP = "preLoop";

void loop()
{
	testbed.runCurrentLoopFunction();
}

void preLoop()
{
	SetBorderBucketsDrawing(false);
	testbed.runCurrentPreLoopFunction();
}