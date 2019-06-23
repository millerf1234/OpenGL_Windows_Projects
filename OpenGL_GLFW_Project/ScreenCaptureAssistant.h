
// todo
//   documentation

//Some notes on how this class is being implemented:
//     
//     -) Despite this class being dependent upon the OpenGL context which owns the 'back buffer' (i.e. the
//        buffer currently being presented to the OS for display), without knowledge of which would prevent 
//        this class from being able to perform any of its primary operations, there are no parameters
//        required to construct a ScreenCaptureAssistant instance. This is made possible because GLFW provides
//        a way to directly retrieve the currently active GL context for any thread. Thus this class's constructor
//        has been designed to only be constructed from the same thread that has active the GL Context that is
//        doing the final rendering and presenting of each frame.
//           Things this means to be aware of:
//
//                      [i]        CALLING THE CONSTRUCTOR FROM A THREAD THAT DOES NOT HAVE AN ACTIVE
//                                 CONTEXT WILL CAUSE THE CONSTRUCTOR TO THROW AN EXCEPTION!
//        
//                      [ii]       CALLING THE CONSTRUCTOR FROM A THREAD THAT HAS A CURRENTLY ACTIVE 
//                                 CONTEXT OTHER THAN THE ONE THAT IS PRESENTING EACH FRAME TO OS WILL
//                                 NOT CAUSE AN EXCEPTION TO BE THROWN BUT MAY RESULT IN INCONSISTANT 
//                                 AND/OR ERATIC BEHAVIOR BY THIS CLASS'S MEMBER FUNCTIONS. CHANCES ARE
//                                 THAT THE WORST THAT WILL HAPPEN IS EVERY ATTEMPTED SCREEN CAPTURE 
//                                 OPERATION WILL FAIL IMMEDIATELY
//                               
//
//
//
//
//
//       References on std::async and std::future:
//                      Scott Meyers on being aware that a std::future will block until it's task
//                      is completed if it's destructor was called. He also says Microsoft breaks
//                      with the standard here and doesn't enforce this requirement, but it may still
//                      show up on other platforms...
//                  http://scottmeyers.blogspot.com/2013/03/stdfutures-from-stdasync-arent-special.html

#pragma once

#ifndef SCREEN_CAPTURE_ASSISTANT_H_
#define SCREEN_CAPTURE_ASSISTANT_H_

#include <iostream>
#include <fstream>
#include <future>

#include "GlobalIncludes.h"
#include "TGA_Image_File_Format_Header.h"


//Struct that serves as the return value for the ScreenCaptureAssistant's high level "Capture-and-
//Write-to-File-All-In-One-Function" functions. This struct consists of the two members, a boolean
//and a std::string. 
//    Member Fields:
//            bool    success   
//      Represents whether the attempt at taking the screenshot was successful. A value of true
//      represents a successful completion of the entire screen capture process (including
//      successfully completing the write into a file). A value of false means that a failure
//      occurred somewhere along the screen capture process.                           
//  
//           std::string    msg   
//      The context of this member is dependent upon the outcome of the screen capture. If
//      the screen capture was completed successfully, then this member will contain the name 
//      of the file into which data was written (note this is simply the file name and not a
//      full filepath). Otherwise, if a failure occurred at any point in the process, this 
//      member will contain a message explaining where the error occurred. Thus if a request
//      to take a screenshot fails, it can be reported to the user why through this message.
typedef struct TakeScreenshotOutcome {
    bool success;
    std::string msg;
} ScreenshotOutcome;



typedef struct ScreenCaptureStorageRequirements {
    int fbWidth, fbHeight; //Frame Buffer Width and Height
    int bytesOfStorageRequired;
} ScreenCaptureRequirements;



typedef struct Data_For_TGA_Header {
    ScreenCaptureRequirements imageRequirements;
    std::vector<unsigned char> capturedData;
} DataForTGA;   



//Chances are pretty good that TGA will be the only supported file
//format for the foreseeable future.
enum class IMAGE_FILE_FORMAT { TGA, JPEG, PNG, TIFF };



typedef void(*ProcessScreenshotResultCallback)(ScreenshotOutcome);



class ScreenCaptureAssistant {
public:
    /*                        //===============================================\\                        *\
                              ||     Constructor/Destructor/Copy/Move/etc.     ||
    \*                        \\===============================================//                        */

    //Will throw an exception if called from a thread that does not contain an 
    //active OpenGL context (as determined by GLFW)
    ScreenCaptureAssistant();

    //Destructor is responsible for making sure all screenshot tasks are complete
    ~ScreenCaptureAssistant() noexcept;


    //Once a ScreenCaptureAssisstant instance has been created, there really should be no reason for it 
    //to ever move somewhere else [and if it turns out there are, then by all means go ahead and
    //implement it yourself].

