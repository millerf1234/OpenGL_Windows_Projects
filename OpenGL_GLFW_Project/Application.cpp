#include "Application.h"

void Application::initialize() {
	mApplicationValid = true;
	displayInfo = nullptr;
	glfwInitializer = nullptr;

	fprintf(MSGLOG, "Application is loading...\n");
	if ( !setupGLFW() ) {
		fprintf(ERRLOG, "\nThe application encountered an error setting up GLFW!\n");
	}
	else {
		if ( !loadGraphicsLanguageFunctions() ) {
			fprintf(ERRLOG, "\nThe application encountered an error while loading the grpahics language!\n");
		}
		else {
			configureGraphicsContextDebugCallbackFunction(); //The behavior of context debugging is set in "ProjectParameters.h"
			setInitialGLState();

			//MSAA requires Framebuffer Objects to be used, which I have not yet gotten around to implementing. 
			//Thus as of right now, checkMSAA() will always return showing no MSAA being used.
			//checkMSAA(); //See if MSAA is being used as expected
		}
	}
	
	if (!mApplicationValid) {
		fprintf(ERRLOG, "The application encountered an error while loading!\n");
		return;
	}
	else {
		fprintf(MSGLOG, "Application loading complete!\nPlaying Intro Movie...\n");
		playIntroMovie(); 
	}
}


Application::Application() {
	initialize();
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


	//fprintf(MSGLOG, "\nSelected AssetLoadingDemo.\n");
	//runAssetLoadingDemo();
	

	fprintf(MSGLOG, "\nSelected LightsourceTestDemo.\n");
	runLightsourceTestDemo();
	return;

	fprintf(MSGLOG, "\nSelected TeapotExplosionDemo.\n");
	runTeapotExplosionDemo();
	return;
}


bool Application::setupGLFW() {
	fprintf(MSGLOG, "Loading GLFW...\n");
	glfwInitializer = std::make_unique<GLFW_Init>();
	glfwInitializer->setDefaultMonitor(MONITOR_TO_USE);
	displayInfo = glfwInitializer->initialize();
	
	if (!displayInfo) {
		fprintf(ERRLOG, "\nAn Error Occured Setting Up The GLFW Window!\n");
		mApplicationValid = false;
		return false;
	}
	else {
		glfwInitializer->specifyWindowCallbackFunctions();
		glfwInitializer->setWindowUserPointer(static_cast<void *>(displayInfo.get())); 
		return true;
	}
}

bool Application::loadGraphicsLanguageFunctions() {
	//Load OpenGL functions once window context has been set
	fprintf(MSGLOG, "Loading Graphics Language Functions...\n");
	int success = gladLoadGL();
	if (!success) {
		fprintf(ERRLOG, "\nERROR OCCURED LOADING GRAPHICS LANGUAGE FUNCTIONS!\n");
		mApplicationValid = false;
		return false;
	}

	fprintf(MSGLOG, "  Graphics Language loaded.\n");
	fprintf(MSGLOG, "\tGraphics Language version: OpenGL %s\n", glGetString(GL_VERSION));
	fprintf(MSGLOG, "\tGraphics Language Vendor:  %s\n", glGetString(GL_VENDOR));
	fprintf(MSGLOG, "\tGraphics Render Device:    %s\n", glGetString(GL_RENDERER));

	return true;
}

