// File:                GL_Context_Debug_Message_Callback_Function.h
//
// Description:         This file defines a function that matches the signature of the Debug
//                      Callback Function an OpenGL context uses for communicating messages. 
//                      Essentially this function provides a format to use to allow OpenGL to
//                      convey information to the application. This function must be provided
//                      to OpenGL through the use of the function glDebugMessageCallback(). 
//                      Furthermore, debugging must be enabled in the GL Context through the 
//                      function call glEnable(GL_DEBUG_CONTEXT). Furthmore, for these messages  
//                      to be synchronus with the events in the application that caused them, 
//                      the function call glEnable(GL_DEBUG_OUTPUT_SYNCHRONUS) must be made.
//                      There is more to ensuring the GL Context is a debug context than is 
//                      mentioned here, but those details go beyond the scope of this application.
//
// Programmer:          Forrest Miller
// Dates:               July 2018       First Implemented a Callback Function
//                      November 2018   Rewrote Callback Function to be much more detailed
//                      
//


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UPDATE/HACK: I realized after writing this that if synchronization is not forced between 
//              the GL Context and the Application, then it would be possible to get asynchronus
//              calls to fprintf occuring from both the application and this callback function. Thus to fix it
//              I have a hack to divert this functions print calls to a buffer which can then be printed at the 
//              very end.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef GL_CONTEXT_DEBUG_MESSAGE_CALLBACK_FUNCTION_H_
#define GL_CONTEXT_DEBUG_MESSAGE_CALLBACK_FUNCTION_H_

#include <stdio.h>
#include "ProjectSetup.h"
#include "LoggingMessageTargets.h"

//-------------------------------------------------------------------------------------
//   ALERT!!!   HACK IN PROGRESS
//   This hack will divert the outputs of the print statements to a temporary buffer as the 
//   function executes, after which it will actually print the message out in one call to fprintf.
//   I undo this hack at the bottom of this file.
//
//  (Why this works?   See: https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html      (hopefully this is portable)
//
// First define the following 2 macros 
#define DEBUG_CALLBACK_BUFFER_NAME msgBuffer
#define DEBUG_CALLBACK_BUFFER_SIZE 2048       

//  Redefine the following
#define fprintf(x, ...) snprintf(DEBUG_CALLBACK_BUFFER_NAME, DEBUG_CALLBACK_BUFFER_SIZE,  __VA_ARGS__)
//#define fprintf(MSGLOG, ...) snprintf(DEBUG_CALLBACK_BUFFER_NAME, DEBUG_CALLBACK_BUFFER_SIZE,  __VA_ARGS__)
//#define fprintf(WRNLOG, ...) snprintf(DEBUG_CALLBACK_BUFFER_NAME, DEBUG_CALLBACK_BUFFER_SIZE,  __VA_ARGS__)
//#define fprintf(ERRLOG, ...) snprintf(DEBUG_CALLBACK_BUFFER_NAME, DEBUG_CALLBACK_BUFFER_SIZE,  __VA_ARGS__)
//-------------------------------------------------------------------------------------



