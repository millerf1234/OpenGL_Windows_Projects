//File:                       Application.h
//Class:                      Application
//
//Description (out-of-date):
//                           The class 'Application' exists at the very top of the program and is responsible
//                            for first performing high-level initialization and then providing an environment for 
//                            individual 'RenderDemos' to operate in. More specifically, this class operates in
//                            two phases. The first phase occurs when the constructor for 'Application' is called. In
//                            this phase, the following tasks are performed (in order):
//                                [Initialization Phase]
//                                  - initialize a window and graphics context through the GLFW 3.x library
//                                  - load the graphics language functions using GLAD 
//                                  - configure graphics context by setting callbacks and global state
//                            
//                            Once these initialization tasks are accomplished, the second phase is commenced by calling
//                            the member function launch(). The way launch() operates is to first verify that all 
//                            initialization has occurred without issue, followed by presenting the program user with a             *(to be implemented still...)
//                            menu selection* of the various 'RenderDemos' available.     
//                            Once a 'RenderDemo' is selected, the application uses the polymorphic behavior of the 
//                            'RemderDemoBase' class and the member function runRenderDemo() to run the various 'RenderDemos'
//                            available.
//
//Programmer:                 Forrest Miller
//Dates:                      July - November 2018   


#pragma once

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <memory>   //smart pointers
#include <fstream>  //Gotta have that fstream
#include <filesystem> //Take advantage of the modern <filesystem>
#include <system_error> //Probably best to be aware of any system exceptions


#include "GlobalConstants.h"
#include "ApplicationConstantSettings.h"
#include "GLFW_Init.h"
#include "RenderDemoBase.h"

//Forward declare the Timepoint class
class Timepoint;

class Application final {
public:
    Application();
    ~Application() noexcept;
    void launch();

    //No Copying or Moving allowed
    Application(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(const Application&) = delete;
    Application& operator=(Application&&) = delete;

private:
    bool                        mApplicationValid;
    std::unique_ptr<InitReport> initReport;
    std::unique_ptr<GLFW_Init>  glfwInitializer;

#if (defined USE_OPTICK && (USE_OPTICK == 1))
    //All data relevant to the 'optick' profiler is stored together 
    typedef struct OptickState {
        bool captureActive;
        std::filesystem::path pathToSaveCapture;
    } OptickState;

    OptickState globalOptickState; 
#endif //USE_OPTICK == 1

    void initialize();

    bool setupGLFW(); //Loads the GLFW library
    bool loadGraphicsLanguageFunctions(); //(Well actually load their pointers) (This uses 'glad' to load Graphics Language)
    //Call this function once the Graphics Language Functions have all been loaded to announce to the 
    //console what specifics the platforms OpenGL implementation is operating with.
    void reportDetailsFromGLImplementation();
    void reportDetailsFromGLImplementationOnTextureUnitLimitations();
    void configureGraphicsContextDebugCallbackFunctions() const; //The callback function is provided by Application and is found in its own header file 
    void setInitialGLState();
    void checkMSAA() const; //Prints MSAA config to MSGLOG

    void playIntroMovie() noexcept;



    //Run Theoretical OpenGL Sandbox Experiment Demo
    void runOpenGLSandboxExperiments();


    //Function to run a RenderDemo. The provided unique pointer for the renderDemo should be 
    //newly created, without either of the load() or run() methods having been called. This
    //function is intended to be called by the more specific run_____Demo() functions below.
    void runRenderDemo(std::unique_ptr<RenderDemoBase>& renderDemo, const char* name = R"([UNTITLED])");




    //---------------
    //  Invoke A Render Demo
    //---------------

    void runTeapotExplosionDemo();
    void runLightsourceTestDemo();
    void runAssetLoadingDemo();
    void runFlyingCameraDemo();



    //                        +---------------------------+                    
    //                        |  ERROR HANDLING FUNCTION  |                    
    //                        +---------------------------+                    
    // Description:          Call this function to initiate a controlled crash.
    //
    // WARNING:                    This function should only be called when the
    //                       Application has completed writing all data and has
    //                          done all it can to prepare itself for oblivion.
    //                              Instead of returning, this function's final
    //                           instruction is a call to std::exit() (with the
    //                             status of 'EXIT_FAILURE'). In doing so, this
    //                            function permanently relinquish's all further
    //                               control of the process by entering into an
    //                           internal exit sequence in which every 'atexit'
    //                         callback function is called in the reverse order
    //                              from which each was assigned [i.e. 'atexit'
    //                            callback functions are assigned onto a stack,
    //                            which is then emptied when 'std::exit()' gets
    //                            called]. THIS PREVENTS ANY FURTHER ACTION NOT
    //                                HANDLED BY AN 'atexit' CALLBACK FROM EVER
    //                                    AGAIN HAVING A CHANCE TO BE PERFORMED
    //                                                  (by the current thread)
    [[noreturn]] static void safeCrash() noexcept;

};



#endif //#define APPLICATION_H_