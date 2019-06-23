//                                                                                                                                                                    
// File:                        LoggingMessageTargets.h                                                                                                               
//                                                                                                                                                                    
//                                                                                                                                                                    
//                                  _________________________                                                                                                         
//                               __/                         \__                                                                                                      
//                       }~~~~~~|__     How Logging Works     __|~~~~~~{                                                                                              
//                                 \_________________________/                                                                                                        
//                                                                                                                                                                    
// Thinly Built Atop <cstdio>       Logging in this project is very rudimentary, with pretty                                                                          
//                              much everything being accomplished through the C function 'fprintf()'.                                                                
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
//            The Macros        There are currently 3 targets that this Application uses for logging itself.                                                          
//                              These targets have been used throughout the codebase as the following 3 macros:                                                       
//                                                   MSGLOG              [Message Log]                                                                                
//                                                   WRNLOG              [Warning Log]                                                                                
//                                                   ERRLOG              [ERROR LOG]                                                                                  
//                              Because these three targets are implemented as macros, it is possible to                                                              
//                              easily modify logging behavior for the entire project simply by redefining                                                            
//                              which FILE* they represent.                                                                                                           
//                                                                                                                                                                    
//                                                                                                                                                                    
//                         ________________________________________________                                                                                           
//                      __/                                                \__                                                                                        
//              }~~~~~~|__     Hopefully-Not-To-Contrived-Example-Usage     __|~~~~~~{                                                                                
//                        \________________________________________________/                                                                                          
//                                                                                                                                                                    
//                                                                                                                                                                    
//      CONSIDER THE FOLLOWING DUMMY SAMPLE CODE WHICH DEMONSTRATES USAGE OF ALL THREE LOG TARGETS                                                                    
//                                                                                                                                                                    
//                                                                                                                                                                    
//                  //The maximum number of items allowed per batch                                                                                                   
//                  constexpr int MAXIMUM_ALLOWED_PER_BATCH = 25;                                                                                                     
//                                                                                                                                                                    
//                  typedef struct Item {                                                                                                                             
//                                                                                                                                                                    
//                        ...                                                                                                                                         
//                                                                                                                                                                    
//                  } Item;                                                                                                                                           
//                                                                                                                                                                    
//                  ...                                                                                                                                               
//                                                                                                                                                                    
//                  //---------------------------------------------------------------------------------------------------                                             
//                  //    void processBatch(int batchSize, Item* items);                                                                                              
//                  // Description: Function That Processes a Batch of Items. Parameters are:                                                                         
//                  //              int   batchSize   --  The number of items contained in the target batch                                                           
//                  //              Item* items       --  A pointer to the first element of an array of at least batchSize Items                                      
//                  // Preconditions:                                                                                                                                 
//                  //         -) 'items' shall not be nullptr and shall contain at least batchSize members                                                           
//                  //         -) batchSize shall be a non-negative value that does not exceed MAXIMUM_ALLOWED_PER_BATCH                                              
//                  //---------------------------------------------------------------------------------------------------                                             
//                  void processBatch(int batchSize, Item* items) {                                                                                                   
//                                                                                                                                                                    
//                                                                                                                   [  The start of this function presents an ideal  ]
//                      fprintf(MSGLOG, "Beginning to process a batch of %d elements\n", batchSize);      <----------[  opportunity to announce through the MSGLOG    ]
//                                                                                                                   [  a brief update covering which current task    ]
//                                                                                                                   [  this Application is actively processing       ]
//                                                                                                                                                                    
//                      if (batchSize == 0) {                                                                                                                         
//                          fprintf(WRNLOG, "WARNING! The processBatch function was called with a\n"                 [  If this function gets called with valid yet   ]
//                               "batch size of 0! While this is a technically legal\n"                   <----------[  unusual parameters, it would be courteous to  ]
//                               "operation, please ensure that this is indeed the\n"                                [  provide notice regarding the extraordinary    ]
//                               "intended behavior!\n");                                                            [  situation. This is where the WRNLOG is used   ]
//                          return;                                                                                                                                   
//                      }                                                                                                                                             
//                                                                                                                                                                    
//                                                                                                                                                                    
//                      if ( (batchSize < 0) || (items == nullptr) ) {                                                                                                
//                          if (batchSize < 0) {                                                                                                                      
//                               fprintf(ERRLOG, "\n\n\nFATAL ERROR! Requested batchSize of %d is\n"      <----------[  In the event that this function gets invoked  ]
//                                   "illegal!\n\n", batchSize);                                                     [  invoked with illegal input of the type such   ]
//                          }                                                                                        [  that the only viable course of response       ]
//                          if (items == nullptr) {{                                                                 {  action is immediately crashing, we should     ]
//                               fprintf(ERRLOG, "\n\n\nFATAL ERROR! The parameter 'items' which should\n"           [  first report the exact cause of the issue by  ]
//                                   "be an array of items to process is instead nullptr!\n\n");          <----------[  targeting ERRLOG with a detailed description  ]
//                          }                                                                                        [  of the cause of the issue.                    ]
//                          std::exit(EXIT_FAILURE);                                                                                                                  
//                      }                                                                                                                                             
//                                                                                                                                                                    
//                      if (batchSize > MAXIMUM_ALLOWED_PER_BATCH) {                                                                                                  
//                                                                                                                                                                    
//                          fprintf( WRNLOG, "WARNING! The requested batch-size of %d items exceeds\n"               [  If this function were to encounter invalid    ]
//                               "the maximum limit of %d items-per-batch outlined in this\n"                        [  input that fails to conform to one or more of ]
//                               "function's pre-conditions! Luckily this is not a fatal since\n"        <-----------[  its pre-conditions, yet this violation does   ]
//                               "it can simply be resolved by recursively calling this\n"                           [  not necessitate a FATAL response, we are able ]
//                               "function from within. Please follow the documented pre-conditions\n"               [  to properly accommodate the violation by      ]
//                               "more closely in the future!\n", batchSize, MAXIMUM_ALLOWED_PER_BATCH );            [  first reporting the situation through WRNLOG  ]
//                                                                                                                   [  before resolving the issue and continuing     ]
//                          int itemsRemaining = batchSize;                                                                                                           
//                          do {                                                                                                                                      
//                              processBatch(MAXIMUM_ALLOWED_PER_BATCH, &(items[batchSize - itemsRemaining]);                                                         
//                              itemsRemaining -= MAXIMUM_ALLOWED_PER_BATCH;                                                                                          
//                          } while (itemsRemaining > MAXIMUM_ALLOWED_PER_BATCH);                                                                                     
//                                                                                                                                                                    
//                          //Process Batch                                                                                                                           
//                                                                                                                                                                    
//                          ...                                                                                                                                       
//                                                                                                                                                                    
//                      }                                                                                                                                             
//                                                                                                                                                                    
//                                                                                                                                                                    
//                                                                                                                                                                    
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