//See the header file for a description
//
// Forrest Miller
// October 4, 2018

#include "RenderDemoBase.h"


RenderDemoBase::RenderDemoBase() {
	mainRenderWindow = nullptr;
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
