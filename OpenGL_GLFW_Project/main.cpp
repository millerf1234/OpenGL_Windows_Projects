#include <iostream> 



#include "Application.h"



using glm::vec3;

int main() {

	/*  //Old test main
	vec3 x(0.0f, 0.0f, 1.0f);
	std::cout << "Hello World!\n";
	std::cout << "X is " << x.x << ", " << x.y << ", " << x.z << std::endl;
	std::cin.get();
	*/ 

	std::unique_ptr<Application> app = std::make_unique<Application>();  //Create an instance of the application
	if (app) {
		app->run();
	}
	return EXIT_SUCCESS;
}