//The detail provided in this message could use a more detailed implementation
static void GLAPIENTRY printGraphicsContextMessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam) {
	//Print a message based off severity 
	if ((severity == GL_DEBUG_SEVERITY_HIGH) || (severity == GL_DEBUG_SEVERITY_MEDIUM)) {

		//////////////////////////////////////////////////////////////////////////////
		// As part of the hack, we need a buffer 
		char DEBUG_CALLBACK_BUFFER_NAME[DEBUG_CALLBACK_BUFFER_SIZE];
		//////////////////////////////////////////////////////////////////////////////
		



		//Message Format depends on the message's source

		//-------------------------------------------------------------
		// Third-Party       Medium-High Priority      Call Back Message
		//-------------------------------------------------------------
		if (source == GL_DEBUG_SOURCE_THIRD_PARTY) {
			fprintf(ERRLOG, "\n<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>\n");
			fprintf(ERRLOG, "Third Party CALLBACK ( ID = 0x%05X,  Severity = ", id); //strlen is 44
			if (GL_DEBUG_SEVERITY_HIGH) {
				fprintf(ERRLOG, "HIGH! ) "); //total strlen now is 52
			}
			else {
				fprintf(ERRLOG, "MEDIUM )"); //total strlen now is 52
			}
			switch (type) {   //The intention here is to have 'type' be right justified on same line
			case (GL_DEBUG_TYPE_ERROR):
				fprintf(ERRLOG, "%28s", "[type = ERROR]\n");
				break;
			case (GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR):
				fprintf(ERRLOG, "%28s", "[type = DEPRECATED BEHAVIOR]\n");
				break;
			case (GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR):
				fprintf(ERRLOG, "%28s", "[type = UNDEFINED BEHAVIOR]\n");
				break;
			case (GL_DEBUG_TYPE_PERFORMANCE):
				fprintf(ERRLOG, "%28s", "[type = PERFORMANCE]\n");
				break;
			case (GL_DEBUG_TYPE_PORTABILITY):
				fprintf(ERRLOG, "%28s", "[type = PORTABILITY]\n");
				break;
			case (GL_DEBUG_TYPE_MARKER):
				fprintf(ERRLOG, "%28s", "[type = MARKER]\n");
				break;
			case (GL_DEBUG_TYPE_PUSH_GROUP):
				fprintf(ERRLOG, "%28s", "[type = PUSH GROUP]\n");
				break;
			case (GL_DEBUG_TYPE_POP_GROUP):
				fprintf(ERRLOG, "%28s", "[type = POP GROUP]\n");
				break;
			case (GL_DEBUG_TYPE_OTHER):
				fprintf(ERRLOG, "%28s", "[type = UNKNOWN]\n");
				break;
			default:
				fprintf(ERRLOG, "%28s", "[type = INVALID_ENUM]\n");
				break;
			}

			fprintf(ERRLOG, "%s", message);
			fprintf(ERRLOG, "\n<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>\n");
		}

		//-------------------------------------------------------------
		// Application       Medium-High Priority      Call Back Message
		//-------------------------------------------------------------

		else if (source == GL_DEBUG_SOURCE_APPLICATION) {
			fprintf(ERRLOG, "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
			fprintf(ERRLOG, "Application CALLBACK ( ID = 0x%05X,  Severity = ", id); //strlen is 44
			if (GL_DEBUG_SEVERITY_HIGH) {
				fprintf(ERRLOG, "HIGH! ) "); //total strlen now is 52
			}
			else {
				fprintf(ERRLOG, "MEDIUM )"); //total strlen now is 52
			}
			switch (type) {   //The intention here is to have 'type' be right justified on same line 
			case (GL_DEBUG_TYPE_ERROR):
				fprintf(ERRLOG, "%28s", "[type = ERROR]\n");
				break;
			case (GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR):
				fprintf(ERRLOG, "%28s", "[type = DEPRECATED BEHAVIOR]\n");
				break;
			case (GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR):
				fprintf(ERRLOG, "%28s", "[type = UNDEFINED BEHAVIOR]\n");
				break;
			case (GL_DEBUG_TYPE_PERFORMANCE):
				fprintf(ERRLOG, "%28s", "[type = PERFORMANCE]\n");
				break;
			case (GL_DEBUG_TYPE_PORTABILITY):
				fprintf(ERRLOG, "%28s", "[type = PORTABILITY]\n");
				break;
			case (GL_DEBUG_TYPE_MARKER):
				fprintf(ERRLOG, "%28s", "[type = MARKER]\n");
				break;
			case (GL_DEBUG_TYPE_PUSH_GROUP):
				fprintf(ERRLOG, "%28s", "[type = PUSH GROUP]\n");
				break;
			case (GL_DEBUG_TYPE_POP_GROUP):
				fprintf(ERRLOG, "%28s", "[type = POP GROUP]\n");
				break;
			case (GL_DEBUG_TYPE_OTHER):
				fprintf(ERRLOG, "%28s", "[type = UNKNOWN]\n");
				break;
			default:
				fprintf(ERRLOG, "%28s", "[type = INVALID_ENUM]\n");
				break;
			}

			fprintf(ERRLOG, "%s", message);
			fprintf(ERRLOG, "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"); //__ characters
		}



		//-------------------------------------------------------------
		// GL Context       Medium-High Priority      Call Back Message
		//-------------------------------------------------------------

		else if (source != GL_DEBUG_SOURCE_OTHER) {
			fprintf(ERRLOG, "\n*******************************************************************************\n"); //80 characters
			fprintf(ERRLOG, "GL Context CALLBACK ( ID = 0x%05X,  Severity = ", id); //strlen is 43
			if (GL_DEBUG_SEVERITY_HIGH) {
				fprintf(ERRLOG, "HIGH! )  "); //total strlen now is 52  (extra spaces at end of string are intentional)
			}
			else {
				fprintf(ERRLOG, "MEDIUM ) "); //total strlen now is 52
			}

			switch (type) {   //The intention here is to have 'type' be right justified on same line 
			case (GL_DEBUG_TYPE_ERROR):
				fprintf(ERRLOG, "%28s", "[type = ERROR]\n");
				break;
			case (GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR):
				fprintf(ERRLOG, "%28s", "[type = DEPRECATED BEHAVIOR]\n");
				break;
			case (GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR):
				fprintf(ERRLOG, "%28s", "[type = UNDEFINED BEHAVIOR]\n");
				break;
			case (GL_DEBUG_TYPE_PERFORMANCE):
				fprintf(ERRLOG, "%28s", "[type = PERFORMANCE]\n");
				break;
			case (GL_DEBUG_TYPE_PORTABILITY):
				fprintf(ERRLOG, "%28s", "[type = PORTABILITY]\n");
				break;
			case (GL_DEBUG_TYPE_MARKER):
				fprintf(ERRLOG, "%28s", "[type = MARKER]\n");
				break;
			case (GL_DEBUG_TYPE_PUSH_GROUP):
				fprintf(ERRLOG, "%28s", "[type = PUSH GROUP]\n");
				break;
			case (GL_DEBUG_TYPE_POP_GROUP):
				fprintf(ERRLOG, "%28s", "[type = POP GROUP]\n");
				break;
			case (GL_DEBUG_TYPE_OTHER):
				fprintf(ERRLOG, "%28s", "[type = OTHER]\n");
				break;
			default:
				fprintf(ERRLOG, "%28s", "[type = INVALID_ENUM]\n");
				break;
			}

			switch (source) {
			case (GL_DEBUG_SOURCE_API):
				fprintf(ERRLOG, "[Source: GL API]\n");
				break;
			case (GL_DEBUG_SOURCE_WINDOW_SYSTEM):
				fprintf(ERRLOG, "[Source: WINDOW SYSTEM]\n");
				break;
			case (GL_DEBUG_SOURCE_SHADER_COMPILER):
				fprintf(ERRLOG, "[Source: SHADER COMPILER]\n");
				break;
			default:
				fprintf(ERRLOG, "[Source: UNKNOWN ENUM]\n");
				break;
			}

			fprintf(ERRLOG, "%s", message);
			fprintf(ERRLOG, "\n*******************************************************************************\n"); //80 characters
		}

		//-------------------------------------------------------------
		// UNKNOWN SOURCE       Medium-High Priority      Call Back Message
		//-------------------------------------------------------------

		else {
			fprintf(ERRLOG, "\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
			fprintf(ERRLOG, "UNKNOWN SOURCE CALLBACK!!! PRIORITY IS ");
			if (GL_DEBUG_SEVERITY_HIGH) {
				fprintf(ERRLOG, "HIGH!\n");
			}
			else {
				fprintf(ERRLOG, "MEDIUM\n");
			}
			fprintf(ERRLOG, "\nMessage is: %s\n", message);
			fprintf(ERRLOG, "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		}

	}

	//-------------------------------------------------------------
	// ANY SOURCE               Low Priority           Call Back Message
	//-------------------------------------------------------------

	else if (severity == GL_DEBUG_SEVERITY_LOW) {


		fprintf(MSGLOG, "\n-------------------------------------------------------------------------------\n");
		fprintf(MSGLOG, "\n[LOW PRIORITY] GL CALLBACK (Source = ");

		switch (source) {
		case GL_DEBUG_SOURCE_API:   //The source is from direct usage of OpenGL API
			fprintf(MSGLOG, "OpenGL API, ");
			break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			fprintf(MSGLOG, "Window System, ");
			break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			fprintf(MSGLOG, "Shader Compiler, ");
			break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			fprintf(MSGLOG, "Third Party, ");    //Message originated from a third-party source 
			break;
		case GL_DEBUG_SOURCE_APPLICATION:
			fprintf(MSGLOG, "Application, ");
			break;
		case GL_DEBUG_SOURCE_OTHER:
			fprintf(MSGLOG, "OTHER, ");
			break;
		default:
			fprintf(MSGLOG, "UNKNOWN, ");
			break;
		}

		fprintf(MSGLOG, "ID = 0x%05X, Type = ", id);


		switch (type) {   //The intention here is to have 'type' be right justified on same line 
		case (GL_DEBUG_TYPE_ERROR):
			fprintf(MSGLOG, "%s", "ERROR)\n");
			break;
		case (GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR):
			fprintf(MSGLOG, "%s", "DEPRECATED BEHAVIOR)\n");
			break;
		case (GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR):
			fprintf(MSGLOG, "%s", "UNDEFINED BEHAVIOR)\n");
			break;
		case (GL_DEBUG_TYPE_PERFORMANCE):
			fprintf(MSGLOG, "%s", "PERFORMANCE)\n");
			break;
		case (GL_DEBUG_TYPE_PORTABILITY):
			fprintf(MSGLOG, "%s", "PORTABILITY)\n");
			break;
		case (GL_DEBUG_TYPE_MARKER):
			fprintf(MSGLOG, "%s", "MARKER)\n");
			break;
		case (GL_DEBUG_TYPE_PUSH_GROUP):
			fprintf(MSGLOG, "%s", "PUSH GROUP)\n");
			break;
		case (GL_DEBUG_TYPE_POP_GROUP):
			fprintf(MSGLOG, "%s", "POP GROUP)\n");
			break;
		case (GL_DEBUG_TYPE_OTHER):
			fprintf(MSGLOG, "%s", "OTHER)\n");
			break;
		default:
			fprintf(MSGLOG, "%s", "INVALID_ENUM)\n");
			break;
		}
		fprintf(MSGLOG, "%s", message);
		fprintf(MSGLOG, "\n-------------------------------------------------------------------------------\n");
	}



	//-------------------------------------------------------------
	// ANY SOURCE                Context Notification        Call Back Message
	//-------------------------------------------------------------


	else {
		fprintf(MSGLOG, "\n*******************   GL Context Notification    *******************\n");
		fprintf(MSGLOG, "[ID = 0x%05X, Type = ", id);
		switch (type) {
		case (GL_DEBUG_TYPE_ERROR):
			fprintf(MSGLOG, "ERROR]");
			break;
		case (GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR):
			fprintf(MSGLOG, "DEPRECATED_BEHAVIOR]");
			break;
		case (GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR):
			fprintf(MSGLOG, "UNDEFINED_BEHAVIOR]");
			break;
		case (GL_DEBUG_TYPE_PERFORMANCE):
			fprintf(MSGLOG, "PERFORMANCE]");
			break;
		case (GL_DEBUG_TYPE_PORTABILITY):
			fprintf(MSGLOG, "PORTABILITY]");
			break;
		case (GL_DEBUG_TYPE_MARKER):
			fprintf(MSGLOG, "MARKER]");
			break;
		case (GL_DEBUG_TYPE_PUSH_GROUP):
			fprintf(MSGLOG, "PUSH_GROUP]");
			break;
		case (GL_DEBUG_TYPE_POP_GROUP):
			fprintf(MSGLOG, "POP_GROUP]");
			break;
		case (GL_DEBUG_TYPE_OTHER):
			fprintf(MSGLOG, "OTHER]");
			break;
		default:
			fprintf(MSGLOG, "INVALID TYPE ENUM!!!]");
			break;
		}

		fprintf(MSGLOG, " MSG: %s\n", message);
	}


}

			//switch (type) {
			//case (GL_DEBUG_TYPE_ERROR):
	        //
			//	break;
			//case (GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR):
	        //
			//	break;
			//case (GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR):
	        //
			//	break;
			//case (GL_DEBUG_TYPE_PERFORMANCE):
	        //
			//	break;
			//case (GL_DEBUG_TYPE_PORTABILITY):
	        //
			//	break;
			//case (GL_DEBUG_TYPE_MARKER):
	        //
			//	break;
			//case (GL_DEBUG_TYPE_PUSH_GROUP):
	        //
			//	break;
			//case (GL_DEBUG_TYPE_POP_GROUP):
	        //
			//	break;
			//case (GL_DEBUG_TYPE_OTHER):
	        //
			//	break;
			//default:
	        //
			//	break;
			//}
		
	




#endif //GL_CONTEXT_DEBUG_MESSAGE_CALLBACK_FUNCTION_H_

