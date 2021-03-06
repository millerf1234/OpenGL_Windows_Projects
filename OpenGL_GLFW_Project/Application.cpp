//File:                 Application.cpp
//Description:          Implementation for the Application class. See header file for more detail
//
//Programmer:           Forrest Miller
//Date:                 July - November(and beyond) 2018

//#include <thread>
//#include <cstddef> //NOTE: Might not be necessary anymore to include <cstddef>



#include "Application.h"

#include "OpenGLEnumToString.h"   //Needed to convert OpenGL query results to strings

#include "Timepoint.h" //Used for timing of various aspects of the Application

//The RenderFemos are included in the order that they were written. As such, the 
//level of complexity in each project increases accordingly. 
#include "TeapotExplosion.h" 
//At this point I found it easier to piggy-back off of AssetLoadingDemo, so these
//RenderDemoes are all heavily built atop AssetLoadingDemo
#include "LightsourceTestDemo.h"
#include "AssetLoadingDemo.h"
#include "FlyingCameraDemo.h" //This one is built on top of 'AssetLoadingDemo'


void Application::initialize() {
    
#if (defined USE_OPTICK && (USE_OPTICK == 1))
    fprintf(MSGLOG, "  OPTICK PROFILING ENABLED!\n");
    OPTICK_START_CAPTURE(/*Optick::Mode::AUTOSAMPLING*/);
    OPTICK_EVENT("Capture Begun");
    //Set the path for where to save Optick capture
    globalOptickState.captureActive = true;
    auto currentPath = std::filesystem::current_path();
    currentPath.append("optickCaptures");
    globalOptickState.pathToSaveCapture = std::filesystem::absolute(currentPath);
    std::cout << "\n\nSaving Capture to: \n\t\"" << 
        globalOptickState.pathToSaveCapture << "\"\n\n\n";

    OPTICK_CATEGORY("InitializeApplication", Optick::Category::Scene);

#endif //USE_OPTICK == 1


    mApplicationValid = true;
    initReport = nullptr;
    glfwInitializer = nullptr;
    
    
    //If Application is built with macro USE_DEBUG_ defined, we will report more 
    //details during start-up about the Runtime Environment's configuration and
    //this Application's state
    if constexpr (USE_DEBUG) {
        //The compiler provides the macro '__cplusplus' used to identify
        //which version of C++ the code is compiling under  
        fprintf(MSGLOG, "[DEBUG:    __cplusplus is defined as %ld]\n\n", __cplusplus);
        
        //----------------------------------------------------------------
        // Print the ID for this thread (the Application's main thread)
        const auto threadID = std::this_thread::get_id();
        std::ostringstream threadIDString;
        threadIDString << "[ ~DEBUG~ ] Application Operating On Thread: 0x"
            << std::hex << threadID;
        fprintf(MSGLOG, "\n%s\n", threadIDString.str().c_str());

        //----------------------------------------------------------------
        // Print the maximum number of supported concurrent threads this system reports
        // it supports. This value is meant only to serve as a hint, and on some 
        // platforms it the returned value may be 0
        const auto supported_concurrent_threads_count = std::thread::hardware_concurrency();
        std::ostringstream supportedConcurrentThreadsMessage;
        supportedConcurrentThreadsMessage << "" 
            << "[ ~DEBUG~ ] System Reports Its Maximum Supported Number Of Threads \n"
            << "            That Can [Theoretically] Run Concurrently Is:    "
            << "     " << supported_concurrent_threads_count << "\n";

        fprintf(MSGLOG, "\n%s\n", supportedConcurrentThreadsMessage.str().c_str());

        //----------------------------------------------------------------
        // Print the maximum number of bytes a multi-byte character may be
        // on this platform (only do so if MB_CUR_MAX is defined)
#ifdef MB_CUR_MAX
        const uint64_t MAX_BYTES_IN_A_MULTIBYTE_CHAR =
            static_cast<uint64_t>(MB_CUR_MAX);
        const std::string mbCurMaxMessage = std::string(""
            "[ ~DEBUG~ ] Maximum number of bytes allowed in a multi-byte\n"
            "            character on this platform:                     "
            "      " + std::to_string(MAX_BYTES_IN_A_MULTIBYTE_CHAR));
        if (MAX_BYTES_IN_A_MULTIBYTE_CHAR > static_cast<uint64_t>(1u))
            fprintf(MSGLOG, "%s Bytes\n", mbCurMaxMessage.c_str());
        else 
            fprintf(MSGLOG, "%s Byte\n", mbCurMaxMessage.c_str());
#endif //MB_CUR_MAX

        //----------------------------------------------------------------
        // Print the default alignment all dynamic memory allocations made
        // during runtime on this platform will adhere to (unless a 
        // non-default alignment is requested using 'std::align_as'.
        //     (Only do this if __STDCPP_DEFAULT_NEW_ALIGNMENT__ is defined)
#ifdef __STDCPP_DEFAULT_NEW_ALIGNMENT__
        static constexpr const uint64_t defaultAlignmentForDynamicMemoryAllocations =
            static_cast<uint64_t>(__STDCPP_DEFAULT_NEW_ALIGNMENT__);
        const std::string defaultDynMemAlignmentMessage = std::string(""
            "[ ~DEBUG~ ] Current Default Alignment Of Every Dynamic Memory\n"
            "            Allocation Which Doesn't Use 'align_as':          ") +
            std::to_string(defaultAlignmentForDynamicMemoryAllocations);
        fprintf(MSGLOG, "\n%s-bits\n", defaultDynMemAlignmentMessage.c_str());
#endif // __STDCPP_DEFAULT_NEW_ALIGNMENT__
    }
    

    //I've decided to leave the fancy FP behavior stuff alone for now...
#ifdef GET_FANCY_WITH_RESTRICTING_FLOATING_POINT_BEHAVIOR
    //Apparently this project has a file found at source->utility->math->'FloatingPointTolerance.h'
    //The stuff I started here might be more at home over in this file
#ifdef ADD_THESE_TO_TOP_OF_THE_FILE_TO_ENABLE_FANCY_FP_CUSTOMIZATION_OPTIONS 
#pragma STDC FENV_ACCESS ON 
#include <cfloat>
    //Look into <cfenv> as well: https://en.cppreference.com/w/cpp/header/cfenv
#endif //ADD_THESE_TO_TOP_OF_THE_FILE_TO_ENABLE_FANCY_FP_CUSTOMIZATION_OPTIONS 
    
    fprintf(MSGLOG, "Floating Point Behavior: \n");

        //----------------------------------------------------------------
        // [Requires C++11 or newer plus the header <cfloat> must be included]
        // Find out from the implementation what the precision in which all
        // floating-point arithmetic operations other than assignment and cast
        // are done at. Note this applies only on the CPU side, how OpenGL 
        // and the GPU are performing their floating point calculations is 
        // dependent on the OpenGL implementation and not the compiler. Also,
        // I'm not sure but there is a good chance the GLM library may 
        // have some additional influence over how it's FP operations are 
        // performed beyond what is reported here. 
        // Reference: https://en.cppreference.com/w/cpp/types/climits/FLT_EVAL_METHOD
#ifdef FLT_EVAL_METHOD 
    //Don't use unsigned types here because FLT_EVAL_METHOD may be negative
    static constexpr const int64_t DEFAULT_FP_EVAL_METHOD = 
        static_cast<int64_t>(FLT_EVAL_METHOD);
    static constexpr const int64_t FLT_EVAL_METHOD_UNKNOWN = static_cast<int64_t>(-1);
    static constexpr const int64_t FLT_EVAL_METHOD_RESPECT_STORAGE_TYPE = static_cast<int64_t>(0);
    static constexpr const int64_t FLT_EVAL_METHOD_FORCE_DOUBLE = static_cast<int64_t>(1);
    static constexpr const int64_t FLT_EVAL_METHOD_FORCE_LONG_DOUBLE = static_cast<int64_t>(2);
    std::ostringstream fltEvalMessage;
    fltEvalMessage << std::string("FP operations and constants evaluate to minimum precision level of: ");
    switch (DEFAULT_FP_EVAL_METHOD + 1) { //I feel like there should be a way to make this switch 
    case FLT_EVAL_METHOD_UNKNOWN:     //statement take advantage of the types being constexpr
        fltEvalMessage << "Unspecified/Unknown";
        break;
    case FLT_EVAL_METHOD_RESPECT_STORAGE_TYPE:
        fltEvalMessage << "Respect Storage Type";
        break;
    case FLT_EVAL_METHOD_FORCE_DOUBLE:
        fltEvalMessage << "Force to double";
        break;
    case FLT_EVAL_METHOD_FORCE_LONG_DOUBLE:
        fltEvalMessage << "Force to long double";
        break;
    default:
        if (DEFAULT_FP_EVAL_METHOD < FLT_EVAL_METHOD_UNKNOWN)
            fltEvalMessage << "Implementation Controlled";
        else
            fltEvalMessage << "Implementation Response Invalid";
        break;
    }
    fprintf(MSGLOG, "%s\n", fltEvalMessage.str().c_str());
#endif //FLT_EVAL_METHOD

        //----------------------------------------------------------------
        //  FP Rounding Behavior   [To do]  (this is LOW LOW LOW priority)
        //https://en.cppreference.com/w/cpp/types/climits/FLT_ROUNDS

#endif //GET_FANCY_WITH_RESTRICTING_FLOATING_POINT_BEHAVIOR


    fprintf(MSGLOG, "Application is loading...\n");
    
    // STEP 1     INITIALIZE GLFW, Create an OpenGL Context and Acquire Our
    //            Application's Window 
    if ( !setupGLFW() ) {
        mApplicationValid = false;
        fprintf(ERRLOG, "\nThe application encountered an error setting up GLFW!\n");
        return;
    }
    else {
        //If GLFW initializes but then we fail later on in the initialization, 
        //we run into a problem that the window we opened with GLFW may not want to relinquish focus
        //back to the OS/Desktop (depending on how it is configured). To ensure the user is able to 
        //see any potential error message, we manually ensure the Application's window relinquishes 
        //its focus for now until we have reached the end of the Application's constructor.
        GLFWwindow* currentWindow = glfwGetCurrentContext();
        if (currentWindow) 
            glfwIconifyWindow(currentWindow);
        
    }
    
    // STEP 2      Once GLFW has been successfully initialized, move on to loading 
    //             definitions for the GL function pointers. This is a really tedious
    //             process that we luckily are able to defer to third party library 
    //            'glad'. 
    if ( !loadGraphicsLanguageFunctions() ) {
        mApplicationValid = false;
        
        fprintf(ERRLOG, "\n"
            "The application encountered an error while loading the graphics language!\n"
            "        (Please Ensure You Have An OpenGL %d.%d Compatible Graphics\n"
            "               Driver Installed And Updated To Its Most Recent Version)\n\n"
            "\t[Press ENTER to end the process]\n\n",
                        DEFAULT_OPENGL_VERSION_MAJOR,
                        DEFAULT_OPENGL_VERSION_MINOR);
        std::cin.get();
        return;
    }
    
    //  Step 3        Once GLFW and glad have completed their required initialization procedures,
    //                the Application performs a lot of it's own self-configuration now before it is
    //                ready to return from its constructor. This configuration process involves setting
    //                all of the necessary callback functions and setting the initial state of the 
    //                OpenGL context.
    configureGraphicsContextDebugCallbackFunctions(); //The behavior of context debugging is set in Header Files -> SetupAndConfiguration -> Modifiable -> DebugBehavior -> "DebugSettings.h"
    setInitialGLState();
        
    //Check to see if GLFW supports Raw Mouse Motion Input
    //  (see: https://www.glfw.org/docs/latest/group__input.html#gae4ee0dbd0d256183e1ea4026d897e1c2 )
    if (glfwRawMouseMotionSupported())
        fprintf(MSGLOG, "\n    [Detected that raw mouse input is supported on this platform!\n");
    else 
        fprintf(MSGLOG, "\n    [Detected that raw mouse input is unsupported on this platform!\n");
    
    //MSAA requires Framebuffer Objects to be used, which I have not yet finished implementing.
    //As such, this function currently is a no-op until Framebuffers enter into use.
    checkMSAA(); //See if MSAA is being used as expected
    
    if (!mApplicationValid) {
        fprintf(ERRLOG, "The application encountered an error while loading!\n");
        return;
    }
    else {
        fprintf(MSGLOG, "Application loading complete!\nPlaying Intro Movie...\n");
        playIntroMovie(); 
    }
}


