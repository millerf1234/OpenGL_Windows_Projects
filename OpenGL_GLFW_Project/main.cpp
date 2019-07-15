//File:                main.cpp
//
//Description:         Create the application and launch it.
//                  
//Updates:             Made much safer by wrapping both the constructor
//                     and 'run()' 
//
//Programmer:          Forrest Miller 
//Date:                July 2018


#include "Application.h"




//Added the following safety features in June 2019. Bring on the Fuzzers!
//  [see FailMalloc: http://www.nongnu.org/failmalloc/ ]
#define SAFETY   try {
#define FIRST } catch (const std::bad_alloc& e) {                                \
fprintf(stderr, "\nWoah! Grab a camera. You have just experienced \n"            \
    "the ultra rare, wildly elusive \'std::bad_alloc\'\n"                        \
    "exception! This is the stuff you only read about\n"                         \
    "these days in textbooks! Perhaps you should restart\n"                      \
    "your computer, but congratualtions on the\n"                                \
    "new story you have for your grandchildren.\n"                               \
    "\n\t[Here is the obligatory exception message]\n\t%s\n\n",                  \
    e.what());                                                                   \
} 






int main(int argc, char * argv[]) {
    SAFETY
	std::unique_ptr<Application> app = std::make_unique<Application>(); 
	app->launch();
    FIRST
}







#ifdef SAFETY
#undef SAFETY
#endif
#ifdef FIRST
#undef FIRST
#endif 