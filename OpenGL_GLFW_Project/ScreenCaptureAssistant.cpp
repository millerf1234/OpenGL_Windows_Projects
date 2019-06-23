
//#include <memory>
#include <vector>
#include <filesystem>
#include <set>   //Used to store the existing names while generating a unique filename
#include <string>
#include "ScreenCaptureAssistant.h"
#include "RelativeFilepathsToResources.h"
#include "ForceBeginAsyncTask.h"


//Note on using std::set
//   http://lafstern.org/matt/col1.pdf
// My usage case here though checks off 2 of his listed when-to-use-std::set
// conditions. Here there will be a potentially very random and potentially large
// number of insertion operations, will at most one look-up operation at the end.
// This could grow into the 1000's of insertions if there becomes a large number
// of screenshots over time.


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
        assert(glfwGetCurrentContext());
        /////////////////////////////////////////////////////////////////
        //   Verify That A Framebuffer Exists That Can Be Read From   // 
        ///////////////////////////////////////////////////////////////
        const auto spaceRequired = determineRequiredSpaceToHoldData();
        if (spaceRequired.bytesOfStorageRequired == 0) { //Make sure there is a framebuffer available to read from
            fprintf(MSGLOG, "\n"
                "\n;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;"
                "\n;                  Error -- Screenshot Request Denied!!!                     ;"
                "\n; Explanation: Upon Querying the Resolution of the Display Framebuffer To    ;"
                "\n               Get Its Most Recent Height And Width, the Strangest Of Things ;"
                "\n               Happened... Both The Height And Width Came Back Each As 0     ;"
                "\n               Pixels.                                                       ;"
                "\n               It Must Be Assumed That Surely A Request For A Screenshot     ;"
                "\n               Placed While This Application's OS-Leased Screen Real Estate  ;"
                "\n               Resides In An Abstract State Such That It Perhaps May Be Best ;"
                "\n               Described As A Subset Of The Null Set Would Have To Have Been ;"
                "\n               Enacted By Accident... I Mean, Common... How The Heck Is A    ;"
                "\n               0x0 Screenshot Even Possible! Try Rendering Something First!  ;"
                "\n;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;"
                "\n\n");
            // activeScreenshotTasks.emplace_back(forceBeginAsyncTask(&(ScreenCaptureAssistant::reportEmptyFramebuffer)), *this);
             return;
        }


        ///////////////////////////////////////////////////////////////
        //   Get a Unique Name to be Used For New Screenshot File   // 
        /////////////////////////////////////////////////////////////
        //Name will not yet include a file extension
        std::string screenshotName = getNextScreencaptureFilename(); 


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
        auto assignedFileName = getNextScreencaptureFilename();
        



        //Now that we have read our framebuffer data off the GPU, we can prepare to 
        //transition to phase 2.
        

        //Determine which image format will be used for storage
        //switch (format) {
        //case IMAGE_FILE_FORMAT::TGA:
            //std::unique_ptr<DataForTGA> tgaData = std::make_unique<DataForTGA>();

      //      break;
        //default:
       //     throw std::exception("Not a supported image format!\n");
      //  }

