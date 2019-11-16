
//See: https://github.com/bombomby/optick/wiki/Optick-API#optick_set_state_changed_callbackcallback_function

#pragma once
#if 0
#ifndef OPTICK_CALLBACK_FUNCTION_H_
#define OPTICK_CALLBACK_FUNCTION_H_

#include "GlobalIncludes.h"

#if USE_OPTICK
bool OnOptickStateChanged(Optick::State::Type state)
{
    if (state == Optick::State::STOP_CAPTURE)
    {
        // Starting to save screenshot
        g_TakingScreenshot = true;
    }

    if (state == Optick::State::DUMP_CAPTURE)
    {
        // Wait for screenshot to be ready
        // Returning false from this function will force Optick to retry again the next frame
        if (g_TakingScreenshot)
            return false;

        // Attach screenshot
        Optick::AttachFile(Optick::File::OPTICK_IMAGE, "Screenshot.bmp", g_ScreenshotRequest.c_str());

        // Attach text file
        const char* textFile = "You could attach custom text files!";
        Optick::AttachFile(Optick::File::OPTICK_TEXT, "Test.txt", (uint8_t*)textFile, (uint32_t)strlen(textFile));

        // Attaching some custom data
        Optick::AttachSummary("Build", __DATE__ " " __TIME__);
    }
    return true;
}
OPTICK_SET_STATE_CHANGED_CALLBACK(OnOptickStateChanged);
#endif




#endif //OPTICK_CALLBACK_FUNCTION_H_

#endif //#if 0