Application::Application() noexcept {

    //The catch blocks for the inner try statement contain operations which 
    //may throw exceptions. Thus the need for the double layered exception
    //handling
    try {
        try {
            initialize();
            if (!mApplicationValid) {
                //fprintf(MSGLOG, "\n\t[Press Enter to abort]\t");
                //std::cin.get(); //Need this on Windows to keep the terminal window open
                safeCrash();
            }
            else { //Else we are successfully initialized and can restore the window
                GLFWwindow* currentWindow = glfwGetCurrentContext();
                if (currentWindow)
                    glfwRestoreWindow(currentWindow);
                else
                    safeCrash();
            }
        }

        catch (const std::system_error & e) {
            mApplicationValid = false;
            fprintf(ERRLOG, "\nCaught a system error exception:\n\t%s\n", e.what());
            fprintf(ERRLOG, "\n\n  [Well this is kinda awkward... For once it appears\n"
                "   as though the reason for crashing is not due to poor\n"
                "   work by the Application programmer but in fact a system\n"
                "   error. Best Just Do What Everyone Else Does And Blame Windows\n"
                "   [Even if you are running this on Linux].\n");
            safeCrash();
        }
        catch (const std::exception & e) {
            mApplicationValid = false;
            fprintf(ERRLOG, "\n\n\n\t[In Application's Constructor]\n\t"
                "Error! Exception Encountered!\n"
                "EXCEPTION MSG: \n\t%s\n\n",
                e.what());
            //assert(false);
            safeCrash();
        }

        catch (...) {
            fprintf(ERRLOG, "\n\n\n\t[In Application Constructor]\n\tError! UNKNOWN EXCEPTION ENCOUNTERED!\n");
            //assert(false);
            safeCrash();
        }
    }
    catch (...) {
        fprintf(ERRLOG, "\n\nCaught an exception while handling a previously\n"
            "caught exception thrown during the Application's\n"
            "construction.\n");
        safeCrash();
    }
}


