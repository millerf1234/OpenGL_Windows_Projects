//File:    ShaderProgram.h
//Class:   ShaderProgram
//Programmer: Forrest Miller
//
//  Description:      This started out as a rewrite of ShaderProgram, but it turns out that 
//                    doing that will be a lot of work. Instead, this class is implemented as 
//                    a Shim ontop of an underlying ShaderProgram1 object. 
//
// Dates:           
//         Original ShaderProgram class:            7/24/18-8/1/18  
//         ShaderProgram 2.0 class [This]:          June 15, 2019


// ADDING FULL DOCUMENTATION TO THIS FILE HAS BEEN POSTPONED UNTIL FUTURE TIMES

#pragma once

#ifdef SHADER_PROGRAM_2_H_
#define SHADER_PROGRAM_2_H_

#include <filesystem>
#include <string_view>

#include "ShaderProgram.h"


class ShaderProgram2 final {
public:

    //-------------------------------
    // Construction/Destruction/Copying/Moving
    //-------------------------------

    ShaderProgram2();
    ~ShaderProgram2() noexcept;

    //Allow moving but disallow copying
    ShaderProgram2(const ShaderProgram2&) = delete;
    ShaderProgram2(ShaderProgram2&&) noexcept;
    ShaderProgram2& operator=(const ShaderProgram2&) = delete;
    ShaderProgram2& operator=(ShaderProgram2&&) noexcept;



    //-------------------------------
    // Public Interface Functions
    //-------------------------------

    //Creates an object-local Vertex shader and attaches it to this program
    bool attachVert(std::string_view) noexcept;
    //Creates an object-local Vertex shader from specified file and attaches it to this program
    bool attachVert(std::filesystem::path) noexcept;
    //Creates an object-local secondary Vertex shader from the specified file and attaches it to
    //this program. A secondary vertex shader can be any legal GLSL Vertex shader code that does NOT
    //contain a 'main()' function.
    void attachSecondaryVert(std::string_view) noexcept;
    //Creates an object-local secondary Vertex shader from the specified file and attaches it to
    //this program. A secondary vertex shader can be any legal GLSL Vertex shader code that does NOT
    //contain a 'main()' function.
    void attachSecondaryVert(std::filesystem::path) noexcept;


    //Creates an object-local Geom shader and attaches it to this program
    bool attachGeom(std::string_view) noexcept;
    //Creates an object-local Geom shader from specified file and attaches it to this program
    bool attachGeom(std::filesystem::path) noexcept;
    //Creates an object-local secondary Geometry shader from the specified file and attaches it to
    //this program. A secondary Geometry shader can be any legal GLSL Geometry shader code that does NOT
    //contain a 'main()' function.
    void attachSecondaryGeom(std::string_view) noexcept;
    //Creates an object-local secondary Geometry shader from the specified file and attaches it to
    //this program. A secondary Geometry shader can be any legal GLSL Geometry shader code that does NOT
    //contain a 'main()' function.
    void attachSecondaryGeom(std::filesystem::path) noexcept;


    //Creates an object-local Tesse shader and attaches it to this program
    bool attachTesse(std::string_view) noexcept;
    //Creates an object-local Tesse shader from specified file and attaches it to this program
    bool attachTesse(std::filesystem::path) noexcept;
    //Creates an object-local secondary Tesse shader from the specified file and attaches it to
    //this program. A secondary Tesse shader can be any legal GLSL Tesse shader code that does NOT
    //contain a 'main()' function.
    void attachSecondaryTesse(std::string_view) noexcept;
    //Creates an object-local secondary Tesse shader from the specified file and attaches it to
    //this program. A secondary Tesse shader can be any legal GLSL Tesse shader code that does NOT
    //contain a 'main()' function.
    void attachSecondaryTesse(std::filesystem::path) noexcept;



    //Creates an object-local Tessc shader and attaches it to this program
    bool attachTessc(std::string_view) noexcept;
    //Creates an object-local Tessc shader from specified file and attaches it to this program
    bool attachTessc(std::filesystem::path) noexcept;
    //Creates an object-local secondary Tessc shader from the specified file and attaches it to
    //this program. A secondary Tessc shader can be any legal GLSL Tessc shader code that does NOT
    //contain a 'main()' function.
    void attachSecondaryTessc(std::string_view) noexcept;
    //Creates an object-local secondary Tessc shader from the specified file and attaches it to
    //this program. A secondary Tessc shader can be any legal GLSL Tessc shader code that does NOT
    //contain a 'main()' function.
    void attachSecondaryTessc(std::filesystem::path) noexcept;



