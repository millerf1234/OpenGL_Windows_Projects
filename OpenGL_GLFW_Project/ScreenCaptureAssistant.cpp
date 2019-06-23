
//#include <memory>
#include <vector>
#include <filesystem>
#include <string>
#include "ScreenCaptureAssistant.h"
#include "RelativeFilepathsToResources.h"
#include "ForceBeginAsyncTask.h"


//Implementation-File-Only Function Prototypes [See definitions at bottom]

void defaultScreenshotResultCallbackFunction(ScreenshotOutcome result);
   

//Implementation-Only struct
typedef struct ScreenCapture {
    ScreenCaptureRequirements screenCaptureInfo;
    std::vector<unsigned char> screenCaptureData;
} ScreenCapture;



ScreenCaptureAssistant::ScreenCaptureAssistant() : mWindowContext_(glfwGetCurrentContext()) {
    
    //All there is to do here in the constructor is to call glfwGetCurrentContext and check
    //the returned handle to see if the thread invoking this constructor has an active OpenGL
    //context. Without an active context, rendering is not possible and thus this class would
    //not make any sense.

    if (!mWindowContext_) {
        throw (std::exception("Exception Was Thrown While Attempting To Construct The\n"
            "ScreenCaptureAssistant object. It appears as though the thread\n"
            "this constructor was called upon does not have a current OpenGL\n"
            "context. Seeing as an OpenGL context is needed to do any rendering\n"
            "and the sole purpose of this class is to record rendered output, it\n"
            "is kinda sorta a major major issue that there is no context!\n"));
    }

    setScreenshotOutcomeCallback(defaultScreenshotResultCallbackFunction);
}


ScreenCaptureAssistant::~ScreenCaptureAssistant() noexcept {
    if (activeScreenshotTasks.size() > 0) {
        fprintf(MSGLOG, "Waiting for pending screen capture tasks to complete...\n");
        for (auto& task : activeScreenshotTasks)
            screenshotOutcomeReportCallback(task.get());
    }
}

#define TGA_ONLY
void ScreenCaptureAssistant::takeScreenshot(IMAGE_FILE_FORMAT format) noexcept {

#ifdef TGA_ONLY
    if (format != IMAGE_FILE_FORMAT::TGA) {
        fprintf(MSGLOG, "\n                         IMPORTANT!\n"
            "The requested file format for this operation is currently unsupported!\n"
            "Instead of whatever format it was you requested, your screenshot data will be\n"
            "stored using the '.tga' format since luckily for the developer its super easy\n"
            "to implement and thus is all that is supported currently!\n");
           
        format = IMAGE_FILE_FORMAT::TGA;
    }
#endif //TGA_ONLY

    try {

        /////////////////////////////////////////////////////////////////
        //   Verify That A Framebuffer Exists That Can Be Read From   // 
        ///////////////////////////////////////////////////////////////
        const auto spaceRequired = determineRequiredSpaceToHoldData();
        if (spaceRequired.bytesOfStorageRequired == 0) { //Make sure there is a framebuffer available to read from
             activeScreenshotTasks.emplace_back(forceBeginAsyncTask(&(ScreenCaptureAssistant::reportEmptyFramebuffer)), *this);
             return;
        }


        ///////////////////////////////////////////////////////////////
        //   Get a Unique Name to be Used For New Screenshot File   // 
        /////////////////////////////////////////////////////////////
        //Name will not yet include a file extension
        std::string screenshotName = getNextSequentialFilename(); 


        //////////////////////////////////////////////////////////////////////////////////////
        //   Prepare to have OpenGL write framebuffer pixel data into Application memory   //
        ////////////////////////////////////////////////////////////////////////////////////
        std::unique_ptr<std::vector<unsigned char>> screenCaptureData; 
        screenCaptureData = std::make_unique<std::vector<unsigned char>>();
        screenCaptureData->reserve(spaceRequired.bytesOfStorageRequired);
        //We must ensure there is no Pixel_Pack_Buffer currently bound or else or precious framebuffer 
        //data may get written to the wrong location. See this function definition for more 
        //details.
        auto pixelPackBuffer = makeSureThereIsNoPixelPackBufferBound();


        //////////////////////////////////////////////////////////////////////////////////////
        //   Prepare to have OpenGL write framebuffer pixel data into Application memory   //
        ////////////////////////////////////////////////////////////////////////////////////
        //Finally we are ready to read the data from the framebuffer. This is done using the
        //OpenGL call 'glReadPixels()'. 
        glReadPixels(0, 0, spaceRequired.fbWidth, spaceRequired.fbHeight, GL_BGR, GL_UNSIGNED_BYTE, screenCaptureData->data());


        ////////////////////////////////////////////////////////////////
        //   Restore the GL Context state if we modified it at all   //
        //////////////////////////////////////////////////////////////
        if (pixelPackBuffer != 0u)
            restoreToPixelPackBufferBinding(pixelPackBuffer);
        

        ///////////////////////////////////////////////////////////////////////////////////////////////
        //   Verify filesystem directory exists for screenshots and get a unique name for the file  //
        /////////////////////////////////////////////////////////////////////////////////////////////
        auto pathToScreenshots = ensureThatADirectoryForScreenshotsExists();




        //Now that we have read our framebuffer data off the GPU, we can prepare to 
        //transition to phase 2.
        

        //Determine which image format will be used for storage
        switch (format) {
        case IMAGE_FILE_FORMAT::TGA:
            std::unique_ptr<DataForTGA> tgaData = std::make_unique<DataForTGA>();

            break;
        default:
            throw std::exception("Not a supported image format!\n");
        }


        //Create the header for the '.tga' file
        DataForTGAHeader dataForHeader;
        dataForHeader.imageRequirements = spaceRequired;
        dataForHeader.capturedData.swap(screenCaptureData);

        auto header = createTGAHeader(dataForHeader);

        //At this point phase 1 is nearly complete. Thus we can now
        //create and beginning to prepare the object that will be 
        //returned at the end of this function. It turns out that we 
        //can do this all in one step.
        //return writeToFile(header, dataForHeader);
    }
    catch (const std::exception& e) {
        activeScreenshotTasks.emplace_back(forceBeginAsyncTask(&(ScreenCaptureAssistant::reportExceptionCaught)), *this, e.what());
        return;
    }
}


