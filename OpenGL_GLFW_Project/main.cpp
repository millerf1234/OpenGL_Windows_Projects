#include <iostream> 

//GLFW version 3.3

#include "ProjectParameters.h"

using glm::vec3;

int main() {

	vec3 x(0.0f, 0.0f, 1.0f);

	std::cout << "Hello World!\n";
	std::cout << "X is " << x.x << ", " << x.y << ", " << x.z << std::endl;
	std::cin.get();
	return EXIT_SUCCESS;
}