Application::~Application() noexcept {
#if (defined USE_OPTICK && (USE_OPTICK == 1))
    try {
        if (globalOptickState.captureActive) {
            //Tell Optick to stop capturing profiling data
            fprintf(MSGLOG, "\n\nFinalizing OPTICK capture...");
            OPTICK_STOP_CAPTURE();
            globalOptickState.captureActive = false;
            fprintf(MSGLOG, "DONE\n");

            //Write the OPTICK capture data to file 
            fprintf(MSGLOG, "Writing OPTICK capture to file...");
            const std::string optickSavePathStr = globalOptickState.pathToSaveCapture.string() + std::string(R"(\)");
            OPTICK_SAVE_CAPTURE(optickSavePathStr.c_str());
            fprintf(MSGLOG, "DONE\n");
            fprintf(MSGLOG, "\t\t\t\t\t\t[OPTICK CAPTURE SAVED]\n");
            fprintf(MSGLOG, "\t  ~~OPTICK capture info~~\n    Filename: %s\n    Location: %s\n\n",
                "[AUTO-GENERATED].opt", optickSavePathStr.c_str());

            //Just to be polite, tell Optick to prepare itself for process termination
            fprintf(MSGLOG, "Cleaning up OPTICK after successful capture...");
            OPTICK_SHUTDOWN();
            fprintf(MSGLOG, "DONE\n\n\n");

        }
    }
    catch (...) {
        fprintf(ERRLOG, "\nOptick Capture Failed!\n");
    }
#endif //USE_OPTICK == 1
    
    //if (glfwInitializer) {
    //    glfwInitializer->terminate();  
    //}

}


