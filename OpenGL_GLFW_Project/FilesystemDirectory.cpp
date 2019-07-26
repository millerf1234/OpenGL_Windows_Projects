//
// File:                                        FilesystemDirectory.cpp
//
//
//
//

#include <cassert>
#include <set>
#include "FilesystemDirectory.h"
#include "LoggingMessageTargets.h"

//Class DirMemberNames used internally 
class FilesystemDirectory::DirMemberNames {
public:
    DirMemberNames() = delete;
    DirMemberNames(std::filesystem::path dirPath) : mDirPath_(dirPath) {
        iterateDirectoryToGetEntryNames();
    }
    ~DirMemberNames() noexcept;
    DirMemberNames(DirMemberNames&& that) noexcept {
        mDirPath_ = that.mDirPath_;
        members.swap(that.members);
        memberFiles.swap(that.memberFiles);
    }
    std::set<std::filesystem::path> members;
    std::set<std::filesystem::path> memberFiles;

    //Causes this object to flush all of its locally-stored
    //data then replenishes its datastores by rebuilding itself
    //from the OS.
    void refresh() {
        iterateDirectoryToGetEntryNames();
    }

private: 
    std::filesystem::path mDirPath_;

    void iterateDirectoryToGetEntryNames() {
        memberFiles.clear(); //Best way to clear a set 
        members.clear();
        for (auto& entry : std::filesystem::directory_iterator(mDirPath_)) {
            std::filesystem::path entryPath = entry.path();
            if (entry.is_regular_file()) {
                entryPath = entryPath.filename();
                memberFiles.insert(entryPath);
            }
            members.insert(entryPath);
        }
    }
};

FilesystemDirectory::DirMemberNames::~DirMemberNames() noexcept {  ;  }


FilesystemDirectory::~FilesystemDirectory() noexcept {

}


//FilesystemDirectory::FilesystemDirectory(std::string_view path) :
//    FilesystemDirectory(std::filesystem::path{ path }) {
//
//}


FilesystemDirectory::FilesystemDirectory(std::filesystem::path path) : mPath_(path),
                                                                       mEntries_(nullptr) {
    assert(!(mPath_.empty()));
    try {
        //If a directory does not exist at this location, create it
        if (!(std::filesystem::is_directory(mPath_))) {
            fprintf(MSGLOG, "\nDetected that currently there does not exist a file system\n"
                "directory named \"%s\"\n"
                "A directory at this location is being created.\n\n", mPath_.lexically_normal().string().c_str());
            std::filesystem::create_directory(mPath_);
        }

        //Create the internal object storing all of this directories members
        mEntries_ = std::make_unique<DirMemberNames>(mPath_);
    }
    catch (const std::filesystem::filesystem_error& e) {
        fprintf(ERRLOG, "\n"
            "\n;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;"
            "\n;                  Error -- Filesystem Request Denied!!!                     ;"
            "\n; Explanation: Now This Can Be Simply Chalked Up To Plain Old Rotten Luck!   ;"
            "\n;              Everything Was Going Swimingly Until We Tried To Access The   ;"
            "\n;              OS's Filesystem. Yup. The Application Failed Due To A Quite   ;"
            "\n               Unexpected \'std::filesystem::filesystem_error\' exception.   ;"
            "\n;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;"
            "\n Filesystem Exception Message:___.             [This Probably Just Some Lame  "
            "\n  __________                     |              Excuse From The OS On Why It  "
            "\n /          \\;-------------------*              Failed At Such A Mundane Task]"
            "\n \\->  \"%s\"\n\n\n", e.what());
        assert(false);
    }
    catch (const std::exception& e) {
        fprintf(ERRLOG, "\n"
            "\n;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;"
            "\n;                  Error -- Filesystem Request Denied!!!                     ;"
            "\n; Explanation: Well It Turns Out That An Exception Was Thrown, Which Means   ;"
            "\n;              You May Have Just Found A Bug! Please Report The Following    ;"
            "\n;              Exception Message To Your Nearest Local Developer!            ;"
            "\n;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;"
            "\nException Message: %s\n\n\n", e.what());
        //activeScreenshotTasks.emplace_back(forceBeginAsyncTask(&(ScreenCaptureAssistant::reportExceptionCaught)), *this, e.what());
        assert(false);
    }
}

FilesystemDirectory::FilesystemDirectory(FilesystemDirectory&& that) noexcept {
    mPath_ = that.mPath_;
}

FilesystemDirectory& FilesystemDirectory::operator=(FilesystemDirectory&& that) noexcept {
    if (this != &that) 
        mPath_ = that.mPath_;
    return *this;
}

//Returns a const reference to the filesystem path of
//this directory
const std::filesystem::path& FilesystemDirectory::getPath() const noexcept {
    return mPath_;
}


std::filesystem::path FilesystemDirectory::getNextUniqueFilenameFor(std::string_view baseName) {
    
    assert(!(baseName.empty()));
    //This next line is inefficient but is necessary for now until a better solution can be implemented
    mEntries_->refresh();
    int nameMatchCounter = 0;
    std::string newUniqueFileName(baseName);
    auto fileNameIterator = mEntries_->memberFiles.cbegin();
    for (; fileNameIterator != mEntries_->memberFiles.cend(); fileNameIterator++) {
        const std::string fileName = fileNameIterator->string();
        //See if we can find our baseName as a substring in this file
        std::string::size_type idx = fileName.find(baseName);
        if (idx != std::string::npos)
            nameMatchCounter++;
        
    }
    if (nameMatchCounter == 0)
        newUniqueFileName.append("00");
    else if (nameMatchCounter < 10) {
        newUniqueFileName.append("0");
        newUniqueFileName.append(std::to_string(nameMatchCounter));
    }
    else 
        newUniqueFileName.append(std::to_string(nameMatchCounter));

    char delimiterChar = *(mPath_.string().crbegin());
    
    auto returnedPath = mPath_.string();
    if ((delimiterChar != '\\') && (delimiterChar != '/'))
        returnedPath += "\\";
    returnedPath += newUniqueFileName;

    return returnedPath;
}