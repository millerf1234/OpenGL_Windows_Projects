
//I am creating so many new classes today that there is no way to document them all...


#pragma once

#ifndef SCREEN_CAPTURE_OPERATION_PARAMETERS_H_
#define SCREEN_CAPTURE_OPERATION_PARAMETERS_H_

//See OpenGL Programming Guide 9e page 365
enum class ReadPixelsDefaultFramebufferTarget {
    //The following 6 are all Default Framebuffer Attachments
    BACK_BUFFER,
    FRONT_BUFFER,
    FRONT_LEFT_BUFFER,
    BACK_LEFT_BUFFER,
    FRONT_RIGHT_BUFFER,
    BACK_RIGHT_BUFFER,
};


////////////////////////////////////////////////////////////////
///////   INTERESTING!!!   On Page 459 of OpenGL SuperBible, it says
///////                   glReadPixels is for default framebuffers only
///////                    while glReadBuffer() is for client framebuffers.
////////////////////////////////////////////////////////////////

enum class ScreenCaptureImplementationStrategy {
    DIRECT_COPY_TO_APPLICATION,
    USE_PIXEL_PACK_BUFFER
};


#endif //SCREEN_CAPTURE_OPREATION_PARAMETERS_H_