    ScreenCaptureAssistant(const ScreenCaptureAssistant&) = delete;
    ScreenCaptureAssistant(ScreenCaptureAssistant&&) noexcept = delete;
    ScreenCaptureAssistant& operator=(const ScreenCaptureAssistant&) = delete;
    ScreenCaptureAssistant& operator=(ScreenCaptureAssistant&&) noexcept = delete;





    /*                        //===============================================\\                        *\
                              ||          ScreenCapture Functionality          ||
    \*                        \\===============================================//                        */



    //--------------------------//
    //  High Level Function(s)  //
    //--------------------------//

    //    This function can be called to take screen captures of the currently rendered frame. 
    //Due to the potential for this operation to last up to several seconds when writing data
    //to a file, this function has been broken into 2 distinct phases.
    //     Phases one requires acquiring the image data from the OpenGL context using 
    //'glReadPixels(...)' and thus must be called from the same thread as the context (i.e.
    //(the main thread). However, once the image data has been acquired from OpenGL, there will no
    //longer exists any dependence upon further OpenGL functionality to complete saving the 
    //screenshot, so this function prepares to enter its second phase.
    //     Phase two is somewhat unique for this Application in that it is run asynchronously.
    //To enter phase two, this function simply launches phase 2 asynchronously and then returns
    //a future which will eventually contain the Screenshot's success or failure outcome. It is 
    //up to code calling this function to handle determining when the future is complete and what
    //to do with the result.
    void takeScreenshot(IMAGE_FILE_FORMAT format = IMAGE_FILE_FORMAT::TGA) noexcept;


    //Once a screenshot has completed processing, a ProcessScreenResultCallback function is called
    //by this class's internals. This class provides its own default version of this function that
    //just simply uses 'printf()' to output basic information to the console. This function here
    //though can be used to provide a custom behavior for reporting the outcome of screenshot 
    //operations. 
    void setScreenshotOutcomeCallback(ProcessScreenshotResultCallback psrc) noexcept;


    //Eventually there may be other options here, such as capturing screenshot data from a
    //Framebuffer other than the primary one [i.e. depth/stencil/attachment/etc] 
   
private:
    //When taking a screenshot for the first time within a single lifetime of the process, this value
    //is checked. If this value is 0, then the entire screenshot directory is examined and the highest
    //trailing ID number among screenshots is saved. Then for all subsequent screenshot attempts, this
    //number will not be 0, so to create a new file name for the screenshot all that is needed to be 
    //done is increment this variable by 1 and then append it to the end of the filename. 
    static int greatestScreenshotID;
    const GLFWwindow* mWindowContext_;
    std::list<std::future<ScreenshotOutcome>> activeScreenshotTasks;

    ProcessScreenshotResultCallback screenshotOutcomeReportCallback;

    //-----------------------//
    //  LOW Level Functions  //
    //-----------------------//
    //   All low level Functions have been permanently moved to 'private' to keep the UI 
    //   of this class from going completely insane
   

    //PHASE 1 ACTIONS  --  [Synchronous]
    //These actions will process as required when the 'takeScreenshot()' public member 
    //function gets called. These functions must all complete before control will be 
    //returned to the Application.

    //Expects a valid OpenGL context
    ScreenCaptureRequirements determineRequiredSpaceToHoldData() const noexcept; 

    //Will return the value of the currently bound buffer to binding GL_PIXEL_PACK_BUFFER.
    //A returned value of 0u means that there was no buffer bound on calling this function.
    GLuint makeSureThereIsNoPixelPackBufferBound() const noexcept;
    
    void restoreToPixelPackBufferBinding(const GLuint previouslyBoundBuffer) const noexcept;


    //Make sure that the filesystem is prepared for a new screenshot file
    std::filesystem::path ensureThatADirectoryForScreenshotsExists() const;
    //Note that the file name will not yet contain a file extension
    std::string getNextSequentialFilename() noexcept;
    



    TGA_INTERNAL::TGA_HEADER createTGAHeader(const DataForTGA& dftgah) const;

    

    //PHASE 2 ACTIONS  --  [Asynchronous]
    //These will run asynchronously in the background while the rest of the
    //Application continues to run  

    ScreenshotOutcome reportEmptyFramebuffer() const noexcept;  //OUTCOME -> FAILURE

    ScreenshotOutcome reportExceptionCaught(std::string exceptionMsg) const noexcept;  //OUTCOME -> FAILURE

    ScreenshotOutcome writeDataToFileAsTGA(const TGA_INTERNAL::TGA_HEADER& header,   //OUTCOME -> T.B.D.
                                           const DataForTGA& dftgah) const;


    //////////////////////////////////////
    ////////  UTILITY FUNCTIONS  /////////
    //////////////////////////////////////

   
    friend class RenderDemoBase;
    void upkeepFunctionToBeCalledByRenderDemoBase() noexcept;
};




#endif //SCREEN_CAPTURE_ASSISTANT_H_