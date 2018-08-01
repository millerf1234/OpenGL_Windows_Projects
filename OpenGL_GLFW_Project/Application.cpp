#include "Application.h"



Application::Application() {
	mApplicationValid = true;
	mDisplayInfo = nullptr;
	mGLFWInitializer = nullptr;

	//initialize the shader interface logger
	//ShaderInterface::initializeShaderInterfaceLogger();

	fprintf(MSGLOG, "Application is loading...\n");
	//ShaderInterface::logMsgEvent("Application is loading...");
	setupGLFW();
	loadGraphicsLanguageFunctions();
	doExtraSetup(); //This function call to doExtraSetup() isn't necessary (probably), it's more for informational purposes and testing/learning/experimenting
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
	if (mGLFWInitializer) {
		mGLFWInitializer->terminate();
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
	
	//runHarfBuzzSetupTest(); //Confirmed that HarfBuzz works as intended. Unfortunatly I currently have no use for it (not until I get billboards/glyphs set up)

	fprintf(MSGLOG, "\n\nApplication is ready to switch to a new program...\n");
	runRenderProject1();



	//loop();

	//std::cin.get();
	

	/*  From Space Game's main()

	std::cout << std::endl << std::endl << "Preparing To Launch Game..." << std::endl;

	std::cout << "    Generating a new instance of the game..." << std::endl;
	Game Game(detectedDisplayInfo); //Create an instance of the game and tell
	//the instance about all the monitor setup that has been done thus far.



	//    //How I initially imagined loading would work:
	//    std::cout << "Done\n    Loading Models...";
	//    Game.loadGameObjects();
	//    std::cout << "Done\n    Loading Shaders...";
	//   // Game.loadShaders();
	//    std::cout << "Done\n    Loading Textures...";
	//    //Game.loadTextures();
	//    std::cout << "Done\n" << std::endl;
	//    std::cout << "Game Launched." << std::endl << std::endl;



	//How loading actually works:
	std::cout << "\n    Loading Game Assets...";
	Game.loadGameObjects();
	std::cout << "    Finished Loading Game Assets.\n" << std::endl;

	///As you can see in this next line, I lie... The game clearly has not launched when
	///I print out the message saying it has.
	std::cout << "Game Launched." << std::endl << std::endl;

	//Game.playIntroMovie(); //Implement later (when budget is bigger)

	Game.launch(); ///Now the game has launched
	//Launch will return only once the game has concluded its game loop
	windowSetupRoutines.terminate(); //Handle closing the window
	*/
}

void Application::setupGLFW() {
	fprintf(MSGLOG, "Loading GLFW...\n");
	mGLFWInitializer = std::make_unique<GLFW_Init>();
	mGLFWInitializer->setDefaultMonitor(MONITOR_TO_USE);
	mDisplayInfo = mGLFWInitializer->initialize();
	if (!mDisplayInfo) {
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



void Application::doExtraSetup() const {
	checkMSAA();
	checkSomeCompilerMacros();
}

void Application::checkMSAA() const { //hmm
	if (mApplicationValid) {
		//glad_glEnable(GL_MULTISAMPLE);
		GLint bufs = -1;
		GLint samples = -1;
		glad_glGetIntegerv(GL_SAMPLE_BUFFERS, &bufs);
		glad_glGetIntegerv(GL_SAMPLES, &samples);
		fprintf(MSGLOG, "\n\nMSAA CONFIGURATION:\n\tBuffers Available: %d\n\tSamples: %d\n\n", bufs, samples);
	}
}

void Application::checkSomeCompilerMacros() const {
	bool unsupportedCompilerDetected = false;
	//I got most of my ideas for this from: 
    //   https://isocpp.org/std/standing-documents/sd-6-sg10-feature-test-recommendations#recs.cpp98

	//A compiler can have certain settings turned on and off. These macros check the settings and report potentially dangerous mismatches

	//Check to see if Run-Time-Type Identification is unsupported!
#ifndef __cpp_rtti   
	fprintf(WRNLOG, "\nWaRNiNg! DETECTED THAT RUN-TIME-TYPE-IDENTIFICATION is not enabled!\nProgram probably will act strangly and crash a lot!\n");
	unsupportedCompilerDetected = true;
#endif //__cpp_rtti

	//Check to see if exceptions
#ifndef __cpp_exceptions
	fprintf(WRNLOG, "\nWarning! DETECTED THat EXCEPTION HANDLING (OR SOMETHING LIKE THAT) IS NOT ENABLED!\n"
		"This may cause a lot of crashing for if an exception gets thrown!\n");
	unsupportedCompilerDetected = true;
#endif //__cpp_exceptions




}


void Application::playIntroMovie() {
	/*if ((nullptr != mDisplayInfo->activeMonitor) && (nullptr != glfwGetCurrentContext())) {
		glfwMakeContextCurrent(mDisplayInfo->activeMonitor);
	}*/
	fprintf(MSGLOG, "PSYCH! There is no intro movie!\n");
}

void Application::runRenderProject1() {
	fprintf(MSGLOG, "Loading RenderProject1...\n");
	if (mDisplayInfo && mApplicationValid) {
		std::unique_ptr<RenderProject1> rp1 = std::make_unique<RenderProject1>(mDisplayInfo);
		rp1->loadAssets();
		rp1->run();
	}
	else {
		fprintf(ERRLOG, "Error loading RenderProject1! The Application is"
			"\ninvalid or the display information is null!\n");

	}
}

void Application::runHarfBuzzSetupTest() {
	fprintf(MSGLOG, "Loading HarfBuzzTest...\n");
	if (mDisplayInfo && mApplicationValid) {
		std::unique_ptr<HarfBuzzSetupTest> hbst1 = std::make_unique<HarfBuzzSetupTest>(mDisplayInfo);
		hbst1->run();
	}
	else {
		fprintf(ERRLOG, "Error loading HarfBuzzSetupTest! The Application is"
			"\ninvalid or the display information is null!\n");

	}
}

//bool Application::loop() {
//	bool loopShouldEnd = false;
//	while (!loopShouldEnd) {
//
//		std::cin.get();
//		loopShouldEnd = true;
//	}
//	return true;
//}