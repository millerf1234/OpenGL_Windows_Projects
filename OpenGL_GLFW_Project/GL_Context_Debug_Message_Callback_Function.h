// This file contains the callback function that makes debugging and
// optimizing OpenGL programs easier. 
//
//Added to project on August 28, 2018.
//By Forrest Miller
//
//This code is based heavily off the example code provided
//at https://www.khronos.org/opengl/wiki/Debug_Output
//

#pragma once

#ifndef GL_CONTEXT_DEBUG_MESSAGE_CALLBACK_FUNCTION_H_
#define GL_CONTEXT_DEBUG_MESSAGE_CALLBACK_FUNCTION_H_

//void GLAPIENTRY printGraphicsContextMessageCallback(GLenum source,
//	GLenum type,
//	GLuint id,
//	GLenum severity,
//	GLsizei length,
//	const GLchar* message,
//	const void* userParam);


//The detail provided in this message could use a more detailed implementation
static void GLAPIENTRY printGraphicsContextMessageCallback(GLenum source,
														   GLenum type,
														   GLuint id,
														   GLenum severity,
														   GLsizei length,
														   const GLchar* message,
														   const void* userParam)  {
	fprintf(ERRLOG, "\n********************************************************************************\n"); //80 characters
	fprintf(ERRLOG, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
	fprintf(ERRLOG, "\n********************************************************************************\n"); //80 characters

}






#endif //GL_CONTEXT_DEBUG_MESSAGE_CALLBACK_FUNCTION_H_
