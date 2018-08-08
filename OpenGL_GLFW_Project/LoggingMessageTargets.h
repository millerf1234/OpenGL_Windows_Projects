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




/////////////////////////////////////
////        Case Enum Logger     ////  
/////////////////////////////////////




/////////////////////////////////////
////    Enum To String (sorta)   ////  
/////////////////////////////////////

//This is based off of an example written by Peter Dalton, Evans & Sutherland as section 1.2
//in the book Game Programming Gems 2 (2001). Their example can be found on page 19. 
//
//

// Doesn't work correctly. I think I misjugded what would be possible.
//
////////////////////////////////
//////    Enum To String    ////  
////////////////////////////////
////see top answer at https://stackoverflow.com/questions/9907160/how-to-convert-enum-names-to-string-in-c
//#define EXTRACT_ENUM__(x) #x
//
//#ifndef ENUM_TO_STRING
//#define ENUM_TO_STRING(a) EXTRACT_ENUM__(a)
//#endif 




#endif //LOGGING_MESSAGE_TARGET_CONFIGURATION_H_