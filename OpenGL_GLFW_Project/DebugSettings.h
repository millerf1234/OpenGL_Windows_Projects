// File:                   DebugSettings
//
// Description:            Defines macros that determine how much debugging is to be  
//                         done throughout the project. This is a global include.
//
// Date:                   January 2019

#pragma once

#ifndef DEBUG_SETTINGS_H___
#define DEBUG_SETTINGS_H___





//This macro is used to enable/disable debugging across the entire project. Turning this
//macro on will cause the OpenGL context to operate in a 'debug' mode. Also, if the 
//macro 'USE_DEBUG_' is defined, then many places within the code will have extra debug
//logic included that will report unexpected or unusual occurances (e.g. if a switch statement 
//results in the 'default' condition being exectued). Leaving 'USE_DEBUG_' undefined (by commenting
//it out) will prevent many unexpected events from being reported but should result in better 
//performance.
#define USE_DEBUG_

//The macro 'FORCE_DEBUG_CONTEXT_APP_SYNCHRONIZATION_' specifically affects the behavior of the
//OpenGL context. Defining this macro will cause the context to be synchronized with the application
//so that any messages/warnings/errors reported by the context will be reported close to the code/operation
//that caused them. Leaving this macro undefined will allow the context to operate asynchronously, which 
//could cause a dramatic performance boost but could also result in context messages to be reported 
//long after the Application event that triggered them. It is recommended to leave this enabled while
//debugging unless absolutly necessary. 
//This macro will only be used if the 'USE_DEBUG_' macro is defined.
#define FORCE_DEBUG_CONTEXT_APP_SYNCHRONIZATION_






#endif //DEBUG_SETTINGS_H___
