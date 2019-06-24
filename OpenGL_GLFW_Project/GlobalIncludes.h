//This file contains headers that are meant to be included globally. 
//
//
//  File structure:     There are 2 sets of includes contained within this header.
//                       -) The first set contains headers that control universal 
//                            settings which determine the behavior of the 
//                            code across the entire project.  
//                       -) The second set consists of the headers of several 
//                            fundamental 3rd party libraries that are used 
//                            throughout the codebase.  
//                        
//                       Please be aware that the second set of includes have 
//                           dependencies on settings/macros specified by 
//                           headers in the first set
//
//
//   File History:
//             - Created by Forrest Miller on July 16, 2018
//             - Reorganized on November 8, 2018 
//                   Removed several includes that could have their scope reduced
//             - Renamed on January 4, 2019 to "GlobalIncludes.h"
//                   Previous name was "ProjectSetup.h" 
//                   Change was made to avoid confusion with code which pertains specifically
//                       to RenderProjects
//             - Replaced GLAD with newer version

#pragma once
 
#ifndef GLOBAL_INCLUDES_H_
#define GLOBAL_INCLUDES_H_


//////////////////////////////////////////////////////////////////////////////////////////////// 
//1st set//////////////////            Global Configuration Headers            ///////////////// 
//////////////////////////////////////////////////////////////////////////////////////////////// 

///////////////////////////////////////////
///////  Project-Wide Log Message Output
///////////////////////////////////////////
#include "LoggingMessageTargets.h"

///////////////////////////////////////////
///////  Project-Wide Debug Settings   
///////////////////////////////////////////
#include "DebugSettings.h"


///////////////////////////////////////////
///////  Project-Wide Timepoint Objects  [Must Have GLFW Initialized To Use]
///////////////////////////////////////////
#include "Timepoint.h"

/////////////////////////////////////////////////////////////////////////////////////////
//2nd set//////////////////       Headers for 3rd Party Libraries       /////////////////
/////////////////////////////////////////////////////////////////////////////////////////  


///////////////////////////////////////////////
//  Choose which GLAD library version to use //
///////////////////////////////////////////////

//GLAD has both a Debug version and a standard version. Which version is to be
//included is determined from whether the project is in debug mode or not. 
//    (Side Note: Both versions of glad depend on the header file 'khrplatform.h'.
//                This header file is identical for both versions) 
#ifdef USE_DEBUG_  
//DEBUG GLAD version (OpenGL 4.6) 
//       link:     http://glad.dav1d.de/#profile=core&api=gl%3D4.6&api=gles1%3Dnone&api=gles2%3Dnone&api=glsc2%3Dnone&extensions=GL_EXT_convolution&extensions=GL_EXT_framebuffer_blit&extensions=GL_EXT_framebuffer_multisample&extensions=GL_EXT_framebuffer_multisample_blit_scaled&extensions=GL_EXT_framebuffer_object&extensions=GL_EXT_framebuffer_sRGB&extensions=GL_EXT_multisample&extensions=GL_EXT_polygon_offset&extensions=GL_EXT_provoking_vertex&extensions=GL_EXT_shadow_funcs&extensions=GL_EXT_texture&extensions=GL_EXT_texture_buffer_object&extensions=GL_EXT_texture_compression_latc&extensions=GL_EXT_texture_compression_rgtc&extensions=GL_EXT_texture_compression_s3tc&extensions=GL_EXT_texture_cube_map&extensions=GL_EXT_texture_filter_anisotropic&extensions=GL_EXT_texture_object&extensions=GL_EXT_texture_sRGB&extensions=GL_EXT_texture_swizzle&language=c-debug&specification=gl&localfiles=on&loader=on
#include "glad/debug/debug_glad.h"
#else
//NON-DEBUG GLAD version (OpenGL 4.6) 
//       link: http://glad.dav1d.de/#profile=core&api=gl%3D4.6&api=gles1%3Dnone&api=gles2%3Dnone&api=glsc2%3Dnone&extensions=GL_EXT_convolution&extensions=GL_EXT_framebuffer_blit&extensions=GL_EXT_framebuffer_multisample&extensions=GL_EXT_framebuffer_multisample_blit_scaled&extensions=GL_EXT_framebuffer_object&extensions=GL_EXT_framebuffer_sRGB&extensions=GL_EXT_multisample&extensions=GL_EXT_polygon_offset&extensions=GL_EXT_provoking_vertex&extensions=GL_EXT_shadow_funcs&extensions=GL_EXT_texture&extensions=GL_EXT_texture_buffer_object&extensions=GL_EXT_texture_compression_latc&extensions=GL_EXT_texture_compression_rgtc&extensions=GL_EXT_texture_compression_s3tc&extensions=GL_EXT_texture_cube_map&extensions=GL_EXT_texture_filter_anisotropic&extensions=GL_EXT_texture_object&extensions=GL_EXT_texture_sRGB&extensions=GL_EXT_texture_swizzle&language=c&specification=gl&loader=on
#include "glad/release/glad.h" //This one header file handles loading the entire graphics language. I am 'glad' it exists (lol)
#endif 


//////////////////////////////////////////////////////////////////
////  Include the 'Graphics Language Framework' (GLFW) library  //
//////////////////////////////////////////////////////////////////
////#include "glfw_config.h"  //This is the file that was generated by cmake when GLFW was compiled, 
////                          //however it is not necessary for it to be included in this project.
#include "glfw3.h"
//Trouble Linking? Is your glfw3.dll the 32-bit version? 


//////////////////////////////
//  Set up the GLM library  //         (GLM is mathematics utility library)
//////////////////////////////
#ifndef GLM_FORCE_SSE2
#define GLM_FORCE_SSE2
#endif //GLM_FORCE_SSE2

#ifndef GLM_FORCE_ALIGNED
#define GLM_FORCE_ALIGNED
#endif //GLM_FORCE_ALIGNED

//Include the main GLM library
#include "glm/glm.hpp"  

//After including the main GLM header, it is still necessary to include
//some additional GLM extensions that are used in this project:

//'type_ptr' is a conversion function that allows GLM vector and GLM 
// matrix types to be accessed as though they are just arrays through
// a GLfloat* to their first element. This is necessary for sending them
// as uniforms to shader programs.
#include "glm/gtc/type_ptr.hpp"   

//'quaternion' is a special data type used to efficiently and cleanly express
// rotations in 3D. They are both cheaper and more robust than relying on Euler angles.
#include "glm/gtc/quaternion.hpp" 

//'matrix_transform' provides functions for creating several useful matrices that 
// are used within the course of rendering. 
#include "glm/gtc/matrix_transform.hpp"





//////////////////////////////////////////////////////////////////////////
//////    These following Includes are not necessarily global but the engine components that will
//////      use them have yet to be implemented. These will be relocated in future
//////////////////////////////////////////////////////////////////////////

//Freetype will be useful for when I get around to implementing a kick-butt glyph-based text
//rendering system (or integrate one of the many available Open-Source libraries that
// provide this functionality).
//#include "ft2build.h"


//Move this to .cpp file?
//#ifndef STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"
//#endif 




#endif //GLOBAL_INCLUDES_H_



