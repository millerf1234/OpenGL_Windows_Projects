//File:                 Application.cpp
//Description:          Implementation for the Application class. See header file for more detail
//
//Programmer:           Forrest Miller
//Date:                 July - November(and beyond) 2018

#include <thread>
#include <cstddef> //NOTE: Might not be necessary anymore to include <cstddef>

#include "Application.h"

//The renderdemos are included in the order that they were written. As such, the 
//level of complexity in each project increases accordingly. 
#include "TeapotExplosion.h"
//At this point I found it easier to piggy-back off of AssetLoadingDemo, so these
//RenderDemoes are all heavily built atop AssetLoadingDemo
#include "LightsourceTestDemo.h"
#include "AssetLoadingDemo.h"
#include "FlyingCameraDemo.h"


void Application::initialize() {
	mApplicationValid = true;
	initReport = nullptr;
	glfwInitializer = nullptr;


    //If Application is built with macro USE_DEBUG_ defined, we will report more 
    //details during start-up about the Runtime Environment's configuration and
    //this Application's state.
    if constexpr (USE_DEBUG) {
        //fprintf(MSGLOG, "[DEBUG:    __cplusplus is defined as %ld]\n\n", __cplusplus);
        
        //Print the ID for this thread (the Application's main thread)
        auto threadID = std::this_thread::get_id();
        std::ostringstream threadIDString;
        threadIDString << "[ ~DEBUG~ ] Application Operating On Thread: 0x" << std::hex << threadID;
        fprintf(MSGLOG, "\n%s\n", threadIDString.str().c_str());


        fprintf(MSGLOG, "[ ~DEBUG~ ] Maximum number of bytes in a multibyte character on this platform: %d\n", MB_CUR_MAX);

        fprintf(MSGLOG, "\n");
    }


	fprintf(MSGLOG, "Application is loading...\n");

    // STEP 1     INITIALIZE GLFW, Create an OpenGL Context and Acquire Our
    //Application's Window 
    if ( !setupGLFW() ) {
        mApplicationValid = false;

		fprintf(ERRLOG, "\nThe application encountered an error setting up GLFW!\n");
		return;
	}
    else {
        //If GLFW initializes but then we fail later on in the initialization, 
        //we run into a problem that the window we opened with GLFW may not want to relinquish focus
        //back to the OS/Desktop (depending on how it is configured). To ensure the user is able to 
        //see any potential error message, we manually ensure the Application's window relinquishes 
        //its focus for now until we have reached the end of the Application's constructor.
        GLFWwindow* currentWindow = glfwGetCurrentContext();
        if (currentWindow) 
            glfwIconifyWindow(currentWindow);
        
    }

    // STEP 2      Once GLFW has been successfully initialized, move on to loading 
    //             definitions for the GL function pointers. This is a really tedious
    //             process that we luckily are able to defer to third party library 
    //            'glad'. 
	if ( !loadGraphicsLanguageFunctions() ) {
        mApplicationValid = false;

        fprintf(ERRLOG, "\n"
            "The application encountered an error while loading the graphics language!\n"
            "        (Please Ensure You Have An OpenGL %d.%d Compatible Graphics\n"
            "               Driver Installed And Updated To Its Most Recent Version)\n\n"
            "\t[Press ENTER to end the process]\n\n",
            DEFAULT_OPENGL_VERSION_MAJOR,
            DEFAULT_OPENGL_VERSION_MINOR);
        std::cin.get();
        return;
	}

    //  Step 3        Once GLFW and glad have completed their required initialization procedures,
    //                the Application performs a lot of it's own self-configuration now before it is
    //                ready to return from its constructor. This configuration process involves setting
    //                all of the necessary callback functions and setting the initial state of the 
    //                OpenGL context.
	configureGraphicsContextDebugCallbackFunctions(); //The behavior of context debugging is set in Header Files -> SetupAndConfiguration -> Modifiable -> DebugBehavior -> "DebugSettings.h"
	setInitialGLState();

    //Check to see if GLFW supports Raw Mouse Motion Input
    //  (see: https://www.glfw.org/docs/latest/group__input.html#gae4ee0dbd0d256183e1ea4026d897e1c2 )
    if (glfwRawMouseMotionSupported())
        fprintf(MSGLOG, "\n    [Detected that raw mouse input is supported on this platform!\n");
    else 
        fprintf(MSGLOG, "\n    [Detected that raw mouse input is unsupported on this platform!\n");

	//MSAA requires Framebuffer Objects to be used, which I have not yet gotten around to implementing. 
	//Thus as of right now, checkMSAA() will always return showing no MSAA being used.
	//checkMSAA(); //See if MSAA is being used as expected
		
	if (!mApplicationValid) {
		fprintf(ERRLOG, "The application encountered an error while loading!\n");
		return;
	}
	else {
		fprintf(MSGLOG, "Application loading complete!\nPlaying Intro Movie...\n");
		playIntroMovie(); 
	}
}


