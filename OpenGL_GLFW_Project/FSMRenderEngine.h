// File:                        FSMRenderEngine.h
// Class:                       FSMRenderEngine
// QuickInfo:
//       Default Constructor    ENABLED
//       Copy Operations        DISABLED
//       Move Operations        DISABLED
//
//
//  IMPORTANT:      GLFW functions are not to be called by code outside of this engine. Attempting
//                  to bypass this engine by directly modifying the state of a GLFW object can cause
//                  major issues with the functionality of the FSMRenderEngine
//
//
// Description:     This is it. This is the highest level class in the entire hierarchy of classes 
//                  that make up the FSMRenderEngine (plus some utility/UI features to boot). Everything
//                  an application needs to get up and running can be achieved by interfacing with this 
//                  class.
//
//
// Programmer:       Forrest S Miller
// History:          Code has been under development since July 2018 but this particular class is from January 2019
//

#pragma once

#ifndef FSM_RENDER_ENGINE_H_
#define FSM_RENDER_ENGINE_H_

#include "GlobalIncludes.h"  //For GLFW and glad

#include <string>            


//This may or may not get fully implemented
enum class EngineLogMessageVerbosity { EVERYTHING = 0, MOST = 1, MEDIUM = 2, IMPORTANT_DETAILS_ONLY = 4, NONE = 10 };

class FSMRenderEngine final {
public:
    //                /////////////////////////////////////////////////////////
    //                //////////            Constructor(s)           //////////
    //                /////////////////////////////////////////////////////////

    FSMRenderEngine(EngineLogMessageVerbosity verbosity = EngineLogMessageVerbosity::MOST);


    //                  //////////////////////////////////////////////////////
    //                  //////////            Destructor            //////////
    //                  //////////////////////////////////////////////////////

    ~FSMRenderEngine();







    //             /////////////////////////////////////////////////////////////////
    //             //////////      Static Environment Query Functions      /////////
    //             /////////////////////////////////////////////////////////////////

    //Sets parameters to match the version of OpenGL that contexts created through this class will use. This
    //will match the version of OpenGL that gets linked into the code at compile time as determined by the 3rd 
    //party library 'glad'. To change versions of OpenGL that will be used, simply replace 'glad.h' and 'glad.c'
    //with their respective configurations for that version. [This class was written targeting OpenGL 4.6] 
    static void getGLVersion(int& versionMajor, int& versionMinor, bool& usesCompatMode);
    //Returns the OpenGL version major that contexts created through this object will use.  This value is set 
    //to match the version of OpenGL that gets linked into the code at compile time determined by the 3rd 
    //party library 'glad'.
    static int getGLVersionMajor();
    //Returns the OpenGL version minor that contexts created through this object will use. This value is set 
    //to match the version of OpenGL that gets linked into the code at compile time determined by the 3rd 
    //party library 'glad'.
    static int getGLVersionMinor();
    //Returns a string outlining the version of OpenGL that will be used when creating GL Contexts. These
    //values can only be specified as parameters during object construction. The returned string will 
    //look like (with the sections in-between the single quotes being replaced by their respective
    //integer values): 
    //    "OpenGL 'versionMajor'.'versionMinor' Core"                   
    static std::string getGLVersionString();
    //Retrieves the Major, Minor and Revision numbers corresponding to the version of GLFW used 
    //during compile time. These values can be compared with the shared-library GLFW version loaded
    //at runtime (which could be helpful for debugging).
    static void getGLFWCompileTimeVersion(int& compileVersionMajor, int& compileVersionMinor, int& compileVersionRevision);
    //Returns a string that looks like: "Compiled against GLFW 'major'.'minor'.'revision'", with the
    //'major', 'minor' and 'revision' replaced with their corresponding integer values. 
    static std::string getGLFWCompileTimeVersionString();
    //Retrieves the Major, Minor and Revision numbers corresponding to the version of GLFW linked with 
    //during run time. These values can be compared with the compile-time GLFW version (which could be 
    //helpful for debugging purposes).
    static void getGLFWRuntimeVersion(int& runtimeVersionMajor, int& runtimeVersionMinor, int& runtimeVersionRevision);
    //Returns a string that looks like: "Running against GLFW 'major'.'minor'.'revision'", with the
    //'major', 'minor' and 'revision' replaced with the corresponding integer values retrieved from
    //the GLFW library which gets linked at run-time. 
    static std::string getGLFWRuntimeVersionString();

    

    //                  //////////////////////////////////////////////////////
    //                  //////////      Public Member Functions      /////////
    //                  //////////////////////////////////////////////////////


    //Checks to see if this object is ready for use
    //inline bool ready() const { return mGLFWWasInitialized_; }
    //Allows for the object to be treated as a bool in certain expressions. Will take on the value
    //of 'true' if object is ready and will take on the value 'false' if not ready
    //explicit operator bool() const { return ready(); }



    //                 //////////////////////////////////////////////////////
    //                 //////////    Copying/Moving are disabled    /////////
    //                 //////////////////////////////////////////////////////
    FSMRenderEngine(const FSMRenderEngine&) = delete;
    FSMRenderEngine(FSMRenderEngine &&) = delete;
    FSMRenderEngine& operator=(const FSMRenderEngine&) = delete;
    FSMRenderEngine& operator=(FSMRenderEngine&&) = delete;


private:
};




#endif //FSM_RENDER_ENGINE_H_