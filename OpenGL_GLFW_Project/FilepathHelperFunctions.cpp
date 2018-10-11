
#include "FilepathHelperFunctions.h"


bool file_exists(const std::string& fn) {
	//As of C++14:
	return std::experimental::filesystem::exists(fn);
}