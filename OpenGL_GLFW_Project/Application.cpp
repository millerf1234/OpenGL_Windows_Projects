#include "Application.h"


Application::Application() {
	mApplicationValid = true;
	displayInfo = nullptr;
	glfwInitializer = nullptr;

	fprintf(MSGLOG, "Application is loading...\n");
	setupGLFW(); 
	loadGraphicsLanguageFunctions();
	checkMSAA(); //See if MSAA is being used as expected
	configureGraphicsContextDebugCallbackFunction(); //If the variable USE_DEBUG is false, then callback function will be empty
	if (!mApplicationValid) {
		fprintf(ERRLOG, "The application encountered an error while loading!\n");
		return;
	}
	else {
		fprintf(MSGLOG, "Application loading complete!\nPlaying Intro Movie...\n");
		playIntroMovie(); //Make intro movie a std::future while the rest of the application loads?
		//keep loading?
	}
}


Application::~Application() {
	if (glfwInitializer) {
		glfwInitializer->terminate();  
	}
}



void Application::launch() {
	if (!mApplicationValid) {
		fprintf(ERRLOG, "\nError launching Application, Application is invalid!\n");
		return;
	}
	
	fprintf(MSGLOG, "Loading Main Menu...\n"); //The context is active at this point, so any choice of what to load should happen on the render surface.
	//have a function here that runs a main menu...  this menu returns a struct consisting of an enum for which mode/sim to launch and then additional configuration parameters (num_players, control scheme, etc...)

	fprintf(MSGLOG, "Application is ready to load a specific program...\n");
	
	//fprintf(MSGLOG, "\n\n[Here will eventually be a list of available demos to load and run]\n\n");

	fprintf(MSGLOG, "\nLoading AssetLoadingDemo...\n");
	runAssetLoadingDemo();
	

		//if (true) { return; }

	fprintf(MSGLOG, "\nTeapotExplosion demo selected\n");
	fprintf(MSGLOG, "Loading TeapotExplosion...\n");
	std::unique_ptr<RenderDemoBase> TeapotExplosionDemo = std::make_unique<TeapotExplosion>(displayInfo);
	runRenderDemo(TeapotExplosionDemo, "Teapot Explosion Demo");
	
}

void Application::setupGLFW() {
	fprintf(MSGLOG, "Loading GLFW...\n");
	glfwInitializer = std::make_unique<GLFW_Init>();
	glfwInitializer->setDefaultMonitor(MONITOR_TO_USE);
	displayInfo = glfwInitializer->initialize();
	glfwInitializer->specifyWindowCallbackFunctions(); 
	if (!displayInfo) {
		mApplicationValid = false;
		return;
	}
}

void Application::loadGraphicsLanguageFunctions() {
	//Load OpenGL functions once window context has been set
	fprintf(MSGLOG, "Loading Graphics Language Functions...\n");
	int success = gladLoadGL();
	if (!success) {
		fprintf(ERRLOG, "\nERROR OCCURED LOADING GRAPHICS LANGUAGE FUNCTIONS!\n");
		mApplicationValid = false;
		return;
	}

	fprintf(MSGLOG, "  Graphics Language loaded.\n");
	fprintf(MSGLOG, "\tGraphics Language version: OpenGL %s\n", glGetString(GL_VERSION));
	fprintf(MSGLOG, "\tGraphics Language Vendor:  %s\n", glGetString(GL_VENDOR));
	fprintf(MSGLOG, "\tGraphics Render Device:    %s\n", glGetString(GL_RENDERER));

	
	fprintf(MSGLOG, "\nInitializing GL StateMachine...\n");

	fprintf(MSGLOG, "\tActivating GL depth-test\n");
	glEnable(GL_DEPTH_TEST); //Turn on the depth test for z-culling
	
	//fprintf(MSGLOG, "\tActivating GL scissor-test\n");
	//glEnable(GL_SCISSOR_TEST);

	//Is blending enabled here too as well? Or should blending be enabled/disabled as 
	//needed for transparent objects? I recall that in Vulkan, blending is a property assigned 
	//to a graphics pipeline. Perhaps OpenGL has a modern similar approach to prevent having to 
	//repeatedly modify the context's global state
}

void Application::configureGraphicsContextDebugCallbackFunction() const {
	fprintf(MSGLOG, "Graphics Context Debug Output Callback Messaging: ");
	if (USE_DEBUG) {   //set in ProjectSetup.h
		fprintf(MSGLOG, "Enabled\n");
		glEnable(GL_DEBUG_OUTPUT);
		fprintf(MSGLOG, "\t[For now] Forcing Synchronization Between Graphics Context and Application...\n\n");
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); //Might cause problems if context gets multithreaded
		glDebugMessageCallback(printGraphicsContextMessageCallback, 0);
	}
	else {
		fprintf(MSGLOG, "Disabled\n\n");
	}
}

void Application::checkMSAA() const { //hmm
	if (mApplicationValid) {
		//glad_glEnable(GL_MULTISAMPLE);
		GLint bufs = -1;
		GLint samples = -1;

		glad_glGetIntegerv(GL_SAMPLE_BUFFERS, &bufs);
		glad_glGetIntegerv(GL_SAMPLES, &samples);
		fprintf(MSGLOG, "\nMSAA CONFIGURATION:\n\tBuffers Available: %d\n\tSamples: %d\n\n", bufs, samples);

		glGetIntegerv(GL_SAMPLE_BUFFERS, &bufs);
		glGetIntegerv(GL_SAMPLES, &samples);
		fprintf(MSGLOG, "\nMSAA CONFIGURATION:\n\tBuffers Available: %d\n\tSamples: %d\n\n", bufs, samples);
	}
}


void Application::playIntroMovie() {
	/*if ((nullptr != displayInfo->activeMonitor) && (nullptr != glfwGetCurrentContext())) {
		glfwMakeContextCurrent(displayInfo->activeMonitor);
	}*/
	fprintf(MSGLOG, "PSYCH! There is no intro movie!\n");
}

void Application::runRenderDemo(std::unique_ptr<RenderDemoBase> & renderDemo, const char * name) {
	bool demoHasNameProvided = ((name != nullptr) && (*name != '\0'));

	//Perform error check
	if (renderDemo == nullptr) {
		if (demoHasNameProvided)
			fprintf(ERRLOG, "\n\n\t(While trying to run RenderDemo %s)\n", name);
		else
			fprintf(ERRLOG, "\n\n");
		fprintf(ERRLOG, "\nWhoah! A 'nullptr' was passed in as the RenderDemo to\nthis Application's "
			"member function 'runRenderDemo()'.\n"
			"Passing around nullptrs is very dangerous, luckily this time it was caught.\n"
			"Only reasonable thing to do here is crash and exit gracefully...\n  Demo is exiting!\n");
		fprintf(MSGLOG, "\t  [ Press ENTER to continue ]\n");
	}
	//Perform a second error check
	else if ( !(displayInfo && mApplicationValid) ) {
		fprintf(ERRLOG, "\nError launching RenderDemo!\n"
			"The Application is invalid or the detected display information is null!\n");
	}
	//Else Proceed with loading and launching the RenderDemo
	else {
		if (demoHasNameProvided) {
			fprintf(MSGLOG, "Loading %s... \n", name);
		}
		renderDemo->loadAssets();
		renderDemo->run();
	}
}

void Application::runAssetLoadingDemo() {
	std::unique_ptr<RenderDemoBase> assetLoadingDemo = std::make_unique<AssetLoadingDemo>(displayInfo);
	runRenderDemo(assetLoadingDemo, "AssetLoadingDemo");
}