// File:                       FSMCallbackInitializer.cpp
//
//
//  Description:               Contains the implementation details which occur when setting up 
//                             the GLFW callback functions. See the header file to get the 
//                             functions which should be called outside this class.
//
//  Programmer:                Forrest Miller
//  Date:                      January 2, 2019
//

#include "FSMCallbackInitializer.h"



namespace FSMEngineInternal {


	FSMCallbackInitializer::FSMCallbackInitializer() {

	}

	struct FSMCallbackInitializer::Callback_State {
		bool test;
	};

/*

	//////////////////////////////////////////
	/////  Internal Function Prototypes  /////
	//////////////////////////////////////////
	namespace FSMCallbackImplementation {
		//Pre-initialization
		void setErrorCallback();
		//Post-initialization
		void setMonitorEventCallback();

	} //namespace FSMCallbackImplementation



    //----------------------------------------------------------------------------------------



	///////////////////////////////////////////////////////////////
	/////    Implementation for Functions listed in header    /////
	///////////////////////////////////////////////////////////////

	//To be called before GLFW is initialized
	void configurePreInitializationCallbackFunctions() {
		FSMCallbackImplementation::setErrorCallback();
	}


	//To be called after GLFW has been successfully initialized
	void configurePostInitializationCallbackFunctions() {
		FSMCallbackImplementation::setMonitorEventCallback();

	}



	//----------------------------------------------------------------------------------------


    ///////////////////////////////////////////////////////////////////
	//////  Implementation of Internal Callback Setup Functions  //////
	///////////////////////////////////////////////////////////////////

	namespace FSMCallbackImplementation {
		
		void setErrorCallback() {
			GLFWerrorfun previousErrorFun = glfwSetErrorCallback(FSMErrorCallbackFunction);
			
			//Question: Do GLFW applications using GLFW as a shared library (DLL) each get to set their error callback function independently?
			//Just in case there was an error callback function that was already set, probably best be safe and print out a warning that it has
			//been replaced by this application's error callback function. If GLFW is well behaved, this message will never be printed.
			if (previousErrorFun != nullptr) {
				fprintf(WRNLOG, "\nWarning! While setting up the Graphics Language Framework's error-reporting function,\n"
					"a previously set error function was replaced!\n");
			}
		}
	
		void setMonitorEventCallback() {
			glfwSetMonitorCallback(MonitorEventCallbackInternal::graphicsLanguageFrameworkMonitorEventCallbackFunction);
		}


	
	} //namespace FSMCallbackImplementation
*/

} //namespace FSMEngineInternal


