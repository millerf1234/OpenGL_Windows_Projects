#include "FilepathHelperFunctions.h"

//See https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c
/*
//Method 1      (this is supposedly OS portable code...)    (from stack exchange link posted above)
#ifdef _WIN32
#include <io.h> 
#define access    _access_s
#else
#include <unistd.h>
#endif

bool file_exists(const std::string& fn) {
	return access(fn.c_str(), 0) == 0;
}

#ifdef _WIN32
#undef access
#endif 
*/

//Method 2   (currently not part of the C++ standard but should be added with C++20)   
//          (borrowed from stack exchange link posted above)  
#include <experimental/filesystem> 
bool file_exists(const std::string& fn) {
	//As of C++14:
	return std::experimental::filesystem::exists(fn);
}