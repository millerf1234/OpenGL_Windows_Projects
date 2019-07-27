

#include "ImageFileLoader.h"
#include <cstdio>
#include "LoggingMessageTargets.h"

//Prepare to include stb_image
//Define configuration options


////////////////////////////////////////
//////  Error Message Verboseness
////////////////////////////////////////
//
// Option A
//Disable Failure Messages in stb_image with the macro
//#define STBI_NO_FAILURE_STRINGS
//
// Option B
//To Get Normal Failure Messages, don't define the macros
//for options 'A' and 'C'
//
// Option C
//To Get Increased Verbosity in failure messages, use the macro
#define STBI_FAILURE_USERMSG


#define STBI_WINDOWS_UTF8

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"










inline bool checkForFailure() noexcept{
    const char* failureReason = stbi_failure_reason();
    if (failureReason) {
        printf("  [Failure Reason: %s]\n", failureReason);
        return true;
    }
    return false;
}

////////////  Definition and Implementation for the class ImageFileLoader
class  ImageFileLoader::ImageFileLoaderImpl {
public:

    ImageFileLoaderImpl(std::filesystem::path imgFile) : isHdr(false),
                                                         is16Bit(false),
                                                         w(0), h(0), comp(0)
                                                         {

        assert(!imgFile.empty());
        assert(std::filesystem::exists(imgFile));

        //stb_image has a maximum filepath length it supports
        static constexpr const size_t FILENAME_MAX_LENGTH = FILENAME_MAX;
       
        std::string fileName = imgFile.string();
        std::string fileNameUTF8 = imgFile.u8string();
        const wchar_t* cstrFilenameWide = imgFile.c_str();
        char cstrFilename[8000] = {'\0'};
        int conversionResult = stbi_convert_wchar_to_utf8(cstrFilename, 6000, cstrFilenameWide);

        printf("\n\n\nPreparing to use stb_image with image file:\n\t\"%s\"\n\n", imgFile.string().c_str());

        printf("Before starting, let's see if we can open this file ourselves...\n");
        FILE* fileHandle;
        const errno_t fileOpeningError = fopen_s(&fileHandle, cstrFilename, "r");
        printf("File Open Attempt returned result: %d\n", fileOpeningError);
        if (fileOpeningError)
            return;


        printf("First we should query details about the file like such: \n");
        printf("    Is Hdr: ");
        isHdr = stbi_is_hdr(cstrFilename);
        printf("%s\n", isHdr ? "True" : "False");

        if (checkForFailure()) {
            //return;
        }

        //fprintf(MSGLOG, "\nThe File Extension is: %s\n",
        //    imgFile.extension().string().data());

                    //It looks like stbi_is_16_bit() only checks files of type png and psd.
            //All other types (i.e. JPEG) will return false from this function.
        const auto fileExtension = imgFile.extension().string().data();
        if (!strcmp(fileExtension, ".png") || (!strcmp(fileExtension, ".psd"))) {
            printf("    Is 16-bit: ");
            is16Bit = stbi_is_16_bit(cstrFilename);
            printf(" %d\n", is16Bit);

            if (checkForFailure()) {
                printf("Failed to determine if image is 16-bit or not!\n");
            }
        }
        assert(fileHandle);
        printf("     File Info: \n");
        //const int success = stbi_info(cstrFilename, &w, &h, &comp);
        const int success = stbi_info_from_file(fileHandle, &w, &h, &comp);
        printf("    [stbi_info() returned %d\n", success);
        if (checkForFailure()) {
            //return;
        }
        printf("    w:    %d\n    h:    %d\n    comp: %d\n", w, h, comp);


        printf("\n\n\n");

        fclose(fileHandle);


    }




    ~ImageFileLoaderImpl() noexcept { ; }

private:
    bool isHdr;
    bool is16Bit;
    int w, h, comp;
};














//////// Implementation For ImageFileLoader class
ImageFileLoader::ImageFileLoader(std::filesystem::path imgPath) : pImpl_(nullptr) {
    pImpl_ = new ImageFileLoaderImpl(imgPath);
    assert(pImpl_);
}

ImageFileLoader::~ImageFileLoader() noexcept {
    if (pImpl_) 
        delete pImpl_;
}