void Application::launch() {

    OPTICK_CATEGORY("LaunchApplication", Optick::Category::Camera);
    OPTICK_EVENT();

    try {
        if (!mApplicationValid) {
            fprintf(ERRLOG, "\nError launching Application, Application is invalid!\n");
            return;
        }

        fprintf(MSGLOG, "Loading Main Menu...\n"); //The context is active at this point, so any choice of what to load should happen on the render surface.
        //have a function here that runs a main menu...  this menu returns a struct consisting of an enum for which mode/sim to launch and then additional configuration parameters (num_players, control scheme, etc...)

        fprintf(MSGLOG, "Application is ready to load a specific program...\n");


        ////////////////////////////////////////////////////////////////////////////
        /////    IF U WANT TO RUN SOME OPENGL EXPERIMENTS, CALL FOLLOWING FUNCTION
        ////////////////////////////////////////////////////////////////////////////
        //runOpenGLSandboxExperiments();


        //fprintf(MSGLOG, "\n\n[Here will eventually be a list of available demos to load and run]\n\n");

        //fprintf(MSGLOG, "\nSelected FlyingCameraDemo.\n");
        //runFlyingCameraDemo();
        //return;

        fprintf(MSGLOG, "\nSelected AssetLoadingDemo.\n");
        runAssetLoadingDemo();
        return;

       // fprintf(MSGLOG, "\nSelected LightsourceTestDemo.\n");
       // runLightsourceTestDemo();
       // return;

        //fprintf(MSGLOG, "\nSelected TeapotExplosionDemo.\n");
        //runTeapotExplosionDemo();
        //return;

    }
    catch (const std::system_error& e) {
        fprintf(stderr, "\nCaught a system error exception:\n\t%s\n", e.what());
        fprintf(stderr, "\n\n"
            "  [Well this is kinda awkward... For once it appears\n"
            "   as though the reason for crashing is not due to poor\n"
            "   work by the Application programmer but in fact a system\n"
            "   error. Best Just Do What Everyone Else Does And Blame Windows]\n");
        safeCrash();
    }

    catch (const std::exception& e) {
        fprintf(ERRLOG, "\n\n\n\n\n\n\n\n[In Application's 'run()' function]\n"
            "\n\tError! Exception Encountered!\n"
            "EXCEPTION MSG: %s\n\n\n\n", e.what());
        safeCrash();
    }


    catch (...) {
        fprintf(ERRLOG, "\n\n\n\n\n\n\n\n\t[In Application's 'run()' function]\n"
            "\n\t\tError! UNKNOWN EXCEPTION ENCOUNTERED!\n\n\n\n");
        safeCrash();
    }

}



