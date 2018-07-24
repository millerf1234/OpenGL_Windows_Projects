
//Random note on Open-Source library installer that I wasn't sure where else to put:
//see: https://github.com/Microsoft/vcpkg/blob/master/README.md  

//On full-program optimization, read the article linked at: https://msdn.microsoft.com/en-us/magazine/dn904673.aspx


#include "Application.h"

////These headers are for the debug code and can be deleted:
//#include "UniformLocationTracker.h"
//#include "ShaderProgram.h"


int main() {
	////Debug test (delete this code eventually):
	/* UniformLocationTracker * temp = new UniformLocationTracker(2);
	temp->updateUniform1i("temp", 3);
	std::cout << "\nSize: " << temp->getNumberOfTrackedUniforms() << "\n";
	delete temp;
	temp = nullptr;
	*/
	


	//Create the application and launch it
	std::unique_ptr<Application> app = std::make_unique<Application>(); 
	if (app) 
		app->launch();
	
	return EXIT_SUCCESS;

}