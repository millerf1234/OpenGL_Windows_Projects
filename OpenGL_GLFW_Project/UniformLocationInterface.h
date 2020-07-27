//File:                        UniformLocationInterface.h
//Class:                       UniformLocationInterface 
//
//Description:                 
//
//                             This class is designed to act as an intermediary between 
//                             a UniformLocationTracker object and the rest of the Application.
//                             The benefits of this class are that it can be constructed into a 
//                             valid state without all the required information available that 
//                             a raw UniformLocationTracker requires [namely, a valid ShaderProgram ID].
//                             
//                             The main idea driving this class is that it provides safety for both:
//                                  a)  the UniformLocationTracker by restricting where its function calls
//                                        can be made while also ensuring that no improper requests get 
//                                        made, and
//                                  b)  the rest of the application by providing a layer of insurance that
//                                        requests to update uniform locations are being made to a valid
//                                        tracker.
//
//Usage Pattern:               i) An instance of this class is constructed along with an OpenGL ShaderProgram. 
//                                   At this point no UniformLocationTracker object exists and any requests 
//                                   (i.e. function calls) made regarding uniforms result in a warning 
//                                    message.
//                            ii) This class's function to activate a UniformLocationTracker is called with 
//                                   a valid ProgramID value provided. That causes this class to 'activate', 
//                                   which allows for uniform requests to go on through to the UniformLocationManager.
//        
//
//                           
//Programmer:                  Forrest Miller
//Date:                        December 4, 2018
//


#pragma once

#ifndef UNIFORM_LOCATION_INTERFACE_H_
#define UNIFORM_LOCATION_INTERFACE_H_

#include <functional>

#include "UniformLocationTracker.h"

//using ShaderInterface::UniformLocationTracker;
namespace ShaderInterface {

    class UniformLocationInterface final {
    public:
        UniformLocationInterface();
        UniformLocationInterface(const GLuint programID);
        ~UniformLocationInterface() noexcept;
        UniformLocationInterface(const UniformLocationInterface&) = delete;
        UniformLocationInterface(UniformLocationInterface&&) noexcept;
        UniformLocationInterface& operator=(const UniformLocationInterface&) = delete;
        UniformLocationInterface& operator=(UniformLocationInterface&&) noexcept;

        //This function puts this object into an 'activated' state which allows for all of the
        //uniform-related functions to be passed on to the UniformLocationTracker 
        void activateUniformLocationTracker(const GLuint programID);

        //Update uniform Functions
        //see: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glUniform.xhtml

        void updateUniform1i(const GLchar * uniformName, const GLint x) { fptr_UpdateUniform1i_(uniformName, x); }
        void updateUniform1u(const GLchar * uniformName, const GLuint x) { fptr_UpdateUniform1u_(uniformName, x); }
        void updateUniform1f(const GLchar * uniformName, const GLfloat x) { fptr_UpdateUniform1f_(uniformName, x); }
        void updateUniform1iv(const GLchar * uniformName, const GLint * ivec, GLsizei count = 1) { fptr_UpdateUniform1iv_(uniformName, ivec, count); }
        void updateUniform1uv(const GLchar * uniformName, const GLuint * uvec, GLsizei count = 1) { fptr_UpdateUniform1uv_(uniformName, uvec, count); }
        void updateUniform1fv(const GLchar * uniformName, const GLfloat * fvec, GLsizei count = 1) { fptr_UpdateUniform1fv_(uniformName, fvec, count); }

        void updateUniform2i(const GLchar * uniformName, const GLint x, const GLint y) { fptr_UpdateUniform2i_(uniformName, x, y); }
        void updateUniform2u(const GLchar * uniformName, const GLuint x, const GLuint y) { fptr_UpdateUniform2u_(uniformName, x, y); }
        void updateUniform2f(const GLchar * uniformName, const GLfloat x, const GLfloat y) { fptr_UpdateUniform2f_(uniformName, x, y); }
        void updateUniform2iv(const GLchar * uniformName, const GLint * ivec, GLsizei count = 1) { fptr_UpdateUniform2iv_(uniformName, ivec, count); }
        void updateUniform2uv(const GLchar * uniformName, const GLuint * uvec, GLsizei count = 1) { fptr_UpdateUniform2uv_(uniformName, uvec, count); }
        void updateUniform2fv(const GLchar * uniformName, const GLfloat * fvec, GLsizei count = 1) { fptr_UpdateUniform2fv_(uniformName, fvec, count); }