bool Application::setupGLFW() {
    OPTICK_EVENT();
    fprintf(MSGLOG, "Loading GLFW...\n");
   
    glfwInitializer = std::make_unique<GLFW_Init>();
    glfwInitializer->setDefaultMonitor(MONITOR_TO_USE);
    initReport = glfwInitializer->initialize();
    
    if (!initReport) {
        fprintf(ERRLOG, "\nAn Error Occurred Setting Up The GLFW Window!\n");
        mApplicationValid = false;
        return false;
    }
    else {
        //printf("Init report values: OpenGL %d . %d \n", initReport->windowContext.context.versionMajor, initReport->windowContext.context.versionMinor);
        glfwInitializer->specifyWindowCallbackFunctions();
        //glfwInitializer->setWindowUserPointer(static_cast<void *>(initReport.get())); 
        return true;
    }
}

bool Application::loadGraphicsLanguageFunctions() {
    OPTICK_EVENT();
    //Load OpenGL functions once window context has been set
    fprintf(MSGLOG, "Loading Graphics Language Functions...\n");
    int success = gladLoadGL();
    if (!success) {
        fprintf(ERRLOG, "\nERROR OCCURED LOADING GRAPHICS LANGUAGE FUNCTIONS!\n");
        mApplicationValid = false;
        return false;
    }

    reportDetailsFromGLImplementation();
    reportDetailsFromGLImplementationOnTextureUnitLimitations();
    return true;
}

void Application::reportDetailsFromGLImplementation() {
    OPTICK_EVENT();
    //if (GLAD_GL_EXT_framebuffer_multisample) {
    //	// GL_EXT_framebuffer_multisample is supported 
    //}

    fprintf(MSGLOG, "  Graphics Language loaded.\n");

    fprintf(MSGLOG, "\tGraphics Language Version:         OpenGL %s\n",
        glGetString(GL_VERSION));
    fprintf(MSGLOG, "\tGraphics Shading Language Version: %s\n",
        glGetString(GL_SHADING_LANGUAGE_VERSION));
    fprintf(MSGLOG, "\tGraphics Language Vendor:          %s\n",
        glGetString(GL_VENDOR));
    fprintf(MSGLOG, "\tGraphics Rendering Device:         %s\n",
        glGetString(GL_RENDERER));

    //Worth mentioning: There are also details about the OpenGL context already stored 
    //                  internally by this class at initReport->windowContext.context. [values here]
    //                  which are provided by GLFW during initialization
}

