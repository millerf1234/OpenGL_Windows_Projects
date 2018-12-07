//File:                       WindowCallbackBuffer.h
//
//Description:                Definition for the class WindowCallbackBuffer which is 
//                            used to store information from Window Callback functions.
//                            This class does not handle thread safety, instead that should
//                            be managed by other code using this class. 
//
//Programmer:                 Forrest Miller
//Date:                       December 7, 2018

#pragma once

#ifndef WINDOW_CALLBACK_BUFFER_H_
#define WINDOW_CALLBACK_BUFFER_H_

class WindowCallbackBuffer {
public:
	WindowCallbackBuffer() = default;
	~WindowCallbackBuffer();

private:


};


#endif //WINDOW_CALLBACK_BUFFER_H_