Application::Application() noexcept {
    
    try {
        initialize();
        if (!mApplicationValid) {
            fprintf(MSGLOG, "\n\t[Press Enter to abort]\t");
            std::cin.get(); //Need this on Windows to keep the terminal window open
        }
        else { //Else we are successfully initialized and can restore the window
            GLFWwindow* currentWindow = glfwGetCurrentContext();
            if (currentWindow) 
                glfwRestoreWindow(currentWindow);
        }
    }

    catch (const std::system_error& e) {
            fprintf(ERRLOG, "\nCaught a system error exception:\n\t%s\n", e.what());
            fprintf(ERRLOG, "\n\n  [Well this is kinda awkward... For once it appears\n"
                "   as though the reason for crashing is not due to poor\n"
                "   work by the Application programmer but in fact a system\n"
                "   error. Best Just Do What Everyone Else Does And Blame Windows\n"
                "   [Even if you are running this on Linux].\n");
    }
    catch (const std::exception& e) {
        fprintf(ERRLOG, "\n\n\n\t\t[In Application Constructor]\n\tError! Exception Encountered!\n"
            "EXCEPTION MSG: %s\n\n", e.what());
        assert(false);
    }

    catch (...) {
        fprintf(ERRLOG, "\n\n\n\t\t[In Application Constructor]\n\tError! UNKNOWN EXCEPTION ENCOUNTERED!\n");
        assert(false);
    }
}


Application::~Application() noexcept {
	if (glfwInitializer) {
		glfwInitializer->terminate();  
	}
}


void Application::launch() {
    try {
        if (!mApplicationValid) {
            fprintf(ERRLOG, "\nError launching Application, Application is invalid!\n");
            return;
        }

        fprintf(MSGLOG, "Loading Main Menu...\n"); //The context is active at this point, so any choice of what to load should happen on the render surface.
        //have a function here that runs a main menu...  this menu returns a struct consisting of an enum for which mode/sim to launch and then additional configuration parameters (num_players, control scheme, etc...)

        fprintf(MSGLOG, "Application is ready to load a specific program...\n");



        //fprintf(MSGLOG, "\n\n[Here will eventually be a list of available demos to load and run]\n\n");

        //fprintf(MSGLOG, "\nSelected FlyingCameraDemo.\n");
        //runFlyingCameraDemo();
        //return;

        fprintf(MSGLOG, "\nSelected AssetLoadingDemo.\n");
        runAssetLoadingDemo();
        return;

        //fprintf(MSGLOG, "\nSelected LightsourceTestDemo.\n");
        //runLightsourceTestDemo();
        //return;

        //fprintf(MSGLOG, "\nSelected TeapotExplosionDemo.\n");
        //runTeapotExplosionDemo();
        //return;

    }
    catch (const std::system_error& e) {
        fprintf(stderr, "\nCaught a system error exception:\n\t%s\n", e.what());
        fprintf(stderr, "\n\n  [Well this is kinda awkward... For once it appears\n"
            "   as though the reason for crashing is not due to poor\n"
            "   work by the Application programmer but in fact a system\n"
            "   error. Best Just Do What Everyone Else Does And Blame Windows\n"
            "   [Even if you are running this on Linux].\n");
    }

    catch (const std::exception& e) {
        fprintf(ERRLOG, "\n\n\n\n\n\n\n\n\t\t[In Application's \'run()\' function]\n"
            "\n\t\tError! Exception Encountered!\n"
            "EXCEPTION MSG: %s\n\n\n\n", e.what());
        assert(false);
    }


    catch (...) {
        fprintf(ERRLOG, "\n\n\n\n\n\n\n\n\t\t[In Application's \'run()\' function]\n"
            "\n\t\tError! UNKNOWN EXCEPTION ENCOUNTERED!\n\n\n\n");
        assert(false);
    }

}


