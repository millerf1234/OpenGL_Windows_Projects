//File:             AssetLoadingDemo.h
//
//  +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
//  |   FYI         Currently the 'AssetLoadingDemo.cpp' source file has much more       |
//  |               thorough documentation than can be found here in the header.         |
//  +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
//
//Description:      This header file contains the definition for a class which inherits from
//                  RenderDemoBase, thus making this class represent a RenderDemo. 
//                  The point of this RenderDemo is to demonstrate basic object parsing/loading
//                  from '.obj' 3D model files. In order to ensure model loading is done properly,
//                  this RenderDemo uses its own utility class classed QuickObject. Note that 
//                  the 'Quick' in QuickObject is referring to the amount of time I spent implementing
//                  the parser/loader rather than the speed at which it parses. Since the parsing is 
//                  simpler though there is a good chance it runs faster than a more robust parser. 
//
// Programmer:      Forrest Miller
// Dates:           10/23/2018      -- Got a rough version together
//                  11/28/2018      -- Performed heavy revision to get this RenderDemo up to snuff with the others.
//                                     Also rewrote all of this RenderDemo's shaders.


#pragma once


#ifndef	ASSET_LOADING_DEMO_H_
#define ASSET_LOADING_DEMO_H_

#include <thread>
#include <cmath> 
#include <functional>   //std::function

#include "GlobalIncludes.h"
#include "GlobalConstants.h"
//#include "RelativeFilepathsToResources.h"
#include "MathFunctions.h"
#include "MeshFunctions.h"
#include "GLFW_Init.h"
#include "ShaderProgram.h"
#include "FramePerformanceTimepointsList.h"

#include "RenderDemoBase.h"
#include "QuickObj.h" //For loading '.obj' files


using ParsedModelData_Iter = std::vector<std::unique_ptr<QuickObj>>::iterator;
using ParsedModelData_CIter = std::vector<std::unique_ptr<QuickObj>>::const_iterator;



////////////////////////////////////////////////////////////////////////////////////////////////  
//////    AssetLoadingDemo Settings   [Modify These Values to Affect Compiled Behavior]   ////// 
////////////////////////////////////////////////////////////////////////////////////////////////

//The following variable dictates whether assets missing texture coordinates should have random coordinates assigned.
//Setting this variable to false will cause all generated texture coordinates to be the same point 
static constexpr const bool ASSIGN_TEXTURE_COORDS_RANDOMLY = true;

static constexpr const GLsizei STARTING_INSTANCE_COUNT = 5;


//To see what 'FRAMES_TO_WAIT_BETWEEN_INPUT_READS' does, set it to 1ull and then run the
//RenderDemo and try to change settings  [unfortunately this constant has not yet been 
//universally implemented for all controls]
static constexpr const uint64_t FRAMES_TO_WAIT_BETWEEN_INPUT_READS = 15;



//////////////////////////////////////////////////////
//////    Global Invariants  [Do Not Modify]    //////
//////////////////////////////////////////////////////

static constexpr const uint64_t FRAMES_TO_WAIT_BEFORE_CHECKING_TO_UPDATE_SHADERS = 60;
static constexpr const GLsizei NUM_VERTEX_COMPONENTS = 9;
static constexpr const GLsizei TRIANGLE_SIDES_AMOUNTAGE = 3;



class AssetLoadingDemo : public RenderDemoBase { 
public:
    AssetLoadingDemo() = delete;
    AssetLoadingDemo(InitReport*);
    virtual ~AssetLoadingDemo() noexcept override;// = default;

    void loadAssets() override; //By declaring these functions 'override', the usage 
    void run() override;        //of the keyword 'virtual' becomes superfluous
    
    
    