        void updateUniform3i(const GLchar * uniformName, const GLint x, const GLint y, const GLint z) { fptr_UpdateUniform3i_(uniformName, x, y, z); }
        void updateUniform3u(const GLchar * uniformName, const GLuint x, const GLuint y, const GLuint z) { fptr_UpdateUniform3u_(uniformName, x, y, z); }
        void updateUniform3f(const GLchar * uniformName, const GLfloat x, const GLfloat y, const GLfloat z) { fptr_UpdateUniform3f_(uniformName, x, y, z); }
        void updateUniform3iv(const GLchar * uniformName, const GLint * ivec, GLsizei count = 1) { fptr_UpdateUniform3iv_(uniformName, ivec, count); }
        void updateUniform3uv(const GLchar * uniformName, const GLuint * uvec, GLsizei count = 1) { fptr_UpdateUniform3uv_(uniformName, uvec, count); }
        void updateUniform3fv(const GLchar * uniformName, const GLfloat * fvec, GLsizei count = 1) { fptr_UpdateUniform3fv_(uniformName, fvec, count); }

        void updateUniform4i(const GLchar * uniformName, const GLint x, const GLint y, const GLint z, const GLint w) { fptr_UpdateUniform4i_(uniformName, x, y, z, w); }
        void updateUniform4u(const GLchar * uniformName, const GLuint x, const GLuint y, const GLuint z, const GLuint w) { fptr_UpdateUniform4u_(uniformName, x, y, z, w); }
        void updateUniform4f(const GLchar * uniformName, const GLfloat x, const GLfloat y, const GLfloat z, const GLfloat w) { fptr_UpdateUniform4f_(uniformName, x, y, z, w); }
        void updateUniform4iv(const GLchar * uniformName, const GLint * ivec, GLsizei count = 1) { fptr_UpdateUniform4iv_(uniformName, ivec, count); }
        void updateUniform4uv(const GLchar * uniformName, const GLuint * uvec, GLsizei count = 1) { fptr_UpdateUniform4uv_(uniformName, uvec, count); }
        void updateUniform4fv(const GLchar * uniformName, const GLfloat * fvec, GLsizei count = 1) { fptr_UpdateUniform4fv_(uniformName, fvec, count); }

        //For the matrices, it is possible to use either a GLM::mat of the
        //corresponding type or to just provide a GLfloat * representing the
        //data. Count can be used to update an array of matrices.

        void updateUniformMat2(const GLchar * uniformName, const glm::mat2 * matrix, GLsizei count = 1) { fptr_UpdateUniformMat2_(uniformName, matrix, count); }
        void updateUniformMat2(const GLchar * uniformName, const GLfloat * matrix, GLsizei count = 1) { fptr_UpdateUniformMat2Array_(uniformName, matrix, count); }
        void updateUniformMat2x2(const GLchar * uniformName, const glm::mat2 * matrix, GLsizei count = 1) { fptr_UpdateUniformMat2_(uniformName, matrix, count); }
        void updateUniformMat2x2(const GLchar * uniformName, const GLfloat * matrix, GLsizei count = 1) { fptr_UpdateUniformMat2Array_(uniformName, matrix, count); }
        void updateUniformMat2x3(const GLchar * uniformName, const glm::mat2x3 * matrix, GLsizei count = 1) { fptr_UpdateUniformMat2x3_(uniformName, matrix, count); }
        void updateUniformMat2x3(const GLchar * uniformName, const GLfloat * matrix, GLsizei count = 1) { fptr_UpdateUniformMat2x3Array_(uniformName, matrix, count); }
        void updateUniformMat2x4(const GLchar * uniformName, const glm::mat2x4 * matrix, GLsizei count = 1) { fptr_UpdateUniformMat2x4_(uniformName, matrix, count); }
        void updateUniformMat2x4(const GLchar * uniformName, const GLfloat * matrix, GLsizei count = 1) { fptr_UpdateUniformMat2x4Array_(uniformName, matrix, count); }


