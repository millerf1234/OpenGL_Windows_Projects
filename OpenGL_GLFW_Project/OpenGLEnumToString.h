//
//  File:                                          OpenGLEnumToString.h
//
//  Description:                Provides definition for a free function
//                                                             meant to
//                               convert the numerous GLenum hex values
//                                         into a human-readable string 
//                                       representation of their macro. 
//
//  Completeness:                There are far to many GLenum values in
//                            total for these functions to exhaustively 
//                             be able to convert every possible input, 
//                                  but these functions are meant to be 
//                               continuously growing. If a macro which 
//                             doesn't yet map to a string is required,
//                                           simply add it to the list.
//
//
//  DISCLAIMER:                       Chances are very high I will have
//                                  made a typo somewhere in dictionary
//
//
//  Programmer:                                          Forrest Miller
//  Epoch:                                                  Summer 2019
//

#pragma once

#ifndef OPENGL_ENUM_TO_STRING_H_
#define OPENGL_ENUM_TO_STRING_H_

#include <string>
#include <unordered_map>    //An unordered map is used to provide 
//                          //(hopefully) constant lookup time.
#include "GlobalIncludes.h"

constexpr const char* UNKNOWN_GLENUM_NAME = "UNKNOWN";

//Creates a std::string object based on the requested hexEnum value.
//Will return a string containing constexpr UNKNOWN_GLENUM_NAME if
//the requested GLenum is not yet recorded within the internal 
//hash table.
std::string convertGLEnumToString(GLenum hexEnum) noexcept;


namespace GL_ENUM_TO_STRING_INTERNAL_NAMESPACE {
    class DictionaryOfGLEnums {
    public:
        DictionaryOfGLEnums();
        ~DictionaryOfGLEnums() noexcept;

        DictionaryOfGLEnums(const DictionaryOfGLEnums&) = delete;
        DictionaryOfGLEnums(DictionaryOfGLEnums&& that) noexcept = default;
        DictionaryOfGLEnums& operator=(const DictionaryOfGLEnums&) = delete;
        DictionaryOfGLEnums& operator=(DictionaryOfGLEnums&&)& = default;
        //DictionaryOfGLEnums& operator=(DictionaryOfGLEnums&&)&& = default;

        std::string lookup(GLenum hexEnum) noexcept;

    private:
        std::unordered_map<GLenum, std::string> mDictionary_;
        
        //These 2 utility functions are used to build the dictionary.
        // 'insert()' adds a single GLenum-string pair to the dictionary 
        void insert(GLenum hexEnum, std::string name);
        //'buildDictionary()' calls 'insert()' for many dictionary entries 
        void buildDictionary();
    };
}

#endif //OPENGL_ENUM_TO_STRING_H_