#if 0
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
#endif 
    }
    catch (const std::filesystem::filesystem_error& e) {
        fprintf(ERRLOG, "\n"
            "\n;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;"
            "\n;                  Error -- Screenshot Request Denied!!!                     ;"
            "\n; Explanation: Now This Can Be Simply Chalked Up To Plain Old Rotten Luck!   ;"
            "\n;              Everything Was Going Swimingly Until We Tried To Access The   ;"
            "\n;              OS's Filesystem. Yup. The Screenshot Failed Due To A Quite    ;"
            "\n               Unexpected \'std::filesystem::filesystem_error\' exception.   ;"
            "\n;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;"
            "\n Filesystem Exception Message:___.             [This Probably Just Some Lame  "
            "\n  __________                     |              Excuse From The OS On Why It  "
            "\n /          \\;-------------------*              Failed At Such A Mundane Task]"
            "\n \\->  \"%s\"\n\n\n", e.what());
    }
    catch (const std::exception& e) {
        fprintf(ERRLOG, "\n"
            "\n;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;"
            "\n;                  Error -- Screenshot Request Denied!!!                     ;"
            "\n; Explanation: Well It Turns Out That An Exception Was Thrown, Which Means   ;"
            "\n;              You May Have Just Found A Bug! Please Report The Following    ;"
            "\n;              Exception Message To Your Nearest Local Developer!            ;"
            "\n;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;"
            "\nException Message: %s\n\n\n", e.what());
        //activeScreenshotTasks.emplace_back(forceBeginAsyncTask(&(ScreenCaptureAssistant::reportExceptionCaught)), *this, e.what());
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

    //////////////////////////////////////////////////////////
    ////  To Query The Default Framebuffer, We Can Use any of:
    ////    -- glGetFramebufferAttachmentParameteriv()          
    ////    --
    ////
    ////
    //// [  see: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glGetFramebufferAttachmentParameter.xhtml  ]
    //// [       https://www.glfw.org/docs/latest/window_guide.html#window_attribs_fb                                ]


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
    }
    return pathToScreenshotsDirectory;
}


