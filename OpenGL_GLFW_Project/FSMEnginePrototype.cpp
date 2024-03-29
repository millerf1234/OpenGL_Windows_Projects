//File:          FSMEnginePrototype.cpp
//
//   See header file for more details
//
// Programmer:    Forrest Miller
// Created:       December 2018 through January 2019 

#include "FSMEnginePrototype.h"
#include "LoggingMessageTargets.h"
#include "FSMCallbackInitializer.h"  //Provides functions which handle assigning GLFW callback functions.


void FSMEnginePrototype::giveMemberVariablesInitialValues() {
	mGLFWWasInitialized_ = false;
	mGLVersionMajor_ = 0;
	mGLVersionMinor_ = 0;
	mGLCompatabilityMode_ = false;
}

FSMEnginePrototype::FSMEnginePrototype(const int glVersionMajor,
	const int glVersionMinor, const bool useCompatabilityProfile) {
	
	giveMemberVariablesInitialValues(); 

	//Set up the relevant Callback functions for before GLFW initialization occurs 
	///FSMEngineInternal::configurePreInitializationCallbackFunctions(); 

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

	//After GLFW has been successfully initialized, set up the relevant callbacks for post-initialization
	///FSMEngineInternal::configurePostInitializationCallbackFunctions(); 

	//Since we now have the GLFW layer initialized, use it to query the underlying operating system
	//to determine what available components we have to work with by enumerating them and storing what
	//we get into our member storage objects. 
	aquireGLFrameworkComponents();
}

FSMEnginePrototype::~FSMEnginePrototype() {

	//initialize GLFW components

	if (mGLFWWasInitialized_) {
		mGLFWWasInitialized_ = false;
		glfwTerminate();
	}
}


void FSMEnginePrototype::getRequestedGLVersion(int& versionMajor, int& versionMinor, bool& usesCompatMode) const {
	versionMajor = mGLVersionMajor_;
	versionMinor = mGLVersionMinor_;
	usesCompatMode = mGLCompatabilityMode_;
}

int FSMEnginePrototype::getRequestedGLVersionMajor() const {
	return mGLVersionMajor_;
}

int FSMEnginePrototype::getRequestedGLVersionMinor() const {
	return mGLVersionMinor_;
}

std::string FSMEnginePrototype::getRequestedGLVersionString() const {
	std::stringstream requestedGLVersionMsg;
	requestedGLVersionMsg << "OpenGL " << mGLVersionMajor_ << "." << mGLVersionMinor_;
	if (!mGLCompatabilityMode_) {
		requestedGLVersionMsg << " Core";
	}
	else {
		requestedGLVersionMsg << " Compatibility";
	}
	return requestedGLVersionMsg.str();
}

void FSMEnginePrototype::getGLFWCompileTimeVersion(int& compileVersionMajor, int& compileVersionMinor, int& compileVersionRevision) const {
	compileVersionMajor = GLFW_VERSION_MAJOR;
	compileVersionMinor = GLFW_VERSION_MINOR;
	compileVersionRevision = GLFW_VERSION_REVISION;
}

std::string FSMEnginePrototype::getGLFWCompileTimeVersionString() const {
	std::stringstream compileTimeVersionMsg;
	compileTimeVersionMsg << "compiled against GLFW " << GLFW_VERSION_MAJOR << "." << GLFW_VERSION_MINOR <<
		"." << GLFW_VERSION_REVISION;
	return compileTimeVersionMsg.str();
}

void FSMEnginePrototype::getGLFWRuntimeVersion(int& runtimeVersionMajor, int& runtimeVersionMinor, int& runtimeVersionRevision) const {
	glfwGetVersion(&runtimeVersionMajor, &runtimeVersionMinor, &runtimeVersionRevision);
}

std::string FSMEnginePrototype::getGLFWRuntimeVersionString() const {
	std::stringstream runtimeVersionMsg;
	int rtMajor = 0;
	int rtMinor = 0;
	int rtRev = 0;
	glfwGetVersion(&rtMajor, &rtMinor, &rtRev);
	runtimeVersionMsg << "running against GLFW " << rtMajor << "." << rtMinor <<
		"." << rtRev;
	return runtimeVersionMsg.str();
}





void FSMEnginePrototype::setGLVersion(const int glVersionMajor, const int glVersionMinor, const bool useCompatProfile) {
	mGLVersionMajor_ = glVersionMajor;
	mGLVersionMinor_ = glVersionMinor;
	mGLCompatabilityMode_ = useCompatProfile;
}

bool FSMEnginePrototype::validateRuntimeGLFWVersion() const {
	int runtimeVersionMajor = 0;
	int runtimeVersionMinor = 0;
	int runtimeRevision = 0;
	getGLFWRuntimeVersion(runtimeVersionMajor, runtimeVersionMinor, runtimeRevision);
	//If the major versions of GLFW do not match, then the libraries are incompatible
	if (GLFW_VERSION_MAJOR != runtimeVersionMajor) {
		fprintf(ERRLOG, "\nERROR! An issue occurred while initializing Graphics Language Framework (GLFW) library!\n"
			"This application was expecting to link with GLFW %s, but the GLFW shared-library that this executable linked with at run\n"
			"time is GLFW %d.%d.%d\n"
			"Unfortunately these two versions are incompatible!\n"
			"   [A possible fix may be finding and building the correct GLFW '.dll' matching the\n"
			"    version expected by this application]\n", getGLFWCompileTimeVersionString().c_str(), runtimeVersionMajor,
			runtimeVersionMinor, runtimeRevision);
		return false;
	}
	//Compatibility shouldn't be an issue between differing minor versions of GLFW, but printing a warning message probably wouldn't hurt
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

void FSMEnginePrototype::setInitializationHints() const {
	//Set all GLFW hints that will affect GLFW's initialization. These must be set before
	//the function 'glfwInit()' is called.
	//Luckily, as of GLFW 3.3, the only relevant initialization hint to set is just a minor detail
	//affecting how GLFW reports the state of connected Joysticks (i.e. controllers).

	//We want GLFW to report Joystick Hat inputs separately in their own 'Hats' category instead of
	//bunching them in with the normal button reporting. 
	glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_FALSE); 

	//There are also a few macOS-specific hints, which are not relevant once macOS stopped really
	//supporting OpenGL
}

bool FSMEnginePrototype::initializeGLFW() {
	int glfwInitSuccess = glfwInit();
	if (glfwInitSuccess == GLFW_FALSE) {
		return false;
	}
	return true;
}


void FSMEnginePrototype::aquireGLFrameworkComponents() {


}

//int FSMEnginePrototype::aquireAllAvailableMonitors() const {
//	int count = 0;
//	glfwGetMonitors(&count);
//	return count;
//}


