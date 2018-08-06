//This file contains headers that are meant to be included globally only once. 

//Created by Forrest Miller on July 16, 2018

#pragma once

#ifndef PROJECT_SETUP_H_
#define PROJECT_SETUP_H_

//Glad version link: http://glad.dav1d.de/#profile=core&specification=gl&api=gl%3D4.5&api=gles1%3Dnone&api=gles2%3Dnone&api=glsc2%3Dnone&language=c&loader=on
#include "glad.h"


#include "glfw_config.h"  //Not sure if this is explicitly necessary
#include "glfw3.h"


//Use the gl math library 
#include "glm/glm/glm.hpp"  //Well... It works so I am not going to fix it  //see: https://stackoverflow.com/questions/17905794/how-to-setup-the-glm-library-in-visual-studio-2012



//FreeType is required for HarfBuzz
#include "ft2build.h"

//HarfBuzz text library  //see: https://harfbuzz.github.io/
#include "HarfBuzz/hb.h"
#include "HarfBuzz/hb-ft.h"  //Font


///For logging 
#ifndef MSGLOG
#define MSGLOG stdout
#endif

#ifndef ERRLOG
#define ERRLOG stderr
#endif 

#ifndef WRNLOG
#define WRNLOG stderr
#endif 


#ifndef PROJECT_SETUP_FUNCTIONS_NAMESPACE_HEADER_GUARD_
#define PROJECT_SETUP_FUNCTIONS_NAMESPACE_HEADER_GUARD_
//There are the following setup functions that are called as part of loading the
//app. These are usually inconsuquential and just involve checking macros and 
//configuring stuff...
namespace PROJECT_SETUP_FUNCTIONS {


	namespace {

		//Function Prototypes
		//static void doFeatureTestingForCPP17(); //test for c++17 feature support
		//static void checkForEnabledRunTimeTypeIdentification(); //See if RTTI is available
		//static void checkForEnabledExceptionHandling(); //See if exception handling is available


		////This is the function that should be called publically
		//void Call_Project_Setup_Functions() {
		//	doFeatureTestingForCPP17();
		//	checkForEnabledRunTimeTypeIdentification();
		//	checkForEnabledExceptionHandling();
		//}


		void doFeatureTestingForCPP17() {
			//                              FEATURE TESTING
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//C++17 stuff:         from: https://msdn.microsoft.com/en-us/library/b0084kay.aspx  (for the MSVC compiler macros)
			//                        and http://clang.llvm.org/docs/LanguageExtensions.html#has-cpp-attribute  (for the CLANG macros)
			//(the one you want) ---> and https://infektor.net/posts/2017-01-19-using-cpp17-attributes-today.html   (for GCC/CLANG macros)
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				//If Compiled using Microsoft Visual Studio compiler
		#if defined _MSVC_LANG   
			#if (_MSVC_LANG  == 201703)  //If C++17 (Recommended compilation standard)

				//Enable C++17 features here?

			#elif _MSVC_LANG == 201402  //If C++14 (minimum compilation standard) (untested)
				#define [[fallthrough]]
				#define [[nodiscard]]
				#define [[maybe_unused]]
			#elif _MSVC_LANG < 201402 //C++ version is less than 14
				#error   //Must be compiled under at least c++14 
			#endif 

		#else //Compiler is not MSVC
			/* These cases haven't been tested or implemented yet... see weblinks
			#ifndef __has_cpp_attribute
				#define __has_cpp_attribute(x) 0   //Provides compatibilty with un-macro-tested compilers
			#endif

			#if __has_cpp_attribute(maybe_unused)     //This macro is supposed to check to see if [[maybe_unused]] is supported by compiler.

			#endif //__has_cpp_attribute(maybe_unused)
			#if __has_cpp_attribute(fallthrough)
				#define FALLTHROUGH [[fallthrough]] //gcc
			#elif __has_cpp_attribute(clang::fallthrough)
				#define FALLTHROUGH [[clang::fallthrough]]  //clang
			#else
				#define FALLTHROUGH
			#endif
			*/
		#endif //defined _MSVC_LANG
		//////////////////////////////////////////////////////////////////////
		//   End of the C++17 Feature testing macros
		//////////////////////////////////////////////////////////////////////
		}


		///////////
		// RTTI and Exception handling checks were based off 
		//   https://isocpp.org/std/standing-documents/sd-6-sg10-feature-test-recommendations#recs.cpp98
		///////////
		void checkForEnabledRunTimeTypeIdentification() {
			//Check to see if Run-Time-Type Identification is unsupported!
		#ifndef __cpp_rtti   
			fprintf(WRNLOG, "\nWaRNiNg! DETECTED THAT RUN-TIME-TYPE-IDENTIFICATION is not enabled!\nProgram probably will act strangly and crash a lot!\n");
			//unsupportedCompilerDetected = true;
		#else
			fprintf(MSGLOG, "Detected that compiler supports Run Time Type Identification\n");
		#endif //__cpp_rtti
		}

		void checkForEnabledExceptionHandling() {
			//Check to see if exceptions
		#ifndef __cpp_exceptions
			fprintf(WRNLOG, "\nWarning! DETECTED THat EXCEPTION HANDLING (OR SOMETHING LIKE THAT) IS NOT ENABLED!\n"
				"This may cause a lot of crashing for if an exception gets thrown!\n");
			//unsupportedCompilerDetected = true;
		#else
			fprintf(MSGLOG, "Detected that compiler supports Exception Handling!\n");
		#endif //__cpp_exceptions
		}




		//This is the function that should be called publically
		void Call_Project_Setup_Functions() {
			doFeatureTestingForCPP17();
			checkForEnabledRunTimeTypeIdentification();
			checkForEnabledExceptionHandling();
		}

	} //anonymous namespace

} //namespace PROJECT_SETUP_FUNCTIONS


#endif //PROJECT_SETUP_FUNCTIONS_NAMESPACE_HEADER_GUARD_  

#endif //PROJECT_SETUP_H_



