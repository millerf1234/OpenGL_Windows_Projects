//File:                main.cpp
//
//Description:         Create the application and launch it
//
//Programmer:          Forrest Miller 
//Date:                July 2018

#include "Application.h"

int main(int argc, char ** argv) {
	std::unique_ptr<Application> app = std::make_unique<Application>(); 
	app->launch();
}