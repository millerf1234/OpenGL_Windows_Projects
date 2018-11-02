#include "Application.h"


Application::Application() {
	mApplicationValid = true;
	displayInfo = nullptr;
	glfwInitializer = nullptr;

	//initialize the shader interface logger
	//ShaderInterface::initializeShaderInterfaceLogger();

	fprintf(MSGLOG, "Application is loading...\n");
	setupGLFW(); 
	loadGraphicsLanguageFunctions();
	//doExtraSetup(); //This function call to doExtraSetup() isn't necessary (probably), it's more for informational purposes and testing/learning/experimenting
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
	
	//fprintf(MSGLOG, "Loading Main Menu...\n");
	//have a function here that runs a main menu...  this menu returns a struct consisting of an enum for which mode/sim to launch and then additional configuration parameters (num_players, control scheme, etc...)

	fprintf(MSGLOG, "Application is ready to load a specific program...\n");
	

	//fprintf(MSGLOG, "\n\nApplication is ready to switch to a new program...\n");
	//The two legacy demos:
	//runRenderProject1();
	//runGeometryShaderExplosion();

	
	fprintf(MSGLOG, "\n\n[Here will eventually be a list of available demos to load and run]\n\n");

	//fprintf(MSGLOG, "\nLoading AssetLoadingDemo...\n");
		//runAssetLoadingDemo();
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

	fprintf(MSGLOG, "\tGraphics Language loaded.\n\tGraphics Language version: OpenGL %s", glGetString(GL_VERSION));
	
	fprintf(MSGLOG, "\nInitializing GL StateMachine...\n");
	std::cout << "\tActivating GL depth-test\n";
	glEnable(GL_DEPTH_TEST); //Turn on the depth test for z-culling
	std::cout << "\tActivating GL scissor-test\n";
	glEnable(GL_SCISSOR_TEST);

	//Is blending enabled here too as well? Or should blending be enabled/disabled as 
	//needed for transparent objects?
}

void Application::checkIfGraphicsContextDebugCallbackFunctionEnabled() const {
	fprintf(MSGLOG, "Graphics Context Debug Output: ");
	if (USE_DEBUG) {   //set in ProjectSetup.h
		fprintf(MSGLOG, "Enabled\n\n");
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); //Might cause problems if context gets multithreaded
		glDebugMessageCallback(printGraphicsContextMessageCallback, 0);
	}
	else {
		fprintf(MSGLOG, "Disabled\n\n");
	}
}


void Application::doExtraSetup() const {
	checkMSAA();
	//checkSomeCompilerMacros(); //This is now done during construction with functions in "ProjectSetup.h"
	checkIfGraphicsContextDebugCallbackFunctionEnabled();
}

void Application::checkMSAA() const { //hmm
	if (mApplicationValid) {
		//glad_glEnable(GL_MULTISAMPLE);
		GLint bufs = -1;
		GLint samples = -1;
		glad_glGetIntegerv(GL_SAMPLE_BUFFERS, &bufs);
		glad_glGetIntegerv(GL_SAMPLES, &samples);
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