// This is just a static global event-logging object for the ShaderInterface.
// Events are logged by calling a pointer-to-static-function they are given
// which will add a message to the message log.
// 
// DOES NOT WORK CORRECTLY AND IS UNFINISHED!
//
//
//Created by Forrest Miller on July 30, 2018

#pragma once

#ifndef SHADER_INTERFACE_LOGGER_H_
#define SHADER_INTERFACE_LOGGER_H_

#include <vector>
#include <iostream> //see:http://www.cplusplus.com/reference/ios/ios/rdbuf/   for redirecting buffers
#include <fstream>  
#include <numeric> //Not sure if I need this?
#include <chrono>

//#include <thread> //eventually...
//#include <future>

#include "ProjectConstants.h"
#include "ProjectParameters.h"
#include "ShaderInterfaceStatusCode.h"



////enum ShaderInterfaceErrorCodes {
//enum ShaderInterfaceStatusCode {
//	SI_NO_ERROR,                         //No error
//	SI_BAD_FILEPATH,					 //
//	SI_BAD_COPY,                         //For when an illegal copy operation is attempted/performed
//	SI_GL_ERROR,						 //For an error within the graphics context 
//	SI_TYPE_MISMATCH,                    //For an illegal operation on mismatched-derived types 
//	SI_LOCAL_DATA_AND_GLCONTEXT_TYPE_MISMATCH,
//	SI_UNCOMPILED,
//	SI_DECOMISSIONED,
//	SI_UNINITIALIZED,
//	SI_MARKED_FOR_DESTRUCTION,
//	SI_POTENTIAL_OBJECT_SLICING_DETECTED,
//
//};


namespace ShaderInterface {
	//namespace ShaderInterfaceLogger {
	namespace UNFINISHED_UNUSED {

		typedef std::chrono::time_point<std::chrono::system_clock>  SystemTime;
		typedef std::chrono::time_point<std::chrono::high_resolution_clock> HighResolutionTime;



		static std::unique_ptr<SystemTime> programSystemStartTime = nullptr;
		static std::unique_ptr<HighResolutionTime> highResProgramStartTime = nullptr;
		static bool useHighResTime = false;
		static bool hasBeenInitialized = false;

		static void initializeShaderInterfaceLogger(bool useHighResClock = false) {
			useHighResTime = useHighResClock;
			programSystemStartTime = std::make_unique<SystemTime>();
			*programSystemStartTime = std::chrono::system_clock::now();
			highResProgramStartTime = std::make_unique<HighResolutionTime>();
			*highResProgramStartTime = std::chrono::high_resolution_clock::now();

		}

		inline void getSystemClockTime(SystemTime& now) {
			now = std::chrono::system_clock::now();
		}
		inline void getHighResClockTime(HighResolutionTime& now) {
			now = std::chrono::high_resolution_clock::now();
		}


		static void logMsgEvent(const char * customMessage) {
			if (useHighResTime) {
				HighResolutionTime nowHR;
				getHighResClockTime(nowHR);
				fprintf(MSGLOG, "At %d: %s\n", (nowHR - (*highResProgramStartTime)).count(), customMessage);
			}
			else {
				SystemTime nowSys;
				getSystemClockTime(nowSys);
				fprintf(MSGLOG, "At %d: %s\n", (nowSys - (*programSystemStartTime)).count(), customMessage);
			}
		}

		static void logMsgEvent(unsigned int id, ShaderInterfaceStatusCode code) {

		}

		static void logWrnEvent(const char * customMessage) {
			if (useHighResTime) {
				HighResolutionTime nowHR;
				getHighResClockTime(nowHR);
				fprintf(WRNLOG, "At %d: %s\n", (nowHR - (*highResProgramStartTime)).count(), customMessage);
			}
			else {
				SystemTime nowSys;
				getSystemClockTime(nowSys);
				fprintf(WRNLOG, "At %d: %s\n", (nowSys - (*programSystemStartTime)).count(), customMessage);
			}
		}

		static void logWrnEvent(unsigned int id, ShaderInterfaceStatusCode code) {

		}

		static void logErrEvent(const char * customMessage) {
			if (useHighResTime) {
				HighResolutionTime nowHR;
				getHighResClockTime(nowHR);
				fprintf(ERRLOG, "At %d: %s\n", (nowHR - (*highResProgramStartTime)).count(), customMessage);
			}
			else {
				SystemTime nowSys;
				getSystemClockTime(nowSys);
				fprintf(ERRLOG, "At %d: %s\n", (nowSys - (*programSystemStartTime)).count(), customMessage);
			}
		}

		static void logErrEvent(unsigned int id, ShaderInterfaceStatusCode code) {


		}





	} //namespace UNFINISHED_UNUSED
	//} //namespace ShaderInterfaceLogger
} //namespace ShaderInterface


#endif //SHADER_INTERFACE_LOGGER_H_