void ScreenCaptureAssistant::setScreenshotOutcomeCallback(ProcessScreenshotResultCallback psrc) noexcept {
    screenshotOutcomeReportCallback = psrc;
}






/////////////////////////////////////////////////////////
/////          Lower Level Functions          ///////////
/////////////////////////////////////////////////////////

ScreenCaptureRequirements ScreenCaptureAssistant::determineRequiredSpaceToHoldData() const noexcept {
    
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



    //The way we are going to be getting the pixel data from the active framebuffer is through
    //the OpenGL function 'glReadPixels()'. It must be noted that the behavior of glReadPixels is
    //dependent on the current state of the GL Context. If there is a buffer currently bound to the
    //GL_PIXEL_PACK_BUFFER binding target, then this function will write the framebuffer data into 
    //it. This isn't what we want though for taking screenshots, since the data just remains on the
    //GPU [more technically it could go any number of places based off the buffer bound]. The behavior
    //we are looking for with 'glReadPixels()' can be made to incur when there is no buffer bound to 
    //GL_PIXEL_PACK_BUFFER. Then the framebuffer's data will be written back out to where ever we want
    //by us simply giving it a pointer on where to start writing the data.
GLuint ScreenCaptureAssistant::makeSureThereIsNoPixelPackBufferBound() const noexcept {
    //First let us check to see what is bound to GL_PIXEL_PACK_BUFFER currently.
    GLint previous = 0u;
    glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &previous);
    //To make sure that no buffer is bound to GL_PIXEL_PACK_BUFFER, it suffices to simply bind the 
    //special reserved 0 buffer to displace any other buffers that may be bound.
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0u);
    return static_cast<GLuint>(previous);
}


void ScreenCaptureAssistant::restoreToPixelPackBufferBinding(const GLuint previouslyBoundBuffer) const noexcept {
    glBindBuffer(GL_PIXEL_PACK_BUFFER, previouslyBoundBuffer);
}

std::filesystem::path ScreenCaptureAssistant::ensureThatADirectoryForScreenshotsExists() const {
    //Navigate the filesystem to get to the directory where screenshots are stored. If no such
        //directory is found, then create one.
    std::filesystem::path pathToScreenshotsDirectory = FILEPATH_TO_SCREENSHOTS;
    if (!(std::filesystem::is_directory(pathToScreenshotsDirectory))) {
        fprintf(MSGLOG, "\nDetected that currently there does not exist a file system\n"
            "directory for storing screenshots. A directory for screenshots\n"
            "is being created at filepath:\n\t%s\n", pathToScreenshotsDirectory.lexically_normal().string().c_str());
        std::filesystem::create_directory(pathToScreenshotsDirectory);

       // std::filesystem::path fileName = pathToScreenshotsDirectory.string() + baseFileName + std::to_string(greatestScreenshotID++);
       // printf("\n\nScreenshot File's name will be: %s\n", fileName.string().c_str());
    }
}


std::string ScreenCaptureAssistant::getNextSequentialFilename() noexcept {
    //Iterate through every file in the directory whose name matches the prototype format to
    //get the largest number among files. Then, once found, increment it by one then assign this
    //number to the next screen shot and increment again and so on. Then all screenshots will get
    //a unique filename.

}

#if 0
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
#endif

ScreenshotOutcome ScreenCaptureAssistant::reportEmptyFramebuffer() const noexcept {
    ScreenshotOutcome outcome;
    outcome.success = false;
    outcome.msg = "Unable to take a screenshot due to there being no valid Framebuffer\n"
        "to pull data from!\n";
    return outcome;
}


ScreenshotOutcome ScreenCaptureAssistant::reportExceptionCaught(std::string exceptionMsg) const noexcept {
    ScreenshotOutcome outcome;
    outcome.success = false;
    outcome.msg = "An Exception was thrown during the screen capture process!\nMessage:\n\t";
    outcome.msg += exceptionMsg;
    return outcome;
}


#ifdef THIS_IS_FINISHED
ScreenshotOutcome ScreenCaptureAssistant::writeDataToFileAsTGA(const TGA_INTERNAL::TGA_HEADER& header,   //OUTCOME -> T.B.D.
    const DataForTGAHeader& dftgah) const {


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
#endif //THIS_IS_FINISHED


int ScreenCaptureAssistant::greatestScreenshotID = 0;




//See https://www.techiedelight.com/remove-elements-vector-inside-loop-cpp/
inline void ScreenCaptureAssistant::upkeepFunctionToBeCalledByRenderDemoBase() noexcept {

    //if (activeScreenshotTasks.size() > 0) {
        

}



    //Implementation-File-Only Function Definitions


void defaultScreenshotResultCallbackFunction(ScreenshotOutcome result) {
    if (result.success)
            fprintf(MSGLOG, "\nScreenshot successfully saved as %s", result.msg.c_str());
    else {
        fprintf(ERRLOG, "\nAn Issue Occurred While Trying To Take Screenshot.\n"
            "The reason for this error is:\n\t\t\"%\"", result.msg.c_str());
    }
}



