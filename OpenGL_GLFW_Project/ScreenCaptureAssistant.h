
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
} DataForTGAHeader;

//Chances are pretty good that TGA will be the only supported file
//format for the foreseeable future.
enum class IMAGE_FILE_FORMAT { TGA, JPEG, PNG, TIFF};


class ScreenCaptureAssistant {
public:
    /*                        //===============================================\\                        *\
                              ||     Constructor/Destructor/Copy/Move/etc.      ||
    \*                        \\===============================================//                        */

    //Will throw an exception if called from a thread that does not contain an active OpenGL context.
    ScreenCaptureAssistant();

    //Must have a way to figure out how best to deal with any files that may still be open and writing.
    ~ScreenCaptureAssistant() noexcept;


    //Once a ScreenCaptureAssisstant instance has been created, there really should be no reason for it 
    //to ever move somewhere else [and if it turns out there are, then by all means go ahead and
    //implement it yourself].

    ScreenCaptureAssistant(const ScreenCaptureAssistant&) = delete;
    ScreenCaptureAssistant(ScreenCaptureAssistant&&) noexcept = delete;
    ScreenCaptureAssistant& operator=(const ScreenCaptureAssistant&) = delete;
    ScreenCaptureAssistant& operator=(ScreenCaptureAssistant&&) noexcept = delete;





    /*                        //===============================================\\                        *\
                              ||          ScreenCapture Functionality           ||
    \*                        \\===============================================//                        */



    //------------------------//
    //  High Level Functions  //
    //------------------------//

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
    [[nodiscard]] std::future<ScreenshotOutcome> takeScreenshot(IMAGE_FILE_FORMAT format = IMAGE_FILE_FORMAT::TGA) const noexcept;

    /*//These were replaced with the above function which changes format based off a parameter. 
    //Operation not supported yet
#ifdef JPEG_WRITE_SUPPORT_AVAILABLE
    std::future<ScreenshotOutcome> takeScreenshotAndSaveItAsJPEG() const noexcept { }
#endif 

    //Operation not supported yet
#ifdef PNG_WRITE_SUPPORT_AVAILABLE
    std::future<ScreenshotOutcome> takeScreenshotAndSaveItAsPNG() const noexcept { }
#endif 
    */



    //-----------------------//
    //  LOW Level Functions  //
    //-----------------------//

    /*Until I have things more in order, only expose the high level functions to the general public.*/
private:  /* (The plan is to enable these lower level functions one by one as this class matures) */
        
    ScreenCaptureRequirements determineRequiredSpaceToHoldData() const;

    TGA_INTERNAL::TGA_HEADER createTGAHeader(const DataForTGAHeader& dftgah) const;

    std::string getNextSequentialFilename() noexcept;



    //Here are possible asynchronous actions that can be used for part 2 based off 
    //how things are proceeding in part 1.

    ScreenshotOutcome reportEmptyFramebuffer() const noexcept ;
    ScreenshotOutcome writeToFile(const TGA_INTERNAL::TGA_HEADER& header, const DataForTGAHeader& dftgah) const;








private:
    //When taking a screenshot for the first time within a single lifetime of the process, this value
    //is checked. If this value is 0, then the entire screenshot directory is examined and the highest
    //trailing ID number among screenshots is saved. Then for all subsequent screenshot attempts, this
    //number will not be 0, so to create a new file name for the screenshot all that is needed to be 
    //done is increment this variable by 1 and then append it to the end of the filename. 
    static int greatestScreenshotID;
    const GLFWwindow* mWindowContext_;

    //Helper function to launch an asynchronous task with the launch policy that ensures the task
    //begins executing right away. The default behavior of std::async allows the task scheduler to
    //optionally delay launching the task until a request is made using 'get' or 'wait' on the future.
    //This default behavior means if the returned future never has 'get' or 'wait' called on it, it
    //could be that it is possible that the task never launches at all. Seeing as this class is perhaps 
    //going to be saving data to files asynchronously, it is very desirable to be able to guarantee that
    //every 'save data' task is carried out without delay. 
    //This example is based directly from Scott Meyer's Effective Modern C++ Item 36
   template<typename F, typename ... Ts>
   inline auto forceBeginAsyncTask(F&& f, Ts&&... parameters) {
       return std::async(std::launch::async, 
                         std::forward<F>(f),
                         std::forward<Ts>(parameters)...);
   }


   

};




#endif //SCREEN_CAPTURE_ASSISTANT_H_