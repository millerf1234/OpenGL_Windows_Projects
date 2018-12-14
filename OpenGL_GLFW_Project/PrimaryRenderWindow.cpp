//File:                     PrimaryRenderWindow.cpp
//
//       See header file for further details
//
//  Programmer:              Forrest Miller
//  Date:                    December 13, 2018

#include "PrimaryRenderWindow.h"

//Helper function to be called at the start of construction for this object to assign
//all member fields initial values.


void PrimaryRenderWindow::initialize() {

}

//Construct a windowed render window on the specifed monitor. Monitor numbering begins at 0. Requesting a number
//greater than the number of connected monitors will cause the highest-available monitor to be selected.
//When running in windowed mode, vsync will automatically match the enabled/disabled behavior of that display.
//The position on the screen and window size will be automatically set, it is possible to move and/or resize the
//window once it has been created.
PrimaryRenderWindow::PrimaryRenderWindow(unsigned int monitorToUse) {
	initialize();


}

//Construct a windowed render window on the specifed monitor. Monitor numbering begins at 0. Requesting a number
//greater than the number of connected monitors will cause the highest-available monitor to be selected.
//When running in windowed mode, vsync will automatically match the enabled/disabled behavior of that display.
//The position on the screen of the window (measured in pixels from the top left corner of the display to
//the top left corner of the window) can be specified, as can the size (span) of the window.
PrimaryRenderWindow::PrimaryRenderWindow(unsigned int monitorToUse, std::array<int, 2> screenPosition, std::array<int, 2> screenSpan) {
	initialize();
}

//Construct a fullscreen render window on the specified monitor. Monitors are indexed from 0. Selecting 
//a monitor number higher than the number of detected available monitors will cause the window to open on
//the monitor with the next closest (highest) index. The screen resolution will be automatically set to match
//the highest detected available resolution of the display.
PrimaryRenderWindow::PrimaryRenderWindow(unsigned int fullScreenMonitorToUse, bool enableVSync, int msaaSamples) {
	initialize();
}

//Construct a fullscreen render window on the specified monitor. Monitors are indexed from 0. Selecting 
//a monitor number higher than the number of detected available monitors will cause the window to open on
//the monitor with the next closest (highest) index. The screen will attempt to be loaded at the specified
//resolution; however, in the event that the resolution exceeds the maximum resolution supported by the display,
//the highest resolution supported by the display will be used. 
PrimaryRenderWindow::PrimaryRenderWindow(unsigned int fullScreenMonitorToUse, bool enableVSync, int msaaSamples, std::array<int, 2> preferedResolution) {
	initialize();

}

//Construct a fullscreen render window on the specified monitor. Monitors are indexed from 0. Selecting 
//a monitor number higher than the number of detected available monitors will cause the window to open on
//the monitor with the next closest (highest) index. The screen will attempt to be loaded at the specified
//resolution; however, in the event that the resolution exceeds the maximum resolution supported by the display,
//the highest resolution supported by the display will be used. The refresh rate of the monitor can be set using
//the refreshRate parameter.
PrimaryRenderWindow::PrimaryRenderWindow(unsigned int fullScreenMonitorToUse, bool enableVSync, int msaaSamples, std::array<int, 2> preferedResolution, int refreshRate) {
	initialize();

}




PrimaryRenderWindow::~PrimaryRenderWindow() {
	//Call GLFW clean-up functions for window?
}
