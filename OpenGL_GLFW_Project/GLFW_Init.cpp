//  Performs the necessary steps to get GLFW up and running on program startup.
//  Also does the steps to close the window once program has completed running.
//
//  GLFW_Init.cpp
//  
//  Created by Forrest Miller on 2/12/18 on macbook pro
//  Modified to work on windows on July 16, 2018 by Forrest Miller


#include "GLFW_Init.h"

//Note that in GLFW3.h these are defined, might want to make the curser invisible when game launches...
// #define GLFW_CURSOR_NORMAL          0x00034001
// #define GLFW_CURSOR_HIDDEN          0x00034002
// #define GLFW_CURSOR_DISABLED        0x00034003



GLFW_Init::GLFW_Init() {
	width = height = refreshRate = 0;
	//pixelWidth = pixelHeight = 0; //Since GLFW3, screen coordinates and viewport pixels are separate concepts
	connectedDisplayCount = 0;
	monitors = nullptr;
	mWindow = nullptr;
	decoratedBorder = resizeable = forwardCompatible =  true;
	contextVersionMajor = DEFAULT_OPENGL_VERSION_MAJOR;
	contextVersionMinor = DEFAULT_OPENGL_VERSION_MINOR; 
	aaSamples = DEFAULT_AA_SAMPLES;
	if (USE_VSYNC) {
		vSyncInterval = 1; //Should only be 0 or 1
	}
	else {
		vSyncInterval = 0;
	}
	openFullScreen = USE_FULLSCREEN; 
	defaultMonitor = MONITOR_TO_USE;
	glfwIsInitialized = false;
}

GLFW_Init::~GLFW_Init() {
	
}

void GLFW_Init::terminate() {
	if (mWindow)
		glfwDestroyWindow(mWindow); //This function should be called for each window created by this application!
	glfwTerminate(); //Terminating is quite a bit easier than setting up, as you can see
}