        void updateUniformMat3x2(const GLchar * uniformName, const glm::mat3x2 * matrix, GLsizei count = 1) { fptr_UpdateUniformMat3x2_(uniformName, matrix, count); }
        void updateUniformMat3x2(const GLchar * uniformName, const GLfloat * matrix, GLsizei count = 1) { fptr_UpdateUniformMat3x2Array_(uniformName, matrix, count); }
        void updateUniformMat3(const GLchar * uniformName, const glm::mat3 * matrix, GLsizei count = 1) { fptr_UpdateUniformMat3_(uniformName, matrix, count); }
        void updateUniformMat3(const GLchar * uniformName, const GLfloat * matrix, GLsizei count = 1) { fptr_UpdateUniformMat3Array_(uniformName, matrix, count); }
        void updateUniformMat3x3(const GLchar * uniformName, const glm::mat3 * matrix, GLsizei count = 1) { fptr_UpdateUniformMat3_(uniformName, matrix, count); }
        void updateUniformMat3x3(const GLchar * uniformName, const GLfloat * matrix, GLsizei count = 1) { fptr_UpdateUniformMat3Array_(uniformName, matrix, count); }
        void updateUniformMat3x4(const GLchar * uniformName, const glm::mat3x4 * matrix, GLsizei count = 1) { fptr_UpdateUniformMat3x4_(uniformName, matrix, count); }
        void updateUniformMat3x4(const GLchar * uniformName, const GLfloat * matrix, GLsizei count = 1) { fptr_UpdateUniformMat3x4Array_(uniformName, matrix, count); }


        void updateUniformMat4x2(const GLchar * uniformName, const glm::mat4x2 * matrix, GLsizei count = 1) { fptr_UpdateUniformMat4x2_(uniformName, matrix, count); }
        void updateUniformMat4x2(const GLchar * uniformName, const GLfloat * matrix, GLsizei count = 1) { fptr_UpdateUniformMat4x2Array_(uniformName, matrix, count); }
        void updateUniformMat4x3(const GLchar * uniformName, const glm::mat4x3 * matrix, GLsizei count = 1) { fptr_UpdateUniformMat4x3_(uniformName, matrix, count); }
        void updateUniformMat4x3(const GLchar * uniformName, const GLfloat * matrix, GLsizei count = 1) { fptr_UpdateUniformMat4x3Array_(uniformName, matrix, count); }
        void updateUniformMat4(const GLchar * uniformName, const glm::mat4 * matrix, GLsizei count = 1) { fptr_UpdateUniformMat4_(uniformName, matrix, count); }
        void updateUniformMat4(const GLchar * uniformName, const GLfloat * matrix, GLsizei count = 1) { fptr_UpdateUniformMat4Array_(uniformName, matrix, count); }
        void updateUniformMat4x4(const GLchar * uniformName, const glm::mat4 * matrix, GLsizei count = 1) { fptr_UpdateUniformMat4_(uniformName, matrix, count); }
        void updateUniformMat4x4(const GLchar * uniformName, const GLfloat * matrix, GLsizei count = 1) { fptr_UpdateUniformMat4Array_(uniformName, matrix, count); }

        //Gets the number of uniform locations that have successfully been updated for this ShaderProgram. 
        size_t getNumberOfTrackedUniforms() const;

        //Returns the GL-assigned program ID that this objects uniform locations are associated with.
        GLuint getProgramID() const;// { return mProgramID; }

    private:
        bool mActivated_;
        std::unique_ptr<UniformLocationTracker> mUniformLocationTracker_;

        //Helper Functions
        void initialize();

        //Intended to be called as part of moving this object. On completion of this function, 
        //this object will have all of its function pointers equal to the "that" object's function
        //pointers upon function entry and "that"'s function pointers will subsequently be deactivated.
        void transferFunctionPointers(UniformLocationInterface & that); 

        void linkFunctionPointersToUniformLocationTracker(); //Used during the activation process

        void linkFunctionPointersToDeactivatedBehaviorFunction();

        //Function to be called when requests are made to this object before it is 'activated'
        static void inactiveUniformTrackingResponse() {
            assert(false);
            //fprintf(WRNLOG, "\nWarning! Unable to process Uniform related request!\n");
        }
        
        //Function Pointers  

        std::function<void(const GLchar *, const GLint)> fptr_UpdateUniform1i_;
        std::function<void(const GLchar *, const GLuint)> fptr_UpdateUniform1u_;
        std::function<void(const GLchar *, const GLfloat)> fptr_UpdateUniform1f_;
        std::function<void(const GLchar *, const GLint *, GLsizei)> fptr_UpdateUniform1iv_;
        std::function<void(const GLchar *, const GLuint *, GLsizei)> fptr_UpdateUniform1uv_;
        std::function<void(const GLchar *, const GLfloat *, GLsizei)> fptr_UpdateUniform1fv_;