    //Creates an object-local Fragment shader and attaches it to this program
    bool attachFrag(std::string_view) noexcept;
    //Creates an object-local Fragment shader from specified file and attaches it to this program
    bool attachFrag(std::filesystem::path) noexcept;
    //Creates an object-local secondary Fragment shader from the specified file and attaches it to
    //this program. A secondary Fragment shader can be any legal GLSL Fragment shader code that does NOT
    //contain a 'main()' function.
    void attachSecondaryFrag(std::string_view) noexcept;
    //Creates an object-local secondary Fragment shader from the specified file and attaches it to
   //this program. A secondary Fragment shader can be any legal GLSL Fragment shader code that does NOT
   //contain a 'main()' function.
    void attachSecondaryFrag(std::filesystem::path) noexcept;


    //Creates an object-local Compute shader and attaches it to this program.  This function should
    //only be called if no other Shaders have been attached to this object, and after calling
    //this function [no additional shaders may be attached / only additional compute shaders
    //may be attached] (I can't remember which behavior I implemented. Test this out and delete this 
    //comment).
    bool attachCompute(std::string_view) noexcept;
#if 0
    //Creates an object-local Fragment shader from specified file and attaches it to this program
    bool attachFrag(std::filesystem::path) noexcept;
    //Creates an object-local secondary Fragment shader from the specified file and attaches it to
    //this program. A secondary Fragment shader can be any legal GLSL Fragment shader code that does NOT
    //contain a 'main()' function.
    void attachSecondaryFrag(std::string_view) noexcept;
    //Creates an object-local secondary Fragment shader from the specified file and attaches it to
   //this program. A secondary Fragment shader can be any legal GLSL Fragment shader code that does NOT
   //contain a 'main()' function.
    void attachSecondaryFrag(std::filesystem::path) noexcept;

    //Creates an object-local Compute shader and attaches it to this program. This function should
    //only be called in no other Shaders have been attached to this object, and after calling
    //this function no additional shaders may be attached.
    inline bool attachCompute(std::string compute) { return (attachCompute(compute.c_str())); }
    //Creates an object-local Compute shader and attaches it to this program. This function should
    //only be called in no other Shaders have been attached to this object, and after calling
    //this function no additional shaders may be attached.
    bool attachCompute(const char* compute);
    //Uses an already-created Compute shader and attaches it to this program. This function should
    //only be called in no other Shaders have been attached to this object, and after calling
    //this function no additional shaders may be attached.
    void attachCompute(const ShaderInterface::ComputeShader* compute);
    //Uses an already created Compute shader that does not contain a 'main' function and has
    //been marked as secondary using the 'makeSecondary()' function and attaches it to this
    //ShaderProgram. Compute shaders have the additional requirement that they must be the only type of
    //shader attached to any ShaderProgram. Attaching a compute shader to a program
    //that already has shaders of another type attached is an error, as is trying to attach shaders
    //of a different type to a program that already has compute shaders attached.
    //It is possible to attach as many secondaries as are available, however it is an
    //error to try to attach the same secondary shader multiple times to one program. 
    void attachSecondaryCompute(const ShaderInterface::ComputeShader* compute);
#endif //#if 0

    //Checks to see if this ShaderProgram is linked
    bool checkIfLinked() const noexcept { return internalShaderProgramObj->checkIfLinked(); }

    //Links this shader program object. This shader program must have either solely a Compute
    //shader or both a Vertex and Fragment shader attached before linking. Additionally,
    //using the tessellation shaders requires both a tessellation control and tessellation 
    //evaluation shader to be attached.
    void link();

    //Binds this ShaderProgram to the active program spot in the GL Context. This ShaderProgram
    //must be linked before this function call will work correctly.
    void use() const;


 
    std::unique_ptr<ShaderProgram> internalShaderProgramObj;
};



#endif //SHADER_PROGRAM_2_H_