    FramePerformanceTimepointsList framePerformance;


protected: //private:
    bool error;
    //float counter;  //Counter was moved to be a member of the base class 'RenderDemoBase'
    float timeTickRateModifier;
    uint32_t frameCounter;
    uint32_t frameUnpaused, frameLineTypeLastSwitched, frameInstancedDrawingBehaviorLastToggled,
        frameInstancedDrawingCountLastModified, frameTimeFreezeLastToggled, frameBlendOperationLastToggled,
        frameDepthClampLastToggled, frameThatTimePropogationWasLastReversed, 
        frameThatCustomShaderParameter1LastModified, frameThatCustomShaderParameter2LastModified,
        frameThatCustomShaderParameter3LastModified;
    mutable uint32_t framePerformanceReportingLastToggled;

    glm::vec3 backgroundColor;

    GLuint vao; //Only 1 VertexArrayObject is required because all buffers share a common data layout.
    GLuint triangleOutlineEBO;
    GLuint sceneBufferVBO;//primarySceneBufferVBO, alternateSceneBufferVBO; 
    GLuint practiceTexture;
    std::vector<GLuint> practiceTextureSet;

    //Variables that are modifiable by input from the user
    PIPELINE_PRIMITIVE_INPUT_TYPE currentPrimitiveInputType;
    bool drawMultipleInstances; //For trying out glDrawArraysInstanced() vs plain old glDrawArrays();
    GLsizei instanceCount;
    
    bool reportPerformance;
    bool freezeTimeToggle; 
    bool reverseTimePropogation;
    bool enableBlending;
    mutable bool enableDepthClamping;

    //These next 3 variables allow for various configurations to be alternated 
    //between within a ShaderProgram while the Application is running. It is up to
    //the shader program to give these values meaning. 
    GLuint customShaderParameter1, customShaderParameter2, customShaderParameter3;
    //The 3 preceding variables are associated with the following 3 uniform values
    //in the shader code
    const GLchar* CUSTOM_SHADER_PARAMETER_1_UNIFORM_NAME = "customParameter1";
    const GLchar* CUSTOM_SHADER_PARAMETER_2_UNIFORM_NAME = "customParameter2";
    const GLchar* CUSTOM_SHADER_PARAMETER_3_UNIFORM_NAME = "customParameter3";


    //Scene Control Variables
    std::unique_ptr<ShaderProgram> sceneShader, quadTextureTestShader;
    std::vector<std::unique_ptr<QuickObj>> sceneObjects;
    std::vector<GLfloat> sceneBuffer;// , alternativeSceneBuffer;
    std::vector<GLuint> triangleOutlineElementOrdering;

    //For dynamic shader recompilation
    struct LiveUpdateEnabledShaderSet {
        FilepathWrapper file;
        bool primary;
        ShaderInterface::ShaderType type;
        LiveUpdateEnabledShaderSet() = delete;
        LiveUpdateEnabledShaderSet(std::string s, bool isPrimary, ShaderInterface::ShaderType type) : file(s), primary(isPrimary), type(type) { ; }
    };
    std::vector<LiveUpdateEnabledShaderSet> shaderSources; //Vector of filepaths to shaders for checking for updates
    std::unique_ptr<ShaderProgram> backupSceneShader; //Used for building a new sceneShader once files are updated
    

    //View parameters
    glm::vec3 cameraPos;
    glm::vec3 lookAtOrgin;
    glm::vec3 upDirection;
    float xTranslation;
    float yTranslation;
    float zTranslation;
    //Projection parameters
    float fov, zNear, zFar;   //see https://glm.g-truc.net/0.9.2/api/a00245.html
    //Shader Uniforms
    glm::mat4 rotation;
    glm::mat4 view;
    glm::mat4 perspective; //Computes from Projection parameters
    float head, pitch, roll;
    float zoom;
    


    ///////////////////////////////////////////////////////
    /////////////      Setup Functions      ///////////////
    ///////////////////////////////////////////////////////
    
    void initialize(); //Called by constructor(s)

    //Sets the initial OpenGL Context state to the default 
    //state expected by this RenderDemo upon first entering its
    //render loop (However, this function does not explicitly  
    //set its state; instead it relies quite a bit on the 
    //Application having set a number of specific state variables*
    //while it was creating the window and loading the OpenGL function pointers) 
    //[If in the future it is decided to use the GPU to generate assets,
    //this function will need to be called after that has taken place]
    void setAssetLoadingDemoSpecificGlobalGLContextState() const;