        std::function<void(const GLchar *, const GLint, const GLint)> fptr_UpdateUniform2i_;
        std::function<void(const GLchar *, const GLuint, const GLuint)> fptr_UpdateUniform2u_;
        std::function<void(const GLchar *, const GLfloat, const GLfloat)> fptr_UpdateUniform2f_;
        std::function<void(const GLchar *, const GLint *, GLsizei)> fptr_UpdateUniform2iv_;
        std::function<void(const GLchar *, const GLuint *, GLsizei)> fptr_UpdateUniform2uv_;
        std::function<void(const GLchar *, const GLfloat *, GLsizei)> fptr_UpdateUniform2fv_;

        std::function<void(const GLchar *, const GLint, const GLint, const GLint)> fptr_UpdateUniform3i_;
        std::function<void(const GLchar *, const GLuint, const GLuint, const GLuint)> fptr_UpdateUniform3u_;
        std::function<void(const GLchar *, const GLfloat, const GLfloat, const GLfloat)> fptr_UpdateUniform3f_;
        std::function<void(const GLchar *, const GLint *, GLsizei)> fptr_UpdateUniform3iv_;
        std::function<void(const GLchar *, const GLuint *, GLsizei)> fptr_UpdateUniform3uv_;
        std::function<void(const GLchar *, const GLfloat *, GLsizei)> fptr_UpdateUniform3fv_;

        std::function<void(const GLchar *, const GLint, const GLint, const GLint, const GLint)> fptr_UpdateUniform4i_;
        std::function<void(const GLchar *, const GLuint, const GLuint, const GLuint, const GLuint)> fptr_UpdateUniform4u_;
        std::function<void(const GLchar *, const GLfloat, const GLfloat, const GLfloat, const GLfloat)> fptr_UpdateUniform4f_;
        std::function<void(const GLchar *, const GLint *, GLsizei)> fptr_UpdateUniform4iv_;
        std::function<void(const GLchar *, const GLuint *, GLsizei)> fptr_UpdateUniform4uv_;
        std::function<void(const GLchar *, const GLfloat *, GLsizei)> fptr_UpdateUniform4fv_;

        std::function<void(const GLchar *, const glm::mat2 *, GLsizei)>  fptr_UpdateUniformMat2_;
        std::function<void(const GLchar *, const GLfloat *, GLsizei)> fptr_UpdateUniformMat2Array_;
        std::function<void(const GLchar *, const glm::mat2x3 *, GLsizei)>  fptr_UpdateUniformMat2x3_;
        std::function<void(const GLchar *, const GLfloat *, GLsizei)> fptr_UpdateUniformMat2x3Array_;
        std::function<void(const GLchar *, const glm::mat2x4 *, GLsizei)>  fptr_UpdateUniformMat2x4_;
        std::function<void(const GLchar *, const GLfloat *, GLsizei)> fptr_UpdateUniformMat2x4Array_;

        std::function<void(const GLchar *, const glm::mat3x2 *, GLsizei)>  fptr_UpdateUniformMat3x2_;
        std::function<void(const GLchar *, const GLfloat *, GLsizei)> fptr_UpdateUniformMat3x2Array_;
        std::function<void(const GLchar *, const glm::mat3 *, GLsizei)>  fptr_UpdateUniformMat3_;
        std::function<void(const GLchar *, const GLfloat *, GLsizei)> fptr_UpdateUniformMat3Array_;
        std::function<void(const GLchar *, const glm::mat3x4 *, GLsizei)>  fptr_UpdateUniformMat3x4_;
        std::function<void(const GLchar *, const GLfloat *, GLsizei)> fptr_UpdateUniformMat3x4Array_;

        std::function<void(const GLchar *, const glm::mat4x2 *, GLsizei)>  fptr_UpdateUniformMat4x2_;
        std::function<void(const GLchar *, const GLfloat *, GLsizei)> fptr_UpdateUniformMat4x2Array_;
        std::function<void(const GLchar *, const glm::mat4x3 *, GLsizei)>  fptr_UpdateUniformMat4x3_;
        std::function<void(const GLchar *, const GLfloat *, GLsizei)> fptr_UpdateUniformMat4x3Array_;
        std::function<void(const GLchar *, const glm::mat4 *, GLsizei)>  fptr_UpdateUniformMat4_;
        std::function<void(const GLchar *, const GLfloat *, GLsizei)> fptr_UpdateUniformMat4Array_;


        
        //There needs to be the following series of functions so that each one can match up with the signature
        //of the reassignable func pointers. Each one of these functions just calls the same 
        //inactiveUniformTrackingResponse() function. These functions should not be modified. Instead modify the
        //inactiveUniformTrackingResponse() function. All parameters are ignored



