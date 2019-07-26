//
// File:                     FilesystemDirectory.h
// Class:                    FilesystemDirectory
//
//        Wraps a physical filesystem directory in the
//        OS. If no directory exists yet, this object will
//        create the directory in its constructor. 
//


#pragma once


#ifndef FILESYSTEM_DIRECTORY_H_
#define FILESYSTEM_DIRECTORY_H_

#include <string>
#include <string_view>
#include <memory>
#include <filesystem>

class FilesystemDirectory {
public:
    //The path to the filesystem directory is a type invariant so 
    //having a default constructor is impossible
    FilesystemDirectory() = delete;
    FilesystemDirectory(const FilesystemDirectory&) = delete;
    FilesystemDirectory& operator=(const FilesystemDirectory&) = delete;
    ~FilesystemDirectory() noexcept; 
    //FilesystemDirectory(std::string_view path);
    FilesystemDirectory(std::filesystem::path path);
    FilesystemDirectory(FilesystemDirectory&& that) noexcept;
    FilesystemDirectory& operator=(FilesystemDirectory&& that) noexcept;

    //Returns a const reference to the filesystem path of
    //this directory
    const std::filesystem::path& getPath() const noexcept;

    //Determines a new unique filename based upon a base pattern. Does not create
    //this file. Does not add a filepath to the name. The name does not include 
    //any directories. Not thread safe. May Throw Exceptions
    std::filesystem::path getNextUniqueFilenameFor(std::string_view baseName);
    
private:
    std::filesystem::path mPath_;
    
    class DirMemberNames;
    std::unique_ptr<DirMemberNames> mEntries_;
};



#endif //FILESYSTEM_DIRECTORY_H_