    //Loads all required GLSL shader file assets, compiles them and
    //links them into ShaderProgram(s). Because valid shaders are 
    //necessary for any rendering to occur, this function returns a
    //bool. Will return 'true' if all shaders compiled and linked 
    //without an issue. Otherwise the returned value will be false
    bool loadShaders(); //Sets up the sceneShader

    //Loads all of the mesh data. Since this data is (surprisingly) not
    //vital for rendering, it is possible for this function to fail
    //completely and the RenderDemo will still be able to enter its
    //render loop and run as normal (baring any uncaught exceptions)
    void loadModels(); //Loads 3D model data from asset files
    
    //This function is meant to be called after the sceneShader is linked and 
    //all models for the scene have finished loading
    void prepareScene();





    //  /////////////////////////////////////////////////////////////////////////////////  //
    //                /////////////////////////////////////////////////////                //
    //                ///////////      The Render Loop      ///////////////                //
    //                /////////////////////////////////////////////////////                //
    //  /////////////////////////////////////////////////////////////////////////////////  //
    void renderLoop();
    //       ----------------------------------------------------------------------        //
    //        \             ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                /         //
    //         \           ~~~~  RENDER LOOP EXECUTION CYCLE  ~~~~              /          //
    //          \           ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~              /           //
    //          The Render Loop Will Always* Be In One of Four Phases Of Execution         //
    //            While The Application Is Running:                                        //
    //                                               - INPUT                               //
    //                                               - LOGIC                               //
    //                                               - DRAW                                //
    //                                               - PRESENT                             //
    //           *[ For DEBUG builds, the Render Loop will briefly reach several           //
    //              places within each cycle where the code executing exists               //
    //              outside any specific phase of execution. This code's purpose is        //
    //              to record certain event timestamps at various moments in the loop      //
    //              which allow for run-time performance data to be profiled. These        //
    //              event timestamps are recorded using the 'Optick Profiler-Tag           //
    //              Insertion' Functions found below ]                                     //

    //                                                                                     //
    //                                                                                     //
    //                                                                                     //
    //                Each Phase Is Contained Within A Single Function Call                //
    //                                                                                     //
    //                   Here Are The Functions To Call For Each Phase                     //
    //                                                                                     //
    //        ---------------------------------------------------------------------        //
    //        * * * *                  +---------------+                    * * * *        //
    //        * * * *                  |  INPUT PHASE  |                    * * * *        //
    //        * * * *                  +---------------+                    * * * *        //
    //        ---------------------------------------------------------------------        //
    void detectInput();
    //        ---------------------------------------------------------------------        //
    //        * * * *                  +---------------+                    * * * *        //
    //        * * * *                  |  LOGIC PHASE  |                    * * * *        //
    //        * * * *                  +---------------+                    * * * *        //
    //        ---------------------------------------------------------------------        //
    void computeLogic();
    //        ---------------------------------------------------------------------        //
    //        * * * *                  +--------------+                     * * * *        //
    //        * * * *                  |  DRAW PHASE  |                     * * * *        //
    //        * * * *                  +--------------+                     * * * *        //
    //        ---------------------------------------------------------------------        //
    void renderScene();
    //        ---------------------------------------------------------------------        //
    //        * * * *                  +---------------+                    * * * *        //
    //        * * * *                  | PRESENT PHASE |                    * * * *        //
    //        * * * *                  +---------------+                    * * * *        //
    //        ---------------------------------------------------------------------        //
    void presentFrame();
    //                                                                                     //
    //                          (Optick Profiler-Tag Insertion Functions)                  //
    void recordFrameStartTimepoint();
    void recordBeginRenderCommandsTimepoint();
    void recordSwapFramebuffersTimepoint();
    //    -----------------------------------------------------------------------------    //
    //===}-----------------------------------------------------------------------------{===//

