        static void warnNotActive1i(const GLchar * uname, const GLint x) { inactiveUniformTrackingResponse(); }
        static void warnNotActive1u(const GLchar * uname, const GLuint x) { inactiveUniformTrackingResponse(); }
        static void warnNotActive1f(const GLchar * uname, const GLfloat x) { inactiveUniformTrackingResponse(); }

        //Since the vector update function (i.e. Uniform1iv, Uniform2iv, Uniform3iv... ) all share the same signature, they
        //can all be set with just one of these functions. These also cover the array-based matrix update functions.
        static void warnNotActiveiv(const GLchar * uname, const GLint * v, GLsizei n) { inactiveUniformTrackingResponse(); }
        static void warnNotActiveuv(const GLchar * uname, const GLuint * v, GLsizei n) { inactiveUniformTrackingResponse(); }
        static void warnNotActivefv(const GLchar * uname, const GLfloat * v, GLsizei n) { inactiveUniformTrackingResponse(); }

        static void warnNotActive2i(const GLchar * uname, const GLint x, const GLint y) { inactiveUniformTrackingResponse(); }
        static void warnNotActive2u(const GLchar * uname, const GLuint x, const GLuint y) { inactiveUniformTrackingResponse(); }
        static void warnNotActive2f(const GLchar * uname, const GLfloat x, const GLfloat y) { inactiveUniformTrackingResponse(); }

        static void warnNotActive3i(const GLchar * uname, const GLint x, const GLint y, const GLint z) { inactiveUniformTrackingResponse(); }

        static void warnNotActive3u(const GLchar * uname, const GLuint x, const GLuint y, const GLuint z) { inactiveUniformTrackingResponse(); }

        static void warnNotActive3f(const GLchar* uname, const GLfloat x, const GLfloat y, const GLfloat z) { inactiveUniformTrackingResponse(); }

        static void warnNotActive4i(const GLchar * uname, const GLint x, const GLint y,
            const GLint z, const GLint w) { inactiveUniformTrackingResponse(); }
        static void warnNotActive4u(const GLchar * uname, const GLuint x, const GLuint y,
            const GLuint z, const GLuint w) { inactiveUniformTrackingResponse(); }
        static void warnNotActive4f(const GLchar * uname, const GLfloat x, const GLfloat y,
            const GLfloat z, const GLfloat w) { inactiveUniformTrackingResponse(); }
        
        static void warnNotActiveMat2(const GLchar * uname, const glm::mat2 * M, GLsizei n) { inactiveUniformTrackingResponse(); }
        static void warnNotActiveMat2x3(const GLchar * uname, const glm::mat2x3 * M, GLsizei n) { inactiveUniformTrackingResponse(); }
        static void warnNotActiveMat2x4(const GLchar * uname, const glm::mat2x4 * M, GLsizei n) { inactiveUniformTrackingResponse(); }

        static void warnNotActiveMat3x2(const GLchar * uname, const glm::mat3x2 * M, GLsizei n) { inactiveUniformTrackingResponse(); }
        static void warnNotActiveMat3(const GLchar * uname, const glm::mat3 * M, GLsizei n) { inactiveUniformTrackingResponse(); }
        static void warnNotActiveMat3x4(const GLchar * uname, const glm::mat3x4 * M, GLsizei n) { inactiveUniformTrackingResponse(); }

        static void warnNotActiveMat4x2(const GLchar * uname, const glm::mat4x2 * M, GLsizei n) { inactiveUniformTrackingResponse(); }
        static void warnNotActiveMat4x3(const GLchar * uname, const glm::mat4x3 * M, GLsizei n) { inactiveUniformTrackingResponse(); }
        static void warnNotActiveMat4(const GLchar * uname, const glm::mat4 * M, GLsizei n) { inactiveUniformTrackingResponse(); }
        
    };

} //namespace ShaderInterface

#endif //UNIFORM_LOCATION_INTERFACE_H_