//main.h
//
//Create the application and launch it
//

#include "Application.h"


int main() {
	std::unique_ptr<Application> app = std::make_unique<Application>(); 
	if (app) 
		app->launch();
}