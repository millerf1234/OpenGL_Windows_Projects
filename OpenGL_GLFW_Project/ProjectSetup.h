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



#endif //define PROJECT_SETUP_H_



