#pragma once

#ifndef PRIMARY_WINDOW_STATE_H_
#define PRIMARY_WINDOW_STATE_H_

#include <array>
#include <mutex>
#include <optional>

#include "PrimaryWindowCallbackFunctions.h"
#include "WindowCurserState.h"

struct CurserPositionState {
	bool curserOnScreen = false;
	std::array<double, 2> scrollDelta = { 0.0, 0.0 };
	std::array<double, 2> curserPosition = { 0.0, 0.0 }; //Most recently reported curser position
	std::array<double, 2> previousPosition = { 0.0, 0.0 }; //Position of curser prior to most recent report
	std::array<double, 2> curserStartPosition = { 0.0, 0.0 }; //Curser's initial position 
};


class PrimaryWindowState final{
public:
	PrimaryWindowState();
	~PrimaryWindowState();



protected:
	//Declare the callback functions as friends of this class
	friend void windowPositionCallback(GLFWwindow* window, int xPos, int yPos);
	friend void windowRefreshCallback(GLFWwindow* window, int xPos, int yPos);
	friend void windowSizeCallback(GLFWwindow*, int, int);
	friend void framebufferSizeCallback(GLFWwindow*, int, int);
	friend void mouseButtonCallback(GLFWwindow*, int, int, int);
	friend void mouseScrollCallback(GLFWwindow*, double, double);
	friend void curserEnterCallback(GLFWwindow*, int);
	friend void curserPositionCallback(GLFWwindow*, double, double);
	friend void filedropCallback(GLFWwindow*, int, const char **);

private:
	//std::mutex mCallbackWriteLock_; //Not sure this type of variable is what I want...
	float mWindowWidth_, mWindowHeight_; //Screen Parameters
	float mFramebufferWidth_, mFramebufferHeight_; //Framebuffer Parameters
	CurserPositionState mCurserPosition_;
	std::optional<WindowCurserState> mCurserParameters_;
};

#endif //PRIMARY_WINDOW_STATE_H_