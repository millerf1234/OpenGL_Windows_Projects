



#include "Application.h"


int main() {

	//Create the application and launch it
	std::unique_ptr<Application> app = std::make_unique<Application>(); 
	if (app) 
		app->launch();
	
	return EXIT_SUCCESS;

}