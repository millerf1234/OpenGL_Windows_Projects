// File:                        FSMWindowAttributes.h
// Class:                       FSMEnginePrototype
// QuickInfo:
//       Default Constructor    DISABLED
//       Copy Operations        DISABLED
//       Move Operations        MOSTLY DISABLED          (Is enabled for internal use by the class FSMWindow) 
//
//
//

//  todo: Write a better description when I get time
//
//
//  Basic idea:   Provide a wrapper for the attributes listed 
//                at: https://www.glfw.org/docs/3.3/window_guide.html#window_attribs
//
//
//  Programmer:     Forrest Miller
//  Date:           January 9, 2019

#pragma once

#ifndef FSM_WINDOW_ATTRIBUTES_H_
#define FSM_WINDOW_ATTRIBUTES_H_

#include "GlobalIncludes.h"  //For GLFW library

class FSMWindowAttributes final {
public:

    FSMWindowAttributes(bool focused, bool iconified, bool maximized, bool visible,
        bool resizable, bool decorated, bool floating);

    ~FSMWindowAttributes() = default;


    //////////////////////////////////////////
    /////     Disabled Functionality     /////
    //////////////////////////////////////////
    FSMWindowAttributes() = delete;
    FSMWindowAttributes(const FSMWindowAttributes&) = delete;
    FSMWindowAttributes& operator=(const FSMWindowAttributes&) = delete;


protected:  //The move constructor and operator are callable by the FSMWindow class
    friend class FSMWindow; //Declare FSMWindow as a friend
    FSMWindowAttributes(FSMWindowAttributes&&) noexcept;
    FSMWindowAttributes& operator=(FSMWindowAttributes&&) noexcept;

private:
    //bool window

};



#endif //FSM_WINDOW_ATTRIBUTES_H_

