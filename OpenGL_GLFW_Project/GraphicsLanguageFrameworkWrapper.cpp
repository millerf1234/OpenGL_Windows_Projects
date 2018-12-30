//File:          GraphicsLanguageFrameworkWrapper.cpp
//
//   See header file for more details
//
// Programmer:    Forrest Miller
// Date:          December 2018

#include "GraphicsLanguageFrameworkWrapper.h"

#include "LoggingMessageTargets.h"
#include "GLFrameworkErrorCallback.h"   //defines global error callback function to use with GLFW library


std::unique_ptr<std::unique_ptr<GLFrameworkInternal::CurrentlyConnectedMonitors>> GraphicsLanguageFrameworkWrapper::connectedMonitors = nullptr;

void GraphicsLanguageFrameworkWrapper::giveMemberVariablesInitialValues() {
	mGLFWWasInitialized_ = false;
	mGLVersionMajor_ = 0;
	mGLVersionMinor_ = 0;
	mGLCompatabilityMode_ = false;

}

GraphicsLanguageFrameworkWrapper::GraphicsLanguageFrameworkWrapper(const int glVersionMajor,
	const int glVersionMinor, const bool useCompatabilityProfile) {
	
	giveMemberVariablesInitialValues(); //Assigns initial values to all of this object's fields

	setGLFWErrorCallback(); 

	if (!validateRuntimeGLFWVersion()) {
		return;
	}

	setInitializationHints();

	if (!initializeGLFW()) {
		mGLFWWasInitialized_ = false;
		return;
	}
	else {
		mGLFWWasInitialized_ = true;
	}

}

GraphicsLanguageFrameworkWrapper::~GraphicsLanguageFrameworkWrapper() {
	if (mGLFWWasInitialized_) {
		mGLFWWasInitialized_ = false;
		glfwTerminate();
	}
}


void GraphicsLanguageFrameworkWrapper::getRequestedGLVersion(int& versionMajor, int& versionMinor, bool& usesCompatMode) const {
	versionMajor = mGLVersionMajor_;
	versionMinor = mGLVersionMinor_;
	usesCompatMode = mGLCompatabilityMode_;
}

int GraphicsLanguageFrameworkWrapper::getRequestedGLVersionMajor() const {
	return mGLVersionMajor_;
}

int GraphicsLanguageFrameworkWrapper::getRequestedGLVersionMinor() const {
	return mGLVersionMinor_;
}

std::string GraphicsLanguageFrameworkWrapper::getRequestedGLVersionString() const {
	std::stringstream requestedGLVersionMsg;
	requestedGLVersionMsg << "OpenGL " << mGLVersionMajor_ << "." << mGLVersionMinor_;
	if (!mGLCompatabilityMode_) {
		requestedGLVersionMsg << " Core";
	}
	else {
		requestedGLVersionMsg << " Compatability";
	}
	return requestedGLVersionMsg.str();
}

void GraphicsLanguageFrameworkWrapper::getGLFWCompileTimeVersion(int& compileVersionMajor, int& compileVersionMinor, int& compileVersionRevision) const {
	compileVersionMajor = GLFW_VERSION_MAJOR;
	compileVersionMinor = GLFW_VERSION_MINOR;
	compileVersionRevision = GLFW_VERSION_REVISION;
}

std::string GraphicsLanguageFrameworkWrapper::getGLFWCompileTimeVersionString() const {
	std::stringstream compileTimeVersionMsg;
	compileTimeVersionMsg << "compiled against GLFW " << GLFW_VERSION_MAJOR << "." << GLFW_VERSION_MINOR <<
		"." << GLFW_VERSION_REVISION;
	return compileTimeVersionMsg.str();
}

void GraphicsLanguageFrameworkWrapper::getGLFWRuntimeVersion(int& runtimeVersionMajor, int& runtimeVersionMinor, int& runtimeVersionRevision) const {
	glfwGetVersion(&runtimeVersionMajor, &runtimeVersionMinor, &runtimeVersionRevision);
}

