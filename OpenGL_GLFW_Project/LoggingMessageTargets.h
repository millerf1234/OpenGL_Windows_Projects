//
// File:                        LoggingMessageTargets.h
//
// How Logging Works:               Logging in this project is very rudimentary, with pretty
//                              much everything being handle through the C function 'fprintf()'.
//
//                                  The syntax of 'fprintf()' is very similar to 'printf()'s 
//                              syntax, with the only difference being 'fprintf()' takes an
//                              additional first parameter which represents any open FILE*
//                              that identifies as an output stream.  
//                                      Syntax:
//                                           int fprintf ( FILE * stream, const char * format, ... );
//                             
//                                 [The integer value returned by 'fprintf()' represents the number of 
//                              characters written to the stream, but this value is typically ignored]
//                              
//                                  Every C/C++ program by default has available to it the following
//                              three FILE* streams: 
//                                                     (i)   stdin
//                                                    (ii)   stdout
//                                                   (iii)   stderr
//                              The first stream 'stdin' is not an output stream, so it plays no further
//                              roll in this discussion. 
//                              The second stream 'stdout' is meant as the default output destination for 
//                              an Application. While its implementation is system-dependent, it typically
//                              by defaults targets the text console. However, it is possible on most Operating
//                              Systems for 'stdout' to be redirected at the time of Application invocation.
//                                 (e.g. most shells support syntax such as:    myapplication > example.txt   
//                                    This redirects any text written to 'stdout' to instead be written to
//                                    the file 'example.txt')
//                              The final stream is 'stderr'. It is just like 'stdout', except its default 
//                              buffering behavior is more restricted. 'stderr' is guaranteed to be either
//                              unbuffered or line-buffered, while implementations are allowed to implement
//                              a more involved full-buffering on 'stdout'.
//                              
// The Log Target Macros:       There are currently 3 targets that this Application uses for logging itself. 
//                              These targets have been used throughout as the following 3 macros: 
//                                                   MSGLOG              [Message Log]
//                                                   WRNLOG              [Warning Log]
//                                                   ERRLOG              [ERROR LOG]
//                              Because these three targets are implemented as macros, it is possible to 
//                              easily modify logging behavior for the entire project simply by redefining 
//                              which FILE* they represent. 
// References:                 
//                   //fprintf()
//                                 http://www.cplusplus.com/reference/cstdio/fprintf/
//                   //stdout
//                                 http://www.cplusplus.com/reference/cstdio/stdout/
//                   //stderr
//                                 http://www.cplusplus.com/reference/cstdio/stderr/
//                   //Change a FILE* buffering properties
//                                 http://www.cplusplus.com/reference/cstdio/setvbuf/ 
//
//July and August 2018          Forrest Miller

#pragma once

#ifndef LOGGING_MESSAGE_TARGET_CONFIGURATION_H_
#define LOGGING_MESSAGE_TARGET_CONFIGURATION_H_

#include <cstdio>  //Needed for fprintf

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


#endif //LOGGING_MESSAGE_TARGET_CONFIGURATION_H_