std::string ScreenCaptureAssistant::getNextScreencaptureFilename() {

    const auto DefaultScreenshotDirectory = ensureThatADirectoryForScreenshotsExists();
    //When taking a screenshot for the first time within a single lifetime of the process, the value of
    //nextCaptureID is checked. If this value is 0, then the entire screenshot directory is examined by
    //one of the following 2 ways:
    //   [(A) the highest trailing ID number among screenshots matching the default name pattern is found
    //        and that number + 1 is where our name-numbering scheme continues from]
    // or 
    //   [(B) the number of files present is counted, and then the resulting number + 1 is used as the
    //        next unique name ]
    constexpr const bool USE_METHOD_A = true;
    constexpr const bool USE_METHOD_B = !USE_METHOD_A;
    //Then for all subsequent screenshot attempts, this number will not be 0, so to create a new file
    //name for a screenshot all that is needed to be done is to increment this variable by 1 then 
    //append it to the end of the filename. 

    constexpr const char* BASE_SCREENSHOT_NAME = "ScreenCapture_";
    std::string assignedFilename = DefaultScreenshotDirectory.string() + BASE_SCREENSHOT_NAME;
    
    if (0ull == nextCaptureID) {
        glfwSetTimer(0.0);
        std::set<size_t> inUseFileNumbers;
        for (auto entry : std::filesystem::directory_iterator(DefaultScreenshotDirectory)) {

            bool skip = false;
            if (!(entry.is_regular_file())) {
                fprintf(WRNLOG, "\nDetected A \'Not-Regular-File\' in ScreenCaptures Directory by\n"
                    "the name of: \"%s\"\n", entry.path().filename().string().c_str());
                skip = true;
            }
            if (!(entry.path().has_filename())) {
                fprintf(WRNLOG, "\nDetected An Entry That Does Not Contain a File Name!\n"
                    "This Entry Is: \"%s\"\n", entry.path().string().c_str());
                skip = true;
            }
            if (skip) { continue; }

            if constexpr (USE_METHOD_A) {
                //Get the filename for our current directory entry
                auto filename = entry.path().filename();
                //Remove its extension 
                if (filename.has_extension())
                    filename.replace_extension();
                //Convert it into a string
                std::string filenameStr = filename.string();
                //Get an iterator to the end of the string
                auto strIter = filenameStr.crend();

                //These 2 variables are used to compute the files ID number
                size_t value = 0ULL;
                size_t decimalPlaceFactor = 1ULL;
                constexpr const size_t decimalPlaceGrowthFactor = 10ULL;

                //Iterate backwards until hit a backspace, recording each numerical value
                //along the way
                while ((*strIter) != '_') {
                    if (std::isdigit(*strIter)) {
                        value += static_cast<size_t>((*strIter) - '0') * decimalPlaceFactor;
                        decimalPlaceFactor *= decimalPlaceGrowthFactor;
                    }
                    else { //This means the file we are examining failed to conform 
                        //to the expected filename rules, so we can skip tracking it. 
                        goto SKIP_TO_NEXT_FILE;
                    }
                    strIter--;
                    if (strIter == filenameStr.crbegin())
                        goto SKIP_TO_NEXT_FILE;
                }

                //So we hit the underscore, which means we can stop recording numbers and 
                //instead now verify our base name string. We do this by comparing our iterator
                //with each character in BASE_SCREENSHOT_NAME.
                const char* baseNameIter = BASE_SCREENSHOT_NAME;
                size_t charactersToCompare = 0ULL;
                while (baseNameIter != '\0') {
                    charactersToCompare++;
                    baseNameIter++;
                }

                //Finally we are ready to do our comparison
                for (size_t i = 0ULL; i < charactersToCompare; i++) {
                    if ((*baseNameIter--) != (*strIter--))
                        goto SKIP_TO_NEXT_FILE;
                    else if (strIter == filenameStr.crbegin())
                        goto SKIP_TO_NEXT_FILE;
                }

                //At last, we have verified that we have a match. All that remains is to add the
                //captured value to the std::set
                if (inUseFileNumbers.insert(value).second || true) { //This line deserves some explanation. Calling
                    //'insert()' on a std::set will return as std::pair<iterator, bool>, with the bool 
                    //representing whether an element was found existing with the specified value or not.
                    //Thus our if statement here is checking this second value to determine if this file
                    //that passed all of the other tests is indeed unique truly unique.
                    fprintf(ERRLOG, "\nError generating a name for the screenshot! Found duplicate \n"
                        "files using ID %u . This means something somewhere got pretty messed up!\n"
                        "In fact I would like to know how this happened, so the program has\n"
                        "just now entered into a frozen state right at the instruction that found\n"
                        "the issue. Please investigate if able, and if not, then just simply:\n"
                        "                 [PRESS ENTER TO CONTINUE]\n", value);
                    std::cin.get();

                    nextCaptureID++;
                }
            }
            else if constexpr (USE_METHOD_B) {
                //Method B has a much simpler implementation
                nextCaptureID++;
            }
        SKIP_TO_NEXT_FILE:
            continue;
        }

        if constexpr (USE_METHOD_B) {
            if (nextCaptureID > 1ull)
                inUseFileNumbers.insert(nextCaptureID - 1ull);
            else
                inUseFileNumbers.insert(0u);
        }

        //Set the value to start writing new screenshot names from
        if (inUseFileNumbers.empty())
            nextCaptureID = 1ULL;
        else
            nextCaptureID = nextCaptureID + (*inUseFileNumbers.crend());

        assignedFilename += std::to_string(nextCaptureID++);
    }
    else {
        assignedFilename += std::to_string(nextCaptureID++);
    } 
    return assignedFilename;
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

#if 0
ScreenshotOutcome ScreenCaptureAssistant::reportExceptionCaught(std::string exceptionMsg) const noexcept {
    ScreenshotOutcome outcome;
    outcome.success = false;
    outcome.msg = "An Exception was thrown during the screen capture process!\nMessage:\n\t";
    outcome.msg += exceptionMsg;
    return outcome;
}
#endif 

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


size_t ScreenCaptureAssistant::nextCaptureID = 0ull;




//See https://www.techiedelight.com/remove-elements-vector-inside-loop-cpp/
void ScreenCaptureAssistant::upkeepFunctionToBeCalledByRenderDemoBase() noexcept {

    //if (activeScreenshotTasks.size() > 0) {
        

}



    //Implementation-File-Only Function Definitions


void defaultScreenshotResultCallbackFunction(ScreenshotOutcome result) {
    if (result.success)
            fprintf(MSGLOG, "\nScreenshot successfully saved as %s", result.msg.c_str());
    else {
        fprintf(ERRLOG, "\nAn Issue Occurred While Trying To Take Screenshot.\n"
            "The reason for this error is:\n\t\t\"%s\"\n\n", result.msg.c_str());
    }
}