std::string GraphicsLanguageFrameworkWrapper::getGLFWRuntimeVersionString() const {
	std::stringstream runtimeVersionMsg;
	int rtMajor = 0;
	int rtMinor = 0;
	int rtRev = 0;
	glfwGetVersion(&rtMajor, &rtMinor, &rtRev);
	runtimeVersionMsg << "running against GLFW " << rtMajor << "." << rtMinor <<
		"." << rtRev;
	return runtimeVersionMsg.str();
}








void GraphicsLanguageFrameworkWrapper::setGLFWErrorCallback() {
	GLFWerrorfun  previousErrorFun = glfwSetErrorCallback(GLFrameworkInternal::graphicsLanguageFrameworkErrorCallbackFunction);
	//Question: Do GLFW applications using GLFW as a shared library (DLL) each get to set their error callback function independently?
	//Just in case there was an error callback function that was already set, probably best be safe and print out a warning that it has
	//been replaced by this application's error callback function. If GLFW is well behaved, this message will never be printed.
	if (previousErrorFun != nullptr) {
		fprintf(WRNLOG, "\nWarning! While setting up the Graphics Language Framework's error-reporting function,\n"
			"a previously set error function was replaced!\n");
	}
}

void GraphicsLanguageFrameworkWrapper::setGLVersion(const int glVersionMajor, const int glVersionMinor, const bool useCompatProfile) {
	mGLVersionMajor_ = glVersionMajor;
	mGLVersionMinor_ = glVersionMinor;
	mGLCompatabilityMode_ = useCompatProfile;
}

bool GraphicsLanguageFrameworkWrapper::validateRuntimeGLFWVersion() const {
	int runtimeVersionMajor = 0;
	int runtimeVersionMinor = 0;
	int runtimeRevision = 0;
	getGLFWRuntimeVersion(runtimeVersionMajor, runtimeVersionMinor, runtimeRevision);
	//If the major versions of GLFW do not match, then the libraries are incompatible
	if (GLFW_VERSION_MAJOR != runtimeVersionMajor) {
		fprintf(ERRLOG, "\nERROR! An issue occured while initializing Graphics Language Framework (GLFW) library!\n"
			"This application was expecting to link with GLFW %s, but the GLFW shared-library that this executable linked with at run\n"
			"time is GLFW %d.%d.%d\n"
			"Unfortunatly these two versions are incompatible!\n"
			"   [A possible fix may be finding and building the correct GLFW '.dll' matching the\n"
			"    version expected by this application]\n", getGLFWCompileTimeVersionString().c_str(), runtimeVersionMajor,
			runtimeVersionMinor, runtimeRevision);
		return false;
	}
	//Compatability shouldn't be an issue between differing minor versions of GLFW, but printing a warning message probably wouldn't hurt
	else if (GLFW_VERSION_MINOR != runtimeVersionMinor) {
		fprintf(WRNLOG, "\nWARNING! This application was compiled against Graphics Language Framework (GLFW) Library %d.%d.%d\n"
			"while the Graphics Language Framework (GLFW) Library that was linked against at\n"
			"runtime is GLFW %d.%d.%d\n"
			"If unusual behavior is displayed by this application, a potential cause may be attributed to this version mismatch!\n",
			GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION, runtimeVersionMajor, runtimeVersionMinor,
			runtimeRevision);
	}
	return true;
}

void GraphicsLanguageFrameworkWrapper::setInitializationHints() const {
	//Set all GLFW hints that will affect GLFW's initialization. These must be set before
	//the function 'glfwInit()' is called.
	//Luckily, as of GLFW 3.3, the only relevant initialzation hint to set is just a minor detail
	//affecting how GLFW reports the state of connected Joysticks (i.e. controllers).

	//We want GLFW to report Joystick Hat inputs seperately in their own 'Hats' caragory instead of
	//bunching them in with the normal button reporting. 
	glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_FALSE); 

	//There are also a few macOS-specific hints, which are not relevant once macOS stopped really
	//supporting OpenGL
}

bool GraphicsLanguageFrameworkWrapper::initializeGLFW() {
	int glfwInitSuccess = glfwInit();
	if (glfwInitSuccess == GLFW_FALSE) {
		return false;
	}
	return true;
}

int GraphicsLanguageFrameworkWrapper::countAvailableMonitors() const {
	int count = 0;
	glfwGetMonitors(&count);
	return count;
}


