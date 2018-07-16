#include "Application.h"



Application::Application() {



	//Copyed from the main() of my space game project:
	/*
	GLFW_Init windowSetupRoutines; //Create a GLFW_Init object to handle window creation
								   //windowSetupRoutines.openWindowed();//Uncomment this line to open program in windowed mode
	windowSetupRoutines.setDefaultMonitor(DEFAULT_MONITOR);//Monitors start at 0, so 1 is a secondary display plugged in
	MonitorData detectedDisplayInfo = windowSetupRoutines.initialize(); //Run window creation routines

	if (detectedDisplayInfo.validContext != true) { //If something went wrong in the window setup routines
		std::cout << "\n\nAn error occured while setting the context for the display. Program will now crash... \n";
		return EXIT_FAILURE;
	}

	//Load OpenGL functions once window context has been set
	std::cout << "\nLoading Graphics Language..." << std::endl;
	gladLoadGL();

	fprintf(stderr, "    Graphics Language loaded.\n    Graphics Language version: OpenGL %s", glGetString(GL_VERSION));
	//set OpenGl global state parameters
	fprintf(stderr, "\nInitializing GL StateMachine...\n");
	std::cout << "    Activating GL depth-test\n";
	glEnable(GL_DEPTH_TEST); //Turn on the depth test for z-culling
							 //std::cout << "    Activating GL scissor-test\n";
							 //glEnable(GL_SCISSOR_TEST);


							 //looks like glGetString comes with a terminating new line. To remove this newline, do:
							 //      str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());  //where str is a string

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


Application::~Application() {

}



void Application::run() {
	std::cout << "\nApplication is running!\n";
	std::cin.get();
	return;
}