//
//  File:                                ScreenCapture.h
//
//  Description:                        Intermediate storage class meant to hold data while  
//                                      being passed through a screen capture operation. 
//                                      Starts as first an empty object to eventually a
//                                      complete screen capture
//                                      [or at least hopefully a sufficiently detailed  
//                                      error message explaining how the operation failed]  
//                                       
//
//
//
//
//


#pragma once

#ifndef SCREEN_CAPTURE_H_
#define SCREEN_CAPTURE_H_

#include <string>
#include <vector>

#include "RestrictedOperationViolation.h"

struct GLFWwindow;

enum class ScreenCaptureState {
    FreshlyCreatedAndAwaitingFramebufferAnalysis,
    CancelledDueToInvalidFramebufferTarget,
};





typedef struct Timepoint {
    double timepoint;
    std::string tag;
} Timepoint;


class ScreenCapture {
public:
    //Shall be called from the main rendering thread only or will throw an exception
   /* ScreenCapture(const GLFWwindow* context,
                  ReadPixelsSourceTarget t = ReadPixelsSourceTarget::BACK_BUFFER, 
                  ScreenCaptureImplementationStrategy s =
                                 ScreenCaptureImplementationStrategy::DIRECT_COPY_TO_APPLICATION); 
                                 */
    ~ScreenCapture() noexcept;
    ScreenCapture(const ScreenCapture&) = delete;
    ScreenCapture(ScreenCapture&&) noexcept;
    ScreenCapture& operator=(const ScreenCapture&) = delete;
    ScreenCapture& operator=(ScreenCapture&&) noexcept;


    //The struct that exists at the heart of every ScreenCapture object.
    //This is the only member guaranteed to always exist since it is  
    //all that is available to a ScreenCapture object as it is constructed. 
    //
    //This struct consists of three members, a boolean, an enum and a std::string.    
    //    Member Fields:
    //            bool    success   
    //      Represents whether the attempt at taking the screenshot was successful. A value of true
    //      represents a successful completion of the entire screen capture process (including
    //      successfully completing the write into a file). A value of false means that a failure
    //      occurred somewhere along the screen capture process.                           
    //                                                                              
    //          ScreenCaptureState     state
    //      This is a custom enum that provides a more detailed overview of this object's state 
    //      of being. By referring to this variable it should be immediately clear how far along
    //      this object made it through the screen capture algorithm. 
    //
    //           std::string     msg   
    //      The context of this member is dependent upon the outcome of the screen capture. If
    //      the screen capture was completed successfully, then this member will contain the name 
    //      of the file into which data was written (note this is simply the file name and not a
    //      full filepath). Otherwise, if a failure occurred at any point in the process, this 
    //      member will contain a message explaining where the error occurred. Thus if a request
    //      to take a screenshot fails, it can be reported to the user why through this message.
    typedef struct ScreenshotOutcome {
        bool success;
        ScreenCaptureState state;
        std::string msg;
    } ScreenshotOutcome;



    //Returns the status information of this object
    const ScreenshotOutcome& status() const noexcept;

    //Return measured Timepoints to help benchmark the
    //current implementation strategy
    std::vector<Timepoint> timepoints() const noexcept;

     

private:
    const GLFWwindow* mContext_;
    ScreenshotOutcome mStatus_;
    std::vector<Timepoint> mTimepoints_;
    ReadPixelsSourceTarget mTarget_;
    ScreenCaptureImplementationStrategy mStrategy_;


};


#endif //SCREEN_CAPTURE_H_

