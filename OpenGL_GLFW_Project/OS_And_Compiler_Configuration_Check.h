//The following setup functions are called as part of loading the Application.
//These are usually inconsequential and just involve checking macros and 
//configuring stuff...

#pragma once

#ifndef OS_AND_COMPILER___CONFIGURATION_CHECK_H_
#define OS_AND_COMPILER___CONFIGURATION_CHECK_H_

#include "LoggingMessageTargets.h"

//There are 
namespace OperatingSystemAndCompilerDependentConfiguationFunc {

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

} //namespace OperatingSystemAndCompilerDependentConfiguationFunc



#endif //OS_AND_COMPILER___CONFIGURATION_CHECK_H_