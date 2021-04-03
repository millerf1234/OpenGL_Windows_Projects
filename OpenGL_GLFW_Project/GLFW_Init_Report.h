
//FILE:             GLFW_INIT_REPORT.h
//
//Description:      Defines a collection of structs that
//                  represent both detected state of the 
//                  runtime environment/platform/system plus
//                  optionally configured state which is
//                  set during initialization. 
//



#pragma once

#ifndef GLFW_INIT_REPORT_H_
#define GLFW_INIT_REPORT_H_

//Variant will become part of the global namespace for
//any code which includes this header.
#include <variant> 

//Forward declare the internal GLFW types
struct GLFWmonitor;
struct GLFWwindow;

static constexpr const int NO_MONITOR_ACTIVE = -1;

//Description: Basic informational report covering the system's enumerated monitors at 
//             time of GLFWInitialization. Intended be passed along to each RenderDemo
//             for use as reference.
typedef struct MonitorEnumerationReport {

    typedef struct EnumeratedMonitorHandles {
        int count = 0;
        GLFWmonitor** enumeratedHandles = nullptr;
    } EnumeratedMonitors;

    typedef struct ActiveMonitorReport {
        int activeMonitorArrayIndex = NO_MONITOR_ACTIVE;
        GLFWmonitor* activeMonitor = nullptr;
        int nativeWidth = 0;
        int nativeHeight = 0;
        int nativeRefreshRate = 1;
    } ActiveMonitor;

    EnumeratedMonitors enumeratedMonitors;
    ActiveMonitor activeMonitor;
} MonitorEnumerationReport;


//Description: Basic informational report covering the system's enumerated monitors at 
//             time of GLFWInitialization. Intended be passed along to each RenderDemo
//             for use as reference.
typedef struct WindowContext {

    typedef struct ContextInfo {
        bool contextValid = false;
        int versionMajor = 4;
        int versionMinor = 0;
        bool forwardCompat = false;
        bool isDebug = false;
       // bool forceContextAppSync = false;  //Turns out this is set elsewhere (in Application.cpp)
        bool loseOnReset = false;
    } Context;

    typedef struct WindowInfo {
        GLFWwindow* window = nullptr;
        bool fullscreen = false;
        int framebufferWidth = 0;
        int framebufferHeight = 0;
        int samples = 0; //FOR MSAA

        
        typedef struct FullscreenWindowInfo {
            int refreshRate = 1;
            bool vsync = false;

        } FullscreenInfo;

        typedef struct WindowedModeIndo {
            bool resizeable = false;
            bool decorated = false;

        } WindowedInfo;

        std::variant<FullscreenInfo, WindowedInfo> conditionalInfo;// = WindowedInfo();
    } Window;

    Context context;
    Window window;
} WindowContext;

typedef struct InitializationReport {
    MonitorEnumerationReport monitors;
    WindowContext windowContext;
} InitReport;



#endif //GLFW_INIT_REPORT_H_