    //                                       IMPLEMENTATION


    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /*}~~                               +~~~~~~~~~~~~~~~~~~~~~~~~~~~+                                 ~~{*/
    /*}~~                               |  (1)  Input Detection     |                                 ~~{*/
    /*}~~                               +~~~~~~~~~~~~~~~~~~~~~~~~~~~+                                 ~~{*/
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
                               

    //The high level function 'detectInput()' breaks input detection into two partitions
    bool checkKeyboardInput();
    void checkControllerInput();

    //These are then all the individual input detection functions
    //   ***\______+============+______/***
    //       ______|  KEYBOARD  |______
    //   ***/      +============+      \***
    bool checkToSeeIfShouldCloseWindow() const noexcept; //check 'esc'
    bool checkIfShouldTogglePerformanceReporting() const noexcept;
    bool checkIfShouldPause() const noexcept; //Probably 'space'
    bool checkIfShouldReset() const noexcept;
    bool checkIfShouldFreezeTime() const noexcept;
    bool checkIfShouldReverseDirectionOfTime() const noexcept;
    bool checkIfShouldIncreasePassageOfTime() const noexcept;
    bool checkIfShouldDecreasePassageOfTime() const noexcept;
    bool checkIfShouldToggleBlending() const noexcept;
    bool checkIfShouldToggleDepthClamping() const noexcept;
    bool checkIfShouldUpdateFieldOfView() const noexcept;
    
    bool checkIfShouldIncreaseCustomShaderParameter1() const noexcept;
    bool checkIfShouldIncreaseCustomShaderParameter2() const noexcept;
    bool checkIfShouldIncreaseCustomShaderParameter3() const noexcept;
    //bool checkIfShouldResetCustomShaderParameter1() const noexcept;
    //bool checkIfShouldResetCustomShaderParameter2() const noexcept;
    //bool checkIfShouldResetCustomShaderParameter3() const noexcept;
    bool checkIfShouldResetCustomShaderParameters() const noexcept;

    //   ***\______+============+______/***
    //       ______|  JOYSTICK  |______
    //   ***/      +============+      \***

    void readJoystick0State_AssumingXInput_AndThenProcessAllInput();
    void readNonStandardJoystick();

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /*}~~                               +~~~~~~~~~~~~~~~~~~~~~~~~~~~+                                 ~~{*/
    /*}~~                               |  (2)  Input Processing    |                                 ~~{*/
    /*}~~                               +~~~~~~~~~~~~~~~~~~~~~~~~~~~+                                 ~~{*/
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    void pause();
    void reset() noexcept;
    void toggleTimeFreeze() noexcept;
    void reverseTime() noexcept;
    void increasePassageOfTime() noexcept;
    void decreasePassageToTime() noexcept;
    void toggleBlending() noexcept;
    void toggleDepthClamping() noexcept;
    void updateFieldOfView() noexcept;
    void recomputeProjectionMatrix() noexcept;  
    void changePrimitiveType();
    void changeInstancedDrawingBehavior() noexcept;
    void increaseCustomShaderParameter1() noexcept;
    void increaseCustomShaderParameter2() noexcept;
    void increaseCustomShaderParameter3() noexcept;
    void resetCustomShaderParameter1() noexcept;
    void resetCustomShaderParameter2() noexcept;
    void resetCustomShaderParameter3() noexcept;
    void rotate() noexcept;
    void changeZoom() noexcept;
    void translate() noexcept;
   
    
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /*}~~                               +~~~~~~~~~~~~~~~~~~~~~~~~~~~+                                 ~~{*/
    /*}~~                               |   (3)   Handle Events     |                                 ~~{*/
    /*}~~                               +~~~~~~~~~~~~~~~~~~~~~~~~~~~+                                 ~~{*/
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    //These functions are specific event-handling tasks that can
    //be called as part of 'computeLogic()' 

