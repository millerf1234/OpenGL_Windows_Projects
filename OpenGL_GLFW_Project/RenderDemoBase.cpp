//See the header file for a description
//
// Forrest Miller
// October 4, 2018

#include "RenderDemoBase.h"


RenderDemoBase::RenderDemoBase() {
	mainRenderWindow = nullptr;
	mJoystickStatePrintingEnabled_ = false;
	mIterationsSinceLastJoystickStatePrintingLastModified_ = 0ull;
}


RenderDemoBase::~RenderDemoBase() {

}



bool RenderDemoBase::checkForContextReset() {
	//See also: https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glGet.xml
	/*
	int resetStrategy = 0;
	glGetIntegerv(GL_RESET_NOTIFICATION_STRATEGY, &resetStrategy);

	fprintf(MSGLOG, "\nReset Strategy is %d\n", resetStrategy);
	if (resetStrategy == GL_NO_RESET_NOTIFICATION) {
		fprintf(MSGLOG, "This corrosponds to NO_RESET_NOTIFICATION\n");
	}
	else if (resetStrategy == GL_LOSE_CONTEXT_ON_RESET) {
		fprintf(MSGLOG, "This corrosponds to LOSE_CONTEXT_ON_RESET\n");
	}
	else {
		fprintf(MSGLOG, "This is an unexpected return value for this function call!\n");
	}
	*/
	bool resetDetected;

	GLenum contextResetStatus = glGetGraphicsResetStatus();
	switch (contextResetStatus) {
	case GL_NO_ERROR:
		//fprintf(MSGLOG, "\nglGetGraphicsResetStatus() returned enum GL_NO_ERROR!\n");
		resetDetected = false;
		break;
	case GL_GUILTY_CONTEXT_RESET:
		fprintf(MSGLOG, "\nglGetGraphicsResetStatus() returned enum GL_GUILTY_CONTEXT_RESET!\nThis means this program caused the issue!\n");
		resetDetected = true;
		break;
	case GL_INNOCENT_CONTEXT_RESET:
		fprintf(MSGLOG, "\nglGetGraphicsResetStatus() returned enum GL_INNOCENT_CONTEXT_RESET!\nThis means something else caused the reset!\n");
		resetDetected = true;
		break;
	case GL_UNKNOWN_CONTEXT_RESET:
		fprintf(MSGLOG, "\nglGetGraphicsResetStatus() returned enum GL_UNKNOWN_CONTEXT_RESET!\n"
			"This means it is not known what happened or why, but what happend was bad!!!\n");
		resetDetected = true;
		break;
	default:
		fprintf(MSGLOG, "\nglGetGraphicsResetStatus() returned unexpected enum (i.e. DEFAULT CASE WAS CHOSEN FOR SOME REASON!)\n");
		resetDetected = false;
		break;
	}

	return resetDetected;
}


void RenderDemoBase::performRenderDemoSharedInputLogic() {
	doJoystickPrinterLoopLogic();
}


void RenderDemoBase::doJoystickPrinterLoopLogic() {
	mIterationsSinceLastJoystickStatePrintingLastModified_++;
	
	//Perform Input Checking
	if (mIterationsSinceLastJoystickStatePrintingLastModified_ > 11ull) { //Have a slight delay between when inputs are accepted

		if ((glfwGetKey(mainRenderWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) ||
			(glfwGetKey(mainRenderWindow, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)) {

			if (glfwGetKey(mainRenderWindow, GLFW_KEY_J) == GLFW_PRESS) {
				mJoystickStatePrintingEnabled_ = !mJoystickStatePrintingEnabled_;
				fprintf(MSGLOG, "\nJoystick Output Reporting Is Now ");
				if (mJoystickStatePrintingEnabled_)
					fprintf(MSGLOG, "Enabled\n");
				else
					fprintf(MSGLOG, "Disabled\n");
				mIterationsSinceLastJoystickStatePrintingLastModified_ = 0ull;
			}

			if (mJoystickStatePrintingEnabled_) {

				//Check if need to reset stored default state for joystick (this would be necessary if
				//buttons were held down when stick connected)
				if (glfwGetKey(mainRenderWindow, GLFW_KEY_K) == GLFW_PRESS) {
					joystickPrinter.reaquireDefaultState();
					mIterationsSinceLastJoystickStatePrintingLastModified_ = 0ull;
				}

				//Toggle Message Output Mode
				if (glfwGetKey(mainRenderWindow, GLFW_KEY_H)) {
					joystickPrinter.toggleMessageOutputMode();
					fprintf(MSGLOG, "\nJoystick Output Mode set to: ");
					if (joystickPrinter.isUsingEchoInputPrintMode())
						fprintf(MSGLOG, "ECHO INPUT\n");
					else
						fprintf(MSGLOG, "ECHO FULL STATE\n");
					mIterationsSinceLastJoystickStatePrintingLastModified_ = 0ull;
				}

				if (glfwGetKey(mainRenderWindow, GLFW_KEY_SEMICOLON)) {
					//increment joystick ID to poll
					joystickPrinter.nextJoystick();
					mIterationsSinceLastJoystickStatePrintingLastModified_ = 0ull;
				}

				if (glfwGetKey(mainRenderWindow, GLFW_KEY_APOSTROPHE)) {
					//derement joystick ID to poll
					joystickPrinter.previousJoystick();
					mIterationsSinceLastJoystickStatePrintingLastModified_ = 0ull;
				}
			}
		}
	}

	//Perform logic
	if (mJoystickStatePrintingEnabled_) {
		joystickPrinter.printState();
	}
}