bool Application::setupGLFW() {
	fprintf(MSGLOG, "Loading GLFW...\n");
   
	glfwInitializer = std::make_unique<GLFW_Init>();
	glfwInitializer->setDefaultMonitor(MONITOR_TO_USE);
	initReport = glfwInitializer->initialize();
	
	if (!initReport) {
		fprintf(ERRLOG, "\nAn Error Occurred Setting Up The GLFW Window!\n");
		mApplicationValid = false;
		return false;
	}
	else {
		glfwInitializer->specifyWindowCallbackFunctions();
		//glfwInitializer->setWindowUserPointer(static_cast<void *>(initReport.get())); 
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

    reportDetailsFromGLImplementation();

	return true;
}

void Application::reportDetailsFromGLImplementation() {

    //if (GLAD_GL_EXT_framebuffer_multisample) {
    //	// GL_EXT_framebuffer_multisample is supported 
    //}

    fprintf(MSGLOG, "  Graphics Language loaded.\n");
    fprintf(MSGLOG, "\tGraphics Language Version:         OpenGL %s\n",
        glGetString(GL_VERSION));
    fprintf(MSGLOG, "\tGraphics Shading Language Version: %s\n",
        glGetString(GL_SHADING_LANGUAGE_VERSION));
    fprintf(MSGLOG, "\tGraphics Language Vendor:          %s\n",
        glGetString(GL_VENDOR));
    fprintf(MSGLOG, "\tGraphics Rendering Device:         %s\n",
        glGetString(GL_RENDERER));

}


void Application::configureGraphicsContextDebugCallbackFunctions() const {
	fprintf(MSGLOG, "Graphics Context Debug Output Callback Messaging: ");
	if (USE_DEBUG) {   //set in ProjectParameters.h
		fprintf(MSGLOG, "Enabled\n");
		glEnable(GL_DEBUG_OUTPUT);
		if (FORCE_SYNC_BETWEEN_CONTEXT_AND_APP) {
			fprintf(MSGLOG, "\t[For now] Forcing Synchronization Between Graphics Context and Application...\n\n");
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); //Might cause problems if context gets multithreaded
		}
		else {
			fprintf(MSGLOG, "\nWARNING! GL Context has been configured as a debug context but without\n"
				"forced synchronization between the context and the application.\n"
				"This means Context Callback messages may appear a long time after the\n"
				"event that caused them!\n"
				"[Re-enable debug output synchronization or disable debugging to remove this warning!]\n");
			glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		}
		glDebugMessageCallback(printGraphicsContextMessageCallback, nullptr);
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

void Application::checkMSAA() const { 
	if (mApplicationValid) {
		//glad_glEnable(GL_MULTISAMPLE); //Need framebuffer objects for proper MSAA
		GLint bufs = -1;
		GLint samples = -1;

		//glad_glGetIntegerv(GL_SAMPLE_BUFFERS, &bufs);
		//glad_glGetIntegerv(GL_SAMPLES, &samples);
		//fprintf(MSGLOG, "\nMSAA CONFIGURATION:\n\tBuffers Available: %d\n\tSamples: %d\n\n", bufs, samples);

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
        std::exit(EXIT_FAILURE);
	}
	//Perform a second error check
	else if ( !(initReport && mApplicationValid) ) {
		fprintf(ERRLOG, "\nError launching RenderDemo!\n"
			"The Application is invalid or the detected display information is null!\n");
	}
	//Else proceed with loading and launching the RenderDemo
    else {

        if (demoHasNameProvided) {
            fprintf(MSGLOG, "Loading %s... \n", name);
        }
        try {
            renderDemo->loadAssets();
            renderDemo->run();
        }
        catch (const std::exception& e) {
            fprintf(ERRLOG, "\nThe following exception was thrown inside the render demo!\n");
            fprintf(ERRLOG, "%s\n", e.what());
            fprintf(ERRLOG, "\n\t[Program will now crash...]\n");
            std::exit(EXIT_FAILURE);
        }
    }
}

void Application::runFlyingCameraDemo() {
    std::unique_ptr<RenderDemoBase> flyingCameraDemo = std::make_unique<FlyingCameraDemo>(initReport.get());
    runRenderDemo(flyingCameraDemo, "Asset Loading Demo");
}

void Application::runAssetLoadingDemo() {
	std::unique_ptr<RenderDemoBase> assetLoadingDemo = std::make_unique<AssetLoadingDemo>(initReport.get());
	runRenderDemo(assetLoadingDemo, "Asset Loading Demo");
}

void Application::runLightsourceTestDemo() {
	std::unique_ptr<RenderDemoBase> lightsourceTestDemo = std::make_unique<LightsourceTestDemo>(initReport.get());
	runRenderDemo(lightsourceTestDemo, "Lightsource Test Demo");
}

void Application::runTeapotExplosionDemo() {
	std::unique_ptr<RenderDemoBase> teapotExplosionDemo = std::make_unique<TeapotExplosion>(initReport.get());
	runRenderDemo(teapotExplosionDemo, "Teapot Explosion Demo");
}