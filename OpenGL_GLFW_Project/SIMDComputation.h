//Readme: This class is not complete. It currently is just a compilation of notes, weblinks and snippits of code that I have come 
//		  across regarding SSE/SSE2 SIMD usage. 
//
//  This header can probably be replaced/removed once I get around to actually trying out SSE2. 
//
//Created by Forrest Miller on July 24, 2018

#pragma once

#ifndef SIMD_COMPUTATION_H
#define SIMD_COMPUTATION_H

//NOTES ON SSE:

////////////////
/// INCLUDES ///
////////////////
//There is contradictory information on the internet regarding what headers to include. 
//Here is a list of individual headers:          (list retrieved from   https://stackoverflow.com/questions/11228855/header-files-for-x86-simd-intrinsics )
#include <mmintrin.h> //MMX
#include <xmmintrin.h> //SSE
#include <emmintrin.h> //SSE2
#include <pmmintrin.h> //SSE3
#include <tmmintrin.h> //SSSE3
#include <smmintrin.h> //SSE4.1
#include <nmmintrin.h> //SSE4.2
#include <ammintrin.h> //SSE4A
#include <wmmintrin.h> //AES
#include <immintrin.h> //AVX
#include <zmmintrin.h> //AVX512


//It looks like instead of including each one of those headers above, it is also possible to just include:
#if defined __WIN32__  //Defined by Borland C++ //see: https://sourceforge.net/p/predef/wiki/OperatingSystems/    //This compiler will probably never be encountered
#error
#elif defined __GNUC__
#include <x86intrin.h>  //Except this doesn't work with Microsoft's compiler
#elif defined __llvm__
//? //Unrelated, but see: https://stackoverflow.com/questions/2224334/gcc-dump-preprocessor-defines
#elif defined _MSC_VER
#include <intrin.h>
#endif 




////////////////
/// ALIGNED MEMORY ///
////////////////
//See example given in blog http://cottonvibes.blogspot.com/2011/01/dynamically-allocate-aligned-memory.html
//There some other good examples on there as well...




#endif //SIMD_COMPUTATION_H
