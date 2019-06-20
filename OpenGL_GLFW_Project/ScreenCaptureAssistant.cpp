
//#include <memory>
#include <vector>
#include <filesystem>
#include <string>
#include "ScreenCaptureAssistant.h"
#include "RelativeFilepathsToResources.h"




//using namespace TGA_INTERNAL;



ScreenCaptureAssistant::ScreenCaptureAssistant() : mWindowContext_(glfwGetCurrentContext()) {
    
    //All there is to do here in the constructor is to call glfwGetCurrentContext and check
    //the returned handle to see if the thread invoking this constructor has an active OpenGL
    //context. Without an active context, rendering is not possible and thus this class would
    //not make any sense.

    if (!mWindowContext_)
        throw (std::exception("Exception Was Thrown While Attempting To Construct The\n"
            "ScreenCaptureAssistant object. It appears as though the thread\n"
            "this constructor was called upon does not have a current OpenGL\n"
            "context. Seeing as an OpenGL context is needed to do any rendering\n"
            "and the sole purpose of this class is to record rendered output, it\n"
            "is kinda sorta a major major issue that there is no context!\n"));

}

ScreenCaptureAssistant::~ScreenCaptureAssistant() noexcept {

}


std::future<ScreenshotOutcome> ScreenCaptureAssistant::takeScreenshot(IMAGE_FILE_FORMAT format) const noexcept {
  
    if (format != IMAGE_FILE_FORMAT::TGA) {
        fprintf(MSGLOG, "\n                         IMPORTANT!\n"
            "The requested file format for this operation is currently unsupported!\n"
            "Instead of whatever format it was you requested, your screenshot data will be\n"
            "stored using the '.tga' format since luckily for the developer its super easy\n"
            "to implement and thus is all that is supported currently!\n");
           
        format = IMAGE_FILE_FORMAT::TGA;
    }

    try {
        const auto spaceRequired = determineRequiredSpaceToHoldData();
        if (spaceRequired.bytesOfStorageRequired == 0) {
            //auto fut = forceBeginAsyncTask(&(ScreenCaptureAssistant::reportEmptyFramebuffer));
            auto future = std::async(std::launch::async, &(ScreenCaptureAssistant::reportEmptyFramebuffer), *this);
        }
        std::vector<unsigned char> screenCaptureData;
        screenCaptureData.resize(spaceRequired.bytesOfStorageRequired);

        //Here we go, let's read the data
        glReadPixels(0, 0, spaceRequired.fbWidth, spaceRequired.fbHeight, GL_BGR, GL_UNSIGNED_BYTE, screenCaptureData.data());

        //Create the header for the '.tga' file
        DataForTGAHeader dataForHeader;
        dataForHeader.imageRequirements = spaceRequired;
        dataForHeader.capturedData.swap(screenCaptureData);

        auto header = createTGAHeader(dataForHeader);

        //At this point phase 1 is nearly complete. Thus we can now
        //create and beginning to prepare the object that will be 
        //returned at the end of this function. It turns out that we 
        //can do this all in one step.
        return writeToFile(header, dataForHeader);
    }
    catch (const std::exception& e) {
        ScreenshotOutcome outcome;
        outcome.success = false;
        outcome.msg = "An Exception was thrown during the screen capture process!\nMessage:\n\t";
        outcome.msg += e.what();
        return outcome;
    }
}


//std::future<ScreenshotOutcome> ScreenCaptureAssistant::takeScreenshotAndSaveItAsTGA() const noexcept {




//}


/////////////////////////////////////////////////////////
/////          Lower Level Functions          ///////////
/////////////////////////////////////////////////////////

ScreenCaptureRequirements ScreenCaptureAssistant::determineRequiredSpaceToHoldData() const {
    
    ScreenCaptureRequirements screenshotRequirements;
    //Trust me, I know what I am doing and this const_cast will be fine
    glfwGetFramebufferSize(const_cast<GLFWwindow*>(mWindowContext_),
                           &screenshotRequirements.fbWidth,
                           &screenshotRequirements.fbHeight);
    
    int rowSize = ((screenshotRequirements.fbWidth * 3 + 3) & ~3);  //The final '& ~3' ensures there will always be alignment
    //                                       //by setting the smallest two bits always to 0
    screenshotRequirements.bytesOfStorageRequired = rowSize * screenshotRequirements.fbHeight;
    return screenshotRequirements;
}


TGA_INTERNAL::TGA_HEADER ScreenCaptureAssistant::createTGAHeader(const DataForTGAHeader& dftgah) const {
    TGA_INTERNAL::TGA_HEADER header;
    memset(&header, 0, sizeof(header));
    
    //Set all the constant values in the header
    header.imagetype = 2;
    header.bpp = 24;
    
    //Set the screen capture variable values in the header (they vary as the
    //framebuffer itself varies)
    header.width = dftgah.imageRequirements.fbWidth;
    header.height = dftgah.imageRequirements.fbHeight;

    return header;
}


ScreenshotOutcome ScreenCaptureAssistant::reportEmptyFramebuffer() const noexcept {
    

}


ScreenshotOutcome ScreenCaptureAssistant::writeToFile(const TGA_INTERNAL::TGA_HEADER& header, const DataForTGAHeader& dftgah) const {
    const std::string baseFileName = "Screenshot_";
    try {
        ScreenshotOutcome outcome;
        outcome.success = false;
        outcome.msg = "No Error Message Specified";

        //Navigate the filesystem to get to the directory where screenshots are stored. If no such
        //directory is found, then create one.
        std::filesystem::path pathToScreenshotsDirectory = FILEPATH_TO_SCREENSHOTS;
        if (!(std::filesystem::is_directory(pathToScreenshotsDirectory))) {
            fprintf(MSGLOG, "\nDetected that currently there does not exist a file system\n"
                "directory for storing screenshots. A directory for screenshots\n"
                "is being created at filepath:\n\t%s\n", pathToScreenshotsDirectory.lexically_normal().string().c_str());
            std::filesystem::create_directory(pathToScreenshotsDirectory);

            std::filesystem::path fileName = pathToScreenshotsDirectory.string() + baseFileName + std::to_string(greatestScreenshotID++);
            printf("\n\nScreenshot File's name will be: %s\n", fileName.string().c_str());
        }
        else {
            //Create a unique filename
            if (greatestScreenshotID == 0)
                (void)greatestScreenshotID;

            else {

            }
        }
        return outcome;
    }
    catch (const std::exception& e) {
        ScreenshotOutcome outcome;
        outcome.success = false;
        outcome.msg = "An Exception was thrown during the file writing stage!\nMessage:\n\t";
        outcome.msg += e.what();
        return outcome;
    }

}



int ScreenCaptureAssistant::greatestScreenshotID = 0;