void Application::reportDetailsFromGLImplementationOnTextureUnitLimitations() {
    OPTICK_EVENT();

    std::ostringstream texLimReport;

    texLimReport << "\n"
        << "\t\t   +-------------------------------+\n"
        << "\t\t   | TEXTURE UNIT OPERATING LIMITS |\n"
        << "\t\t   +-------------------------------+\n";

    //Maximum Supported Texture Size
    GLint maxTexSize = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTexSize);
    texLimReport << "\t  Maximum Texture Size:                         "
        << maxTexSize << "\n";

    //Maximum Supported Texture Size [CubeMap]
    GLint maxTexSizeCubemap = 0;
    glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &maxTexSizeCubemap);
    texLimReport << "\t  Maximum Texture Size [CUBEMAP]:               "
        << maxTexSizeCubemap << "\n";

    //Maximum Supported Simultaneous Texture Units
    GLint maxTexUnits = 0;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTexUnits);
    texLimReport << "\t  MAXIMUM SUPPORTED SIMULTANEOUS TEXTURE UNITS: "
        << maxTexUnits << "\n";

    //Supported Compressed Texture Formats Count
    GLint numSupportedCompressedTextureFormats = 0;
    glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS,
                  &numSupportedCompressedTextureFormats);
    texLimReport << "\t  SUPPORTED COMPRESSED TEXTURE FORMATS:         "
        << numSupportedCompressedTextureFormats << "\n";
    

    //Supported Compressed Texture Formats Array 
    // [Some of these formats supported by my computer are 
    //  really old and aren't part of Modern OpenGL]
    //  See: https://www.khronos.org/registry/OpenGL/extensions/OES/OES_compressed_paletted_texture.txt
    constexpr const bool REPORT_SUPPORTED_COMPRESSED_TEXTURE_FORMATS_TO_CONSOLE = false; //Move this declaration somewhere more visible
    if constexpr (REPORT_SUPPORTED_COMPRESSED_TEXTURE_FORMATS_TO_CONSOLE) {
        GLint* supportedCompTexFormats = new GLint[numSupportedCompressedTextureFormats];
        if (!supportedCompTexFormats) {
            fprintf(ERRLOG, "\nMemory Allocation Failed!\n");
            std::exit(EXIT_FAILURE);
        }
        for (GLint i = 0; i < numSupportedCompressedTextureFormats; i++) {
            supportedCompTexFormats[i] = 0;
        }
        glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, &(supportedCompTexFormats[0]));
        for (GLint i = 0; i < numSupportedCompressedTextureFormats; i++) {
            if (supportedCompTexFormats[i] == 0)
                break;
            texLimReport << "\t[0x" << std::hex << supportedCompTexFormats[i]
                << std::dec << "]   " << convertGLEnumToString(supportedCompTexFormats[i])
                << "\n";
        }
        delete[] supportedCompTexFormats;
        supportedCompTexFormats = nullptr;
    }
    
    
    //EXPERIMENT WITH USING A QUERY OBJECT
    //WARNING! THIS IS NOT HOW TIMESTAMP QUERIES WORK! [See OpenGL SuperBible (7e) P.547]
    //Get a timestamp from the context just to see what happens
    //GLuint timestampQuery = 0u;
    //glGenQueries(1, &timestampQuery);
    //glQueryCounter(GL_TIMESTAMP, timestampQuery);
    //GLuint readTimeVal = 0u;
    //glGetQueryObjectuiv(timestampQuery, GL_QUERY_RESULT, &readTimeVal);
    //glDeleteQueries(1, &timestampQuery);
    
    fprintf(MSGLOG, "%s\n", texLimReport.str().c_str());
}



void Application::configureGraphicsContextDebugCallbackFunctions() const {
    OPTICK_EVENT();
    fprintf(MSGLOG, "Graphics Context Debug Output Callback Messaging: ");
    if (USE_DEBUG) {   //set in ProjectParameters.h
        fprintf(MSGLOG, "Enabled\n");
        glEnable(GL_DEBUG_OUTPUT);
        if (FORCE_SYNC_BETWEEN_CONTEXT_AND_APP) {
            fprintf(MSGLOG, "\t[For now] Forcing Synchronization Between Graphics Context and Application...\n\n");
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); //Might cause problems if context gets multithreaded
        }
        else {
            fprintf(MSGLOG, "\nWARNING! GL Context has been configured as a debug context but without\n"
                "forced synchronization between the context and the application.\n"
                "This means Context Callback messages may appear a long time after the\n"
                "event that caused them!\n"
                "[Re-enable debug output synchronization or disable debugging to remove this warning!]\n");
            glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        }
        glDebugMessageCallback(printGraphicsContextMessageCallback, nullptr);
    }
    else {
        fprintf(MSGLOG, "Disabled\n\n");
    }
}

void Application::setInitialGLState() {
    OPTICK_EVENT();
    fprintf(MSGLOG, "\nInitializing GL StateMachine...\n");
    
    //For options of what can be activated/deactivated
    //with glEnable()/glDisable(), see:
    //   https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glEnable.xml
    
    fprintf(MSGLOG, "\tActivating GL depth-test\n");
    glEnable(GL_DEPTH_TEST); //Depth Test will be used for every draw call, so it is activated globally
}

void Application::checkMSAA() const { 
    
    //This function's current implementation does not 
    constexpr bool CHECK_MSAA_IMPLEMENTATION_HAS_BEEN_FIXED = false;

    if (mApplicationValid) {
        //glad_glEnable(GL_MULTISAMPLE); //Need framebuffer objects for proper MSAA
        GLint bufs = -1;
        GLint samples = -1;

        //glad_glGetIntegerv(GL_SAMPLE_BUFFERS, &bufs);
        //glad_glGetIntegerv(GL_SAMPLES, &samples);
        //fprintf(MSGLOG, "\nMSAA CONFIGURATION:\n\tBuffers Available: %d\n\tSamples: %d\n\n", bufs, samples);

        glGetIntegerv(GL_SAMPLE_BUFFERS, &bufs);
        glGetIntegerv(GL_SAMPLES, &samples);
        fprintf(MSGLOG, "\nMSAA CONFIGURATION:\n\tBuffers Available: %d\n\tSamples: %d\n\n", bufs, samples);
    }
}


