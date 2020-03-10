// File:                 DebugSettings                                             
//                                                                                 
// Description:                  Define macros which determine the behavior of this
//                             Application's underlying libraries "GLFW" and "glad"
//                                                                                 
//                             The first option is the macro "USE_DEBUG_". Defining
//                            this macro lets GLFW know that the OpenGL context and
//                         render-window it creates should operate in 'debug' mode.
//                           [Behind the scenes: 'glfwWindowHint()' is called using
//                          parameters 'GLFW_OPENGL_DEBUG_CONTEXT' and 'GLFW_TRUE',
//                            [and what happens with glad is actually broken and is
//                                                 eventually going to be removed]]
//                         Defining this may also cause the start-up to be slightly
//                         more verbose. However, leaving this macro undefined will
//                           cause neither 'GLFW's debug_context hint nor 'glad' in
//                          debug configuration to be used for the build. One major
//                               difference between release and debug 'glad' is the
//                                   inclusion of customizable pre- and post-OpenGL
//                                       function-call callbacks in the debug mode.
//                                                                                 
//                                                                                 
//                                                                                 
//        The second option is the macro "FORCE_DEBUG_CONTEXT_APP_SYNCHRONIZATION_"
//                         which affects solely the single [for now] OpenGL context
//                       used on the primary render thread. Typically an OpenGL    
//                      Context is designed to run in a very asynchronous manner   
//                      its client Application. However, this can lead to some     
//                     out-of-order debug/warning/error messages to be reported by 
//                    the OpenGL Context many Application instructions beyond their
//                      source. Thus, to overcome this issue for debugging it is   
//                     simply a matter of defining this macro, which will cause    
//                     every OpenGL function call to block until the OpenGL Context
//                                has completed the operation. Be advised this will 
//                                                            slow everything down.
//                                                                                 
//                                                                                 
// Date:                 January 2019                                              
                                                                                   
#pragma once                                                                       
                                                                                   
#ifndef DEBUG_SETTINGS_H_                                                        
#define DEBUG_SETTINGS_H_                                                        



//This macro is used to enable/disable debugging across the entire project. Turning this
//macro on will cause the OpenGL context to operate in a 'debug' mode. Also, if the 
//macro 'USE_DEBUG_' is defined, then many places within the code will have extra debug
//logic included that will report unexpected or unusual occurrences (e.g. if a switch statement 
//results in the 'default' condition being executed). Leaving 'USE_DEBUG_' undefined (by commenting
//it out) will prevent many unexpected events from being reported but should result in better 
//performance.
#define USE_DEBUG_


//The macro 'FORCE_DEBUG_CONTEXT_APP_SYNCHRONIZATION_' specifically affects the behavior of the
//OpenGL context. Defining this macro will cause the context to be synchronized with the application
//so that any messages/warnings/errors reported by the context will be reported close to the code/operation
//that caused them. Leaving this macro undefined will allow the context to operate asynchronously, which 
//could cause a dramatic performance boost but could also result in context messages to be reported 
//long after the Application event that triggered them. It is recommended to leave this enabled while
//debugging unless absolutely necessary. 
//This macro will only be used if the 'USE_DEBUG_' macro is defined.
#define FORCE_DEBUG_CONTEXT_APP_SYNCHRONIZATION_






#endif //DEBUG_SETTINGS_H_
