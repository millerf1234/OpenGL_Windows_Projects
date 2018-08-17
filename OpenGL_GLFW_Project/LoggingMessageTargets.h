//For logging 
//
//   Since the vast majority of other files in this project will use logging, 
//   it just makes sense to put these in their own file.

//July and August 2018          Forrest Miller

#pragma once

#ifndef LOGGING_MESSAGE_TARGET_CONFIGURATION_H_
#define LOGGING_MESSAGE_TARGET_CONFIGURATION_H_

//The Message log is the target for Application messages which occur
//over the course of normal/expected program execution. 
#ifndef MSGLOG
#define MSGLOG stdout
#endif

//This log is the target for error messages 
#ifndef ERRLOG
#define ERRLOG stderr
#endif 


//The Warning log is written to when an unexpected (but not illegal) event/input/request
//is made within program execution. This also can include common pitfalls that might 
//actually be intended behavior, these messages are intended to be tweaked and 
//activated/deactivated during the course of development.
#ifndef WRNLOG
#define WRNLOG stderr
#endif 




////////////////////////////////////////
////        Case Enum Logger        ////  
////////////////////////////////////////

//First we need a macro to print the enum
//This is based off of the top answer at ////see top answer at https://stackoverflow.com/questions/9907160/how-to-convert-enum-names-to-string-in-c
#define EXTRACT_ENUM_(x) #x

//We need a second macro function to get the proper behavior
#define ENUM_TO_STRING(e) EXTRACT_ENUM_(e)

#define LOG_ENUM(a) fprintf(MSGLOG, "%s", ENUM_TO_STRING(a))

//The CASE_ENUM macro function is intended to be used within switch statements.
//
//The CASE_ENUM macro is based off of an example written by Peter Dalton, Evans & Sutherland
//as section 1.2 in the book Game Programming Gems 2 (2001). Their example can be found on
//page 19. 
#define CASE_ENUM(val)       case(val) : LOG_ENUM( #val )     
						      

//Then a switch statement with enums might look like:
//		switch (enum) {
//		CASE_ENUM(ENUM1);
//			processCaseEnum1();
//			break;
//		CASE_ENUM(ENUM2);
//			processCaseEnum2();
//			break;
//
//		//etc...
//
//		}

#endif //LOGGING_MESSAGE_TARGET_CONFIGURATION_H_