void Application::playIntroMovie() noexcept {
    fprintf(MSGLOG, "PSYCH! There is no intro movie!\n");
}


void Application::runOpenGLSandboxExperiments() {
#if 0
    assert(initReport);
    assert(initReport->windowContext.window.window);
    
    GLFWwindow* window = initReport->windowContext.window.window;

    glfwIconifyWindow(window);

    //For ideas, see:
    //           https://learnopengl.com/Advanced-OpenGL/Advanced-Data

    static constexpr const size_t DATA1_SIZE = 50000;      //500,00
    static constexpr const size_t DATA2_SIZE = 800000;   //8,000,00
    static constexpr const size_t DATA3_SIZE = 2000000; //20,000,00

    fprintf(MSGLOG, "\n\n\nStart of the OpenGL Sandbox Experiments!\n");
    fprintf(MSGLOG, "\nFor Our Setup We Need To Generate Some Large Data Sets...\n");
    LocalTimepoint dataGenStart("Start Data Generation");

    std::vector<GLfloat> largeData1, largeData2, largeData3;
    largeData1.reserve(DATA1_SIZE);
    largeData2.reserve(DATA2_SIZE);
    largeData3.reserve(DATA3_SIZE);

    fprintf(MSGLOG, "Memory Allocated!\n");

    LocalTimepoint memAllocated("Memory For Data Allocated");

    for (auto i = 0; i < DATA1_SIZE; i++) {
        largeData1.push_back(MathFunc::getRandomInRangef(-1.0f, 1.0f));
    }
    LocalTimepoint data1Ready("Data Set 1 Ready!\n");

    for (auto i = 0; i < DATA2_SIZE; i++) {
        if (i < DATA1_SIZE)
            largeData2.push_back(largeData1.at(i));
        else
            largeData2.push_back(MathFunc::getRandomInRangef(-1.0f, 1.0f));
    }
    LocalTimepoint data2Ready("Data Set 2 Ready!\n");

    for (auto i = 0; i < DATA3_SIZE; i++) {
        if (i < DATA2_SIZE) 
            largeData3.push_back(largeData2.at(i));
        else
            largeData3.push_back(MathFunc::getRandomInRangef(-2.0f, 2.0f));
    }
    LocalTimepoint data3Ready("Data Set 3 Ready!\n");


    fprintf(MSGLOG, "\nTotal Time Spent Generating Data:  %f sec\n", data3Ready - dataGenStart);

    fprintf(MSGLOG, "\nTime Spent Allocating Memory:  %f  sec\n", memAllocated - dataGenStart);

    fprintf(MSGLOG, "\nTime Spent on Generating:\n"
                    "\t\tD1:  %f  sec\n"
                    "\t\tD2:  %f  sec\n"
                    "\t\tD3:  %f  sec\n",
                    data1Ready - memAllocated,
                    data2Ready - data1Ready,
                    data3Ready - data2Ready);






    fprintf(MSGLOG, "\n\n\n\t[Press Enter To Continue To A Render Demo]");
    std::cin.get();
    glfwRestoreWindow(window);
    //glfwFocusWindow(window); //Not necessary
#endif //#if 0
}



void Application::runRenderDemo(std::unique_ptr<RenderDemoBase>& renderDemo, const char * name) {
    bool demoHasNameProvided = ((name != nullptr) && (*name != '\0'));
    OPTICK_CATEGORY("RenderDemoSelected", Optick::Category::Navigation )
    OPTICK_EVENT("ErrorCheckingRenderDemo");
    //Perform error check
    if (renderDemo == nullptr) {
        if (demoHasNameProvided) {
            fprintf(ERRLOG, "\n\n\t(While trying to run RenderDemo %s)\n", name);
        }
        else {
            fprintf(ERRLOG, "\n\n");
        }
        fprintf(ERRLOG, "\nWhoah! A 'nullptr' was passed in as the RenderDemo to\nthis Application's "
            "member function 'runRenderDemo()'.\n"
            "Passing around nullptrs is very dangerous, luckily this time it was caught.\n"
            "Only reasonable thing to do here is crash and exit gracefully...\n  Demo is exiting!\n");
        fprintf(MSGLOG, "\t  [ Press ENTER to continue ]\n");
        std::cin.get(); //Require acknowledgment from programmer that he/she/they messed up big time before crashing 
        std::exit(EXIT_FAILURE);
    }
    //Perform a second error check
    else if ( !(initReport && mApplicationValid) ) {
        fprintf(ERRLOG, "\nError launching RenderDemo!\n"
            "The Application is invalid or the detected display information is null!\n");
    }
    //Else proceed with loading and launching the RenderDemo
    else {
        if (demoHasNameProvided) {
            fprintf(MSGLOG, "Loading %s... \n", name);
        }
        
        try {
            OPTICK_EVENT("LoadingRenderDemoAssets");
            renderDemo->loadAssets();
            OPTICK_EVENT("AllAssetsLoaded");
            renderDemo->run();
        }
        catch (const std::exception& e) {
            fprintf(ERRLOG, "\nThe following exception was thrown inside the render demo!\n");
            fprintf(ERRLOG, "%s\n", e.what());
            fprintf(ERRLOG, "\n\t[Program will now crash...]\n");
            std::exit(EXIT_FAILURE);
        }
    }
}

