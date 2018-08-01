#include "RenderProject1.h"



RenderProject1::RenderProject1(std::shared_ptr<MonitorData> screenInfo) {
	initialize();
	//Make sure we have a monitor to render to
	if (!screenInfo || !screenInfo->activeMonitor) {
		error = true;
		return;
	}
	//Make sure the context is set to this monitor (and this thread [see glfw documentation])
	if (glfwGetCurrentContext() != screenInfo->activeMonitor) {
		std::ostringstream warning;
		warning << "\nWARNING!\n[In RenderProject1's constructor]\n" <<
			"RenderProject1 detected that the glfw active context was set" <<
			"\nto a different monitor or different execution-thread then\n" <<
			"the one passed to this contructor!\n";
		warning << "This means that running RenderProject1 will invalidate\n" <<
			"the previoud context by replacing it with this one, which\n" <<
			"could lead to errors! Please ensure that the correct context is\n" <<
			"being passed to RenderProject1!\n";

		fprintf(WRNLOG, warning.str().c_str());
		glfwMakeContextCurrent(screenInfo->activeMonitor);
	}
	window = screenInfo->activeMonitor;
}



RenderProject1::~RenderProject1() {


}


void RenderProject1::run() {
	if (error) {
		fprintf(MSGLOG, "An error occured while loading RenderProject1");
		return;
	}
	fprintf(MSGLOG, "\nRENDER PROJECT 1 IS LOADED AND RUNNING!\n");


	//fprintf(MSGLOG, "\nLoading TextEngine...\n");
	

	//fprintf(MSGLOG, "\nRender PROJECT 1 will attempt a HarfBuzz test...\n");
	//std::unique_ptr<HarfBuzzTester> hbTest = std::make_unique<HarfBuzzTester>();

	//hbTest->runTest();
	if (txtEngine) {
		fprintf(MSGLOG, "\nLoaded TextEngine! Text Engine ready status: %d\n", txtEngine->ready());
	}
	

	std::cin.get(); //keep the screen open


}



void RenderProject1::initialize() {
	error = false;
	window = nullptr;
	frameNumber = 0ull;
	frameUnpaused = 0ull;
	counter = 0.0f;

	txtEngine = nullptr;

}

void RenderProject1::loadAssets() {
	//txtEngine = std::make_unique<TextEngine>("Fonts\\Roboto-Black.ttf");

	fprintf(MSGLOG, "\nCreating a new shader program!\n");
	std::unique_ptr<ShaderProgram> testProgram = std::make_unique<ShaderProgram>();

	fprintf(MSGLOG, "\nAttaching vertex shader!\n");
	testProgram->attachVert(VERT_PASSTHROUGH2D);
	fprintf(MSGLOG, "\nAttaching fragment shader!\n");
	testProgram->attachFrag(FRAG_CONSTANTCOLOR);

	fprintf(MSGLOG, "\nAttempting to link program!\n");
	testProgram->link();

	testProgram->uniforms->updateUniform1f("zoom", 1.0f);

	//Test Updating/Caching uniform locations too!
}