    bool checkForUpdatedShaders();
    void buildNewShader();
    void reportStatistics() ;
    void propagateTime();
    void updateTaggedVariablesWithOptick() const;
    
    
    
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /*}~~                                +~~~~~~~~~~~~~~~~~~~~~~~~~+                                   ~~{*/
    /*}~~                                |   (4)   Render Scene    |                                   ~~{*/
    /*}~~                                +~~~~~~~~~~~~~~~~~~~~~~~~~+                                   ~~{*/
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    
    /////////////////////////////////////////
    ///  (4-1)  Background Color Update   ///
    /////////////////////////////////////////
    
    void updateFrameClearColor();
    
    
    ///////////////////////////////////
    ///  (4-2a)  Update Uniforms    ///
    ///////////////////////////////////
    
    void updateBaseUniforms() noexcept;
    
    
    //////////////////////////////////
    ///  (4-2b)  Update Buffers    ///
    //////////////////////////////////
    
    /*        Maybe one day         */
    
    
    //////////////////////////////////
    ///  (4-2c)  Make Draw Call    ///                           
    ////////////////////////////////// 
    virtual void drawVerts();                  
                                      
    // [REPEAT Step 4-2 (parts a-c) for each draw call]  
    
    
    /*                    +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
                          |    (5) Clean-up and Prepare for Next Frame    |
                          +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                    */

    void prepareGLContextForNextFrame() noexcept;




    /////////////////////////////////////////////////////////////////////////////////////////////
    ////  The following are utility functions called by the setup and render-loop functions  ////
    /////////////////////////////////////////////////////////////////////////////////////////////

    bool buildQuadTextureTestShader();
    bool loadTexture2DFromImageFile();

    //This function expects each vertex in the passed-in sceneBuffer to be exactly
    //9 components.
    GLsizei computeNumberOfVerticesInSceneBuffer(const std::vector<GLfloat>&) const noexcept;

    void generateTriangleOutlineElementOrdering() noexcept;

    //NOT_IMPLEMENTED:
    //glm::vec3 computePositionOffsetForNextObject(const glm::vec3& posOffset = glm::vec3(0.0f, 0.0f, 0.0f)) const noexcept;

    //Prints out the name of the currently active primitive type 
    void printNameOfTheCurrentlyActivePrimitive() const;

    //Checks to see if either left or right shift (or both) are pressed and
    //uses the results to compute a 'boost' value. This is used in some 
    //of the input logic to allow for faster input-response rates.
    float getShiftBoost() const noexcept;

    //Sets up the sceneBuffer. Will fill in missing uvTexCoord and Normal vertex components
    //as needed to give every vertex in the sceneBuffer the same format. 
    void buildSceneBufferFromLoadedSceneObjects();

    //The following 4 functions are intended for use within the function
    //buildSceneBufferFromLoadedSceneObjects() to handle 
    //data into the sceneBuffer
    void addObject(std::vector<std::unique_ptr<QuickObj>>::const_iterator object,
        const glm::vec3& objPos);

    //Helper function for generating random texture coordinates
    static inline glm::vec2 generateRandomTexCoords() {
        return glm::vec2(MathFunc::getRandomInRangef(0.0f, 1.0f), MathFunc::getRandomInRangef(0.0f, 1.0f));
    }
    //Helper function for generating constant texture coords
    static inline glm::vec2 generateConstantTexCoords() noexcept {
        return glm::vec2(0.5f, 0.5f);
    }

    void createSceneVBO() noexcept;
    void createTriangleOutlineEBO() noexcept;

    //Does exactly what you think [creates buffers within the GL Context, uploads vertices to buffer]
    void uploadSceneBufferToGPU(GLuint& targetVBO, const std::vector<float>& sceneBuf) noexcept;
    void uploadTriangleOutlineElementOrderingBufferToGPU(GLuint& ebo, const std::vector<GLuint>& eboData) noexcept;
    //Sets up the VAO [which describes how the vertex data is arranged in the VertexArrayBuffer]
    void configureVertexArrayAttributes() noexcept; 


};


#endif //ASSET_LOADING_DEMO_H_