void Application::runFlyingCameraDemo() {
    OPTICK_EVENT();
    std::unique_ptr<RenderDemoBase> flyingCameraDemo = std::make_unique<FlyingCameraDemo>(initReport.get());
    runRenderDemo(flyingCameraDemo, "Asset Loading Demo");
}

void Application::runAssetLoadingDemo() {
    OPTICK_EVENT();
    std::unique_ptr<RenderDemoBase> assetLoadingDemo = std::make_unique<AssetLoadingDemo>(initReport.get());
    runRenderDemo(assetLoadingDemo, "Asset Loading Demo");
}

void Application::runLightsourceTestDemo() {
    OPTICK_EVENT();
    std::unique_ptr<RenderDemoBase> lightsourceTestDemo = std::make_unique<LightsourceTestDemo>(initReport.get());
    runRenderDemo(lightsourceTestDemo, "Lightsource Test Demo");
}

void Application::runTeapotExplosionDemo() {
    OPTICK_EVENT();
    std::unique_ptr<RenderDemoBase> teapotExplosionDemo = std::make_unique<TeapotExplosion>(initReport.get());
    runRenderDemo(teapotExplosionDemo, "Teapot Explosion Demo");
}




//
//  Call this function to crash the Application in a controlled manner.
//
//                    IMPLEMENTATION NOTE 
// This function is expecting for the following 'cleanup' steps to each
// be performed by their own 'atexit' callback functions.
//                   - 
//                   This way each 'atexit'
//                           callback can be specified only once the
//                     component in question which needs cleanup has
//                        been created/allocated by the Application.
//                   
void Application::safeCrash() noexcept {
    OPTICK_CATEGORY("SafeCrashInitiated", Optick::Category::Debug)
    OPTICK_EVENT("SafeCrashInitiated");

    //todo Look into this more first before acting but I feel like
    //most of the 'clean-up' code for this project's dependencies 
    //(i.e. such as for GLFW or Optick) never gets called if the 
    //process finds itself at this point in it's execution. Since 
    //knowing the states of these dependencies is important, perhaps
    //have the dependency clean-up happen in a non-static function which
    //then calls this function to finalize everything. (actually though if
    //the clean-up takes any nominal length of time, the render window isn't 
    //minimized until this function runs. Perhaps the minimizing of the render
    //window should get moved up to the start of all the cleanup processes)


    try {
        //First, attempt to iconify (minimize) any Application window 
        //so that it isn't in the way of allowing our console error messages
        //to be displayed
        GLFWwindow* potentiallyFullscreenWindow = glfwGetCurrentContext();
        if (potentiallyFullscreenWindow)
            glfwIconifyWindow(potentiallyFullscreenWindow);
            
        //Now we can proceed with reporting our error messages and crashing...

        fprintf(ERRLOG, "\n\n"
            "An event occurred which has caused this Application\n"
            "to enter into a Controlled Crash Protocol Sequence!\n\n\n\n"
            "      [Press Enter To Acknowledge And Abort]");
        //Have the user acknowledge that there is an issue before terminating
        std::cin.get(); 
        fprintf(ERRLOG, "\n\n\nBeginning Controlled Exit Procedure...\n");
    }
    catch (...) {
        fprintf(ERRLOG, "\n\n"
            "An Error Occurred While Preparing For A Controlled Crash\n");
    }


    //WARNING!!! Calling 'std::exit(EXIT_FAILURE)' (seems to) skip all destructor
    //calls. However, safeCrash has been documented as guaranteeing the termination
    //of the process. So really I need to either make sure none of my clean-up
    //routines are dependent on any object destructor functions getting called
    //(which unfortunately several rather-major clean-up requirements currently
    //do) or I need to rethink error-handling entirely so this safeCrash() function
    //is no longer required.
    std::exit(EXIT_FAILURE);
}