//Do window setup routines and return a struct representing information on detected monitors
std::shared_ptr<MonitorData> GLFW_Init::initialize() {
	glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_FALSE); //Treat joystick hats as separate from Joystick buttons
	fprintf(MSGLOG, "Initializing GLFW..."); 
	if (!glfwInit()) {
		fprintf(ERRLOG, "\nError initializing GLFW!\n");
		glfwIsInitialized = false;
		return nullptr;
	}
	else {
		glfwIsInitialized = true;
	}
	fprintf(MSGLOG, "DONE!  GLFW version: %s\n", glfwGetVersionString()); 


	fprintf(MSGLOG, "\tWindow Context Rendering provided through OpenGL %d.%d\n", contextVersionMajor, contextVersionMinor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, contextVersionMajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, contextVersionMinor);
	
	fprintf(MSGLOG, "\tConfiguring OpenGL Context...\n");
	fprintf(MSGLOG, "\t  GL Profile: CORE\n"); //Always use CORE 
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	fprintf(MSGLOG, "\t  Create as debug context: ");  //Enabling this may reduce performance
	if (USE_DEBUG) {
		fprintf(MSGLOG, "ENABLED\n");
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
	}
	else {
		fprintf(MSGLOG, "DISABLED\n");
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_FALSE);
	}
	
	fprintf(MSGLOG, "\t  Setting Context Robustness / Reset Strategy:  "); 
	glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, GLFW_LOSE_CONTEXT_ON_RESET); //Can be set to either GLFW_NO_RESET_NOTIFICATION or GLFW_LOSE_CONTEXT_ON_RESET
	fprintf(MSGLOG, "LOSE_CONTEXT_ON_RESET\n");


	fprintf(MSGLOG, "\t  FORWARD COMPATIBILITY: ");
	if (forwardCompatible) {
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
		fprintf(MSGLOG, "TRUE\n");
	}
	else {
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_FALSE);
		fprintf(MSGLOG, "FALSE\n");
	}


	fprintf(MSGLOG, "\t  Decorate Window Border: ");
	if (decoratedBorder) {
		glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
		fprintf(MSGLOG, "TRUE\n");
	}
	else {
		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		fprintf(MSGLOG, "FALSE\n");
	}

	fprintf(MSGLOG, "\t  WINDOW RESIZEABLE: ");
	if (this->resizeable) {
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		fprintf(MSGLOG, "TRUE\n");
	}
	else {
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		fprintf(MSGLOG, "FALSE\n");
	}

	//glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE); //NOT SURE IF THIS IS NEEDED

	fprintf(MSGLOG, "\t  Full-Screen MSAA Samples: %d\n", aaSamples);
	glfwWindowHint(GL_SAMPLES, aaSamples);

	
	if (vSyncInterval == 0) {
		fprintf(MSGLOG, "\t  VSYNC: OFF\n");
		glfwSwapInterval(0);
	}
	else {
		fprintf(MSGLOG, "\t  VSYNC: ON\n");
		glfwSwapInterval(1);
	}
	

	fprintf(MSGLOG, "OpenGL context configured!\n");


	fprintf(MSGLOG, "\nDetecting monitors...\n");
	monitors = glfwGetMonitors(&connectedDisplayCount);//Tell GLFW to look for all monitors connected currently.
	if ((connectedDisplayCount == 0) || (monitors == nullptr) ) {
		fprintf(ERRLOG, "ERROR! GLFW was unable to detect any connected monitors!\n");
        fprintf(ERRLOG, "\n\t[Press Enter to exit program]\n");
        std::cin.get();
        glfwTerminate();
		glfwIsInitialized = false;
		return nullptr;
	}
	fprintf(MSGLOG, "\t%d connected displays are detected!\n", connectedDisplayCount);
	

	//First try to make the window open on the specified display
	if (openFullScreen) {
		if (connectedDisplayCount >= (defaultMonitor + 1)) { //Check to make sure there is at least enough connected displays for defaultMonitor to exist
			detectDisplayResolution(defaultMonitor, width, height, refreshRate);
			if (!glfwIsInitialized) {
				fprintf(ERRLOG, "\nError detecting display resolution!\n");
                fprintf(ERRLOG, "\n\t[Press Enter to exit program]\n");
                std::cin.get();
                glfwTerminate();
                std::exit(EXIT_FAILURE);
				return nullptr;
			}
			//Otherwise log the display information
			std::ostringstream displayInfoString;
			displayInfoString << "    Window will open on display: " << this->defaultMonitor + 1 
			 << "\n    Name of Display: " << glfwGetMonitorName(monitors[this->defaultMonitor])
			 << "\n    Resolution of this Display: " << this->width << "x" << this->height 
			 << "\n   Display Refresh Rate: " << this->refreshRate << "\n";

			fprintf(MSGLOG, "%s\nOpening Window...\n", displayInfoString.str().c_str());

			mWindow = glfwCreateWindow(width, height, NAME_OF_APPLICATION, monitors[defaultMonitor], nullptr);

			if (mWindow) {
				fprintf(MSGLOG, "Window Successfully Created!\n\n");
			}
			else {
				fprintf(ERRLOG, "Error occured creating GLFW window in fullscreen mode on monitor!\n");
				glfwIsInitialized = false;
			}
		}
		//Re-Implement more robust window checking later... 
		////If specified display is not connected, try opening on the first non-primary display detected
		//else if (this->connectedDisplayCount >= 2) {
		//	detectDisplayResolution(1, this->width, this->height, this->refreshRate);
		// //REMEMBER TO CHECK TO SEE IF CONTEXT IS STILL VALID AFTER DETECTING DISPLAY INFO
		//	std::cout << "    Window will open on display: " << 2 << std::endl;
		//	this->defaultMonitor = 1;
		//	std::cout << "    Name of Display: " << glfwGetMonitorName(monitors[this->defaultMonitor]) << std::endl;
		//	std::cout << "    Resolution of this Display: " << this->width << "x" << this->height << std::endl;
		//	std::cout << "    Display Refresh Rate: " << this->refreshRate << std::endl;

		//	std::cout << "Window Context is ready to open on this Display" << std::endl;

		//	this->mWindow = glfwCreateWindow(this->width, this->height, NAME_OF_APPLICATION,
		//		monitors[1], nullptr);
		//}
		////If all that fails, open on the primary display
		//else {
		//	detectDisplayResolution(0, this->width, this->height, this->refreshRate);
		//	std::cout << "    Window will open on display: " << 1 << std::endl;
		//	this->defaultMonitor = 0;
		//	std::cout << "    Name of Display: " << glfwGetMonitorName(monitors[this->defaultMonitor]) << std::endl;
		//	std::cout << "    Resolution of this Display: " << this->width << "x" << this->height << std::endl;
		//	std::cout << "    Display Refresh Rate: " << this->refreshRate << std::endl;

		//	std::cout << "Window Context is ready to open on this Display" << std::endl;

		//	this->mWindow = glfwCreateWindow(this->width, this->height, NAME_OF_APPLICATION,
		//		glfwGetPrimaryMonitor(), nullptr);
		//}
	}
	else { //Open windowed
		fprintf(MSGLOG, "\nWindow Context set to open in windowed mode...\n\nOpening Window\n");
		//(If not on a 4k monitor, then this resolution works fine. However with 4k, this is tiny.
		//mWindow = glfwCreateWindow(1670, 960, NAME_OF_APPLICATION, nullptr, nullptr); //Open as window
		//So if using a 4k monitor, then do something more like:
		mWindow = glfwCreateWindow(3600, 1800, NAME_OF_APPLICATION, nullptr, nullptr); //Open as window
		defaultMonitor = 0;
		if (mWindow) {
			glfwSetWindowMonitor(mWindow, NULL, 30, 50, 3600, 1800, GLFW_DONT_CARE);
			fprintf(MSGLOG, "Window Successfully Opened!\n\n");
		}
		else {
			fprintf(ERRLOG, "Error opening a GLFW window in windowed mode (i.e. not fullscreen)!\n");
			glfwIsInitialized = false;
			return nullptr;
		}
	}

	//I do one additional check to make sure mWindow definitly is not nullptr (this check is surpurfulous but doesn't hurt)
	if (mWindow == nullptr) {
		fprintf(ERRLOG, "Failed Creating OpenGL Context and Window\n");
		glfwIsInitialized = false;
		return nullptr;
	}
	else {
		glfwMakeContextCurrent(mWindow); //Context must be made current here due to load dependencies 
		glfwIsInitialized = true;
	}

	//return (std::move(generateDetectedMonitorsStruct()));  //Don't do this! See comment about return value for function generateDetectedMonitorsStruct() below
    return generateDetectedMonitorsStruct();
}

void GLFW_Init::specifyWindowCallbackFunctions() {
	if (mWindow) {
		

		glfwSetWindowSizeCallback(mWindow, WindowCallbackInternal::windowSizeCallback);
		glfwSetFramebufferSizeCallback(mWindow, WindowCallbackInternal::framebufferSizeCallback);
		glfwSetWindowPosCallback(mWindow, WindowCallbackInternal::windowPositionCallback);

		glfwSetWindowOpacity(mWindow, 1.0f);

		glfwSetDropCallback(mWindow, WindowCallbackInternal::filedropCallback);

		glfwSetWindowMaximizeCallback(mWindow, WindowCallbackInternal::windowMaximizeCallback);

		glfwSetJoystickCallback(WindowCallbackInternal::joystickConnectionCallback);

		//glfwSetMouseButtonCallback(mWindow, mouseButtonCallback);
		//glfwSetScrollCallback(mWindow, mouseScrollCallback);
		//glfwSetCursorEnterCallback(mWindow, curserEnterCallback);
		//glfwSetCursorPosCallback(mWindow, curserPositionCallback);
	}
}

void GLFW_Init::setWindowUserPointer(void * userPointer) {
	if (mWindow) {
		glfwSetWindowUserPointer(mWindow, userPointer);
	}
	else {
		fprintf(ERRLOG, "\nERROR setting user pointer for window! Window is NULL!!!\n");
		fprintf(ERRLOG, "\t [Press ENTER to continue]\n");
		std::cin.get();
	}
}

//Creates a struct from the members of this class
std::shared_ptr<MonitorData> GLFW_Init::generateDetectedMonitorsStruct() {
	std::shared_ptr<MonitorData> detectedDisplayData = std::make_shared<MonitorData>();
	
	detectedDisplayData->numDetected = connectedDisplayCount;
	detectedDisplayData->activeMonitorNum = defaultMonitor;
	detectedDisplayData->width = this->width;
	detectedDisplayData->height = this->height;
	detectedDisplayData->refreshRate = this->refreshRate;
	//Set monitor pointers
	detectedDisplayData->monitorArray = monitors;
	detectedDisplayData->activeMonitor = mWindow;

	detectedDisplayData->validContext = glfwIsInitialized;

	//return std::move(detectedDisplayData); //This is a terrible thing to do. See Scott Meyer's Effective Modern C++ item 25 (page 174)
    //                                     //Using a move statement here prevents Return Value Optimization / Copy Elision from being legal. 
    //Instead, just do
    return detectedDisplayData;
}

//Detects the resolution of the active display
void GLFW_Init::detectDisplayResolution(int displayNum, int& width, int& height, int& refreshRate) {
	const GLFWvidmode * mode = nullptr;
	if (displayNum == 0) {
		mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	}
	else {
		mode = glfwGetVideoMode(this->monitors[displayNum]);
	}
	//Make sure mode isn't nullptr for some reason
	if (mode != nullptr) {
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// Set screen resolution here? //(Make sure to understand the difference between viewport size and framebuffer size, GLFW's documentation explains this)
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		width = mode->width;
		height = mode->height;

		refreshRate = mode->refreshRate;
	}
	else {
		fprintf(ERRLOG, "Error! GLFW failed while communicating with your display.\nError is due to the following reason:\n"
			"\tUNABLE TO RETRIEVE MONITOR VIDEO MODE INFORMATION. PERHAPS TRY A DIFFERENT MONITOR? OR REBOOT? OR CALL I.T.\n"
			"[If you are IT and you are reading this message, then chances are something is funky with the monitor since\n"
			"it is not communicating nicely with this program]\n\n");
		this->glfwIsInitialized = false;
	}
}