void Application::configureGraphicsContextDebugCallbackFunction() const {
	fprintf(MSGLOG, "Graphics Context Debug Output Callback Messaging: ");
	if (USE_DEBUG) {   //set in ProjectParameters.h
		fprintf(MSGLOG, "Enabled\n");
		glEnable(GL_DEBUG_OUTPUT);
		if (FORCE_SYNC_BETWEEN_CONTEXT_AND_APP) {
			fprintf(MSGLOG, "\t[For now] Forcing Synchronization Between Graphics Context and Application...\n\n");
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); //Might cause problems if context gets multithreaded
		}
		else {
			const char * warnHeader = "**********";
				fprintf(WRNLOG, "\n%s%s%s%s%s%s%s%s\n"
					"WARNING! Context Has Been Created As A Debug Context Without Synchronization Between\n"
					"The Application And The Context! Callback Messages May Not Match Up With The Events That Caused Them!\n"
				"%s%s%s%s%s%s%s%s\n", warnHeader, warnHeader, warnHeader, warnHeader, warnHeader, warnHeader,
					warnHeader, warnHeader, warnHeader, warnHeader, warnHeader, warnHeader, warnHeader,
					warnHeader, warnHeader, warnHeader);
				glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		}
		glDebugMessageCallback(printGraphicsContextMessageCallback, 0);
	}
	else {
		fprintf(MSGLOG, "Disabled\n\n");
	}
}

void Application::setInitialGLState() {
	fprintf(MSGLOG, "\nInitializing GL StateMachine...\n");

	//For options of what can be activated/deactivated
	//with glEnable()/glDisable(), see:
	//   https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glEnable.xml

	fprintf(MSGLOG, "\tActivating GL depth-test\n");
	glEnable(GL_DEPTH_TEST); //Depth Test will be used for every draw call, so it is activated globally
}

void Application::checkMSAA() const { //hmm
	if (mApplicationValid) {
		//glad_glEnable(GL_MULTISAMPLE); //Need framebuffer objects for proper MSAA
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
	fprintf(MSGLOG, "PSYCH! There is no intro movie!\n");
}

void Application::runRenderDemo(std::unique_ptr<RenderDemoBase> & renderDemo, const char * name) {
	bool demoHasNameProvided = ((name != nullptr) && (*name != '\0'));

	//Perform error check
	if (renderDemo == nullptr) {
		if (demoHasNameProvided) {
			fprintf(ERRLOG, "\n\n\t(While trying to run RenderDemo %s)\n", name);
		}
		else {
			fprintf(ERRLOG, "\n\n");
		}
		fprintf(ERRLOG, "\nWhoah! A 'nullptr' was passed in as the RenderDemo to\nthis Application's "
			"member function 'runRenderDemo()'.\n"
			"Passing around nullptrs is very dangerous, luckily this time it was caught.\n"
			"Only reasonable thing to do here is crash and exit gracefully...\n  Demo is exiting!\n");
		fprintf(MSGLOG, "\t  [ Press ENTER to continue ]\n");
		std::cin.get(); //Require acknowledgment from programmer that he/she/they messed up big time before crashing 
	}
	//Perform a second error check
	else if ( !(displayInfo && mApplicationValid) ) {
		fprintf(ERRLOG, "\nError launching RenderDemo!\n"
			"The Application is invalid or the detected display information is null!\n");
	}
	//Else proceed with loading and launching the RenderDemo
	else {
		if (demoHasNameProvided) {
			fprintf(MSGLOG, "Loading %s... \n", name);
		}
		renderDemo->loadAssets();
		renderDemo->run();
	}
}

//This is commented out until the AssetLoadingDemo is fixed
//void Application::runAssetLoadingDemo() {
//	std::unique_ptr<RenderDemoBase> assetLoadingDemo = std::make_unique<AssetLoadingDemo>(displayInfo);
//	runRenderDemo(assetLoadingDemo, "AssetLoadingDemo");
//}

void Application::runLightsourceTestDemo() {
	std::unique_ptr<RenderDemoBase> lightsourceTestDemo = std::make_unique<LightsourceTestDemo>(displayInfo);
	runRenderDemo(lightsourceTestDemo, "Lightsource Test Demo");
}

void Application::runTeapotExplosionDemo() {
	std::unique_ptr<RenderDemoBase> teapotExplosionDemo = std::make_unique<TeapotExplosion>(displayInfo);
	runRenderDemo(teapotExplosionDemo, "Teapot Explosion Demo");
}