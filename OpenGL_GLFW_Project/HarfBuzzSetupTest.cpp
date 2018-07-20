#include "HarfBuzzSetupTest.h"



HarfBuzzSetupTest::HarfBuzzSetupTest(std::shared_ptr<MonitorData> screenInfo) {
	initialize();
	
	//Make sure we have a monitor to render to
	if (!screenInfo || !screenInfo->activeMonitor) {
		error = true;
		return;
	}

	window = screenInfo->activeMonitor;
}



HarfBuzzSetupTest::~HarfBuzzSetupTest() {


}


void HarfBuzzSetupTest::run() {
	
	fprintf(MSGLOG, "\nRunning HarfBuzz Setup Test...\n");

	std::unique_ptr<HarfBuzzTester> hbTest = std::make_unique<HarfBuzzTester>();

	hbTest->runTest();


	fprintf(MSGLOG, "HarfBuzz test program is terminating...\n\n");

	//std::cin.get(); //keep the screen open
}



void HarfBuzzSetupTest::initialize() {
	error = false;
	window = nullptr;

}
