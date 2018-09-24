//main.h
//
//Create the application and launch it
//
// Forrest Miller           July 2018

#include "Application.h"

int main() {
	std::unique_ptr<Application> app = std::make_unique<Application>(); 
	app->launch();
}