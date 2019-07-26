//
//  File:                                                          TGAImage.cpp
//
//  documentation forthcoming 
//  
//  July 20, 2019
//  Forrest Miller

#include "TGAImage.h"

#include <vector>
#include <cassert>

#include "TGASDK/include/TGA.h"
#include "TGASDK/include/TGAFile.h"
#include "TGASDK/include/TGAError.hpp"

/*                    TGA SDK Image File Loader Class
X -~-~-  -~-~-  -~-~-  -~-~-  -~-~-  -~-~-  -~-~-  -~-~-  -~-~-  -~-~-  -~-~- X
|      The TGA SDK library's sample code uses this following class for        |
|      performing file I/O. Advantage is taken of having access to this       |
|      already implemented and [presumably] correct class by first            |
|      slightly modifying it to conform with modern standards [as             |
|      described next] then simply reusing it as part of TGAImage's           |
|      implementation to serve as the file loader. This is ideal because      |
|      the TGA SDK is able to construct its parser object directly from       |
|      any TGAFileImpl which has successfully acquired file data.             |
X -~-~-  -~-~-  -~-~-  -~-~-  -~-~-  -~-~-  -~-~-  -~-~-  -~-~-  -~-~-  -~-~- X 
*/

///////////////////////////////////////////////////////////////////////////////
////         MODIFICATION REQUIRED TO COMPLY WITH MODERN STANDARDS         ////
///////////////////////////////////////////////////////////////////////////////
////     To allow the 'TGAFileImpl' class to function within a Modern      ////
////     Visual Studio project, a modification to its constructor was      ////
////     necessary. This is because the unmodified implementation uses     ////
////       the old traditional C-function 'fopen()'. Unfortunately         ////
////     'fopen()' is so ancient that it suffers from potential buffer     ////
////        overflows which in modern computers is a major security        //// 
////     vulnerability. Thus 'fopen()' has been deprecated by Microsoft    ////
////      and will fail to compile in modern Visual Studio [unless the     ////
////        code explicitly disables some of its security features].       ////
////                                                                       ////
////        To allow code reliant upon 'fopen()' a viable alternative,     ////
////    Microsoft has introduced their custom safe version 'fopen_s()'.    ////
////   The difference with 'fopen_s()' is instead of returning a FILE*,    ////
////   it now returns an errno_t based upon the result of attempting to    ////
////   open the file. The FILE* is now passed back to the calling code     ////
////   through the usage of a pointer to the FILE* which will be set to    ////
////   the open FILE handle on success. To get this errno_t back out to    ////
////   the TGAImageImpl code, an additional parameter is passed to the     ////
////     constructor which is a pointer to an errno_t. The result of       ////
////      opening the file will be written to this pointer. If the         ////
//// pointer is NULL, no information about the operation will be returned. ////
///////////////////////////////////////////////////////////////////////////////

//To keep the distinction clear between my code and not my code, I implement
//my changes all behind this macro
#define REPLACEMENT_CONSTRUCTOR                                               \
TGAFileImpl(const char* file, errno_t* result)                                \
{                                                                             \
    const errno_t fileOpeningError = fopen_s(&m_hFile, file, "r");            \
    if (result)                                                               \
        *result = fileOpeningError;                                           \
}

//----------------------------------------------------------------------------\
//////////////////////////////////////////////////////////////////////////////|
////////////////////////       THIRD PARTY CODE       ////////////////////////|
//////////////////////////////////////////////////////////////////////////////|
//                                                                          //|
class TGAFileImpl : public ITGAStream                                       //|
{                                                                           //|
    FILE* m_hFile;                                                          //|
    size_t      m_size;                                                     //|
public: /* ORIGINAL_CONSTRUCOR                                              //|
    TGAFileImpl(const char* file)                                           //|
        : m_hFile(0)                                                        //|
    {                                                                       //|
        m_hFile = fopen(file, "r"); //Unsafe version using old C func       //|
    }*/ REPLACEMENT_CONSTRUCTOR;                                            //|
                                                                            //|
    int read(void* pBuffer, int sizeBlock)                                  //|
    {                                                                       //|
        return fread(pBuffer, sizeof(unsigned __int8), sizeBlock, m_hFile); //|
    }                                                                       //|
                                                                            //|
    int seek(__int64 offset, TGAStremOrigin SeekMethod)                     //|
    {                                                                       //|
        return _fseeki64(m_hFile, offset, SeekMethod);                      //|
    }                                                                       //|
                                                                            //|
    unsigned __int64 tell()                                                 //|
    {                                                                       //|
        return _ftelli64(m_hFile);                                          //|
    }                                                                       //|
                                                                            //|
    unsigned __int64 size() const                                           //|
    {                                                                       //|
        return m_size;                                                      //|
    }                                                                       //|
};                                                                          //|
//////////////////////////////////////////////////////////////////////////////|
////////////////////////     END THIRD PARTY CODE     ////////////////////////|
//////////////////////////////////////////////////////////////////////////////|
//----------------------------------------------------------------------------/


#ifdef REPLACEMENT_CONSTRUCTOR
#undef REPLACEMENT_CONSTRUCTOR
#endif 




//---------------------------------------------------
//                   TGAImageImpl
//---------------------------------------------------
class TGAImage::TGAImageImpl {
public:
    

    //DESCTRUCTOR
    ~TGAImageImpl() { ; }





    //CONSTRUCTORS

    TGAImageImpl() {
        constructFromEmptyFile();
    }


    //This is the constructor that does the heavy lifting 
    TGAImageImpl(const std::filesystem::path& tgaFilepath) {
        
        //Validate the filepath before attempting to load
        if (tgaFilepath.filename().empty()) {
            mErrReport_ += std::string("[ERROR] Filepath For Requested TGA Image Was Empty!\n");
            constructFromEmptyFile();
            return;
        }
        else if (tgaFilepath.extension() != std::filesystem::path(".tga")) {
            const std::string msgPart1 = "[ERROR] Filepath to TGA Image \"";
            const std::string msgPart2 = tgaFilepath.string();
            const std::string msgPart3 = "\"\n        lacks the required \".tga\" extension!\n";
            mErrReport_ += msgPart1;
            mErrReport_ += msgPart2;
            mErrReport_ += msgPart3;
            constructFromEmptyFile();
            return;
        }
        else if (!(std::filesystem::exists(tgaFilepath))) {
            const std::string msgPart1 = "[ERROR] Filepath to Requested TGA Image \"";
            const std::string msgPart2 = tgaFilepath.string();
            const std::string msgPart3 = "\"\n        does not refer to an existing file!\n";
            mErrReport_ += msgPart1;
            mErrReport_ += msgPart2;
            mErrReport_ += msgPart3;
            constructFromEmptyFile();
            return;
        }
        else if (!(std::filesystem::is_regular_file(tgaFilepath))) {
            const std::string msgPart1 = "[ERROR] Filepath to Requested TGA Image \"";
            const std::string msgPart2 = tgaFilepath.string();
            const std::string msgPart3 = "\"\n        does not refer to a loadable file!\n";
            mErrReport_ += msgPart1;
            mErrReport_ += msgPart2;
            mErrReport_ += msgPart3;
            constructFromEmptyFile();
            return;
        }

        //Otherwise we are ready to start cooking with gas
       
        //Best to play things safe and surround the whole interaction with the 
        try { //TGA SDK API in one huge try block to seal off any rouge exceptions
            //from reaching any further than this classes implementation

            //This is based largely off the sample code accompanying the TGA SDK library
            constexpr const errno_t NO_ERROR = 0;
            errno_t fileOpenError = NO_ERROR;
            TGAFileImpl tgaFile(tgaFilepath.string().c_str(), &fileOpenError);
            if (fileOpenError != NO_ERROR) {
                const std::string msgPart1 = "[ERROR] Filepath to Requested TGA Image \"";
                const std::string msgPart2 = tgaFilepath.string();
                const std::string msgPart3 = "\"\n        failed to open!\n";
                mErrReport_ += msgPart1;
                mErrReport_ += msgPart2;
                mErrReport_ += msgPart3;
                constructFromEmptyFile();
                return;
            }

            TGA tga(&tgaFile);
            tga.read();
            const TGAHeader& hdr = tga.header();
            const TGAField& field = hdr.field();
            const ColorMapSpecification& cmap = hdr.color_map_specification();
            const ImageSpecification& image_spec = hdr.image_specification();

            auto sizeHeader = hdr.size();

            std::vector<unsigned __int8> vImageID; vImageID.resize((int)tga.image_id_size());
            std::vector<unsigned __int8> vColorMapData; vColorMapData.resize((int)tga.color_map_data_size());
            std::vector<unsigned __int8> vImageData; vImageData.resize((int)tga.image_data_size());

            tga.image_id(vImageID.data());
            tga.color_map_data(vColorMapData.data());
            tga.image_data(vImageData.data());

            printf("\nNow let's pause a second to see where we are at!\n");

            printf("\nCopying data to TGAImage's vector of uint8_t!\n");
            mData_.reserve(vImageData.size());
            for (auto c : vImageData) {
                mData_.push_back(c);
            }

            mWidth_ = image_spec.width;
            mHeight_ = image_spec.height;
            mComponents_ = image_spec.pixel_depth / 8;
        }

        catch (const TGAError& tgaErr) { //Most likely we will be catching a TGA error
            const std::string msgPart1 = "[ERROR] While attempting to load the TGA file \"";
            const std::string msgPart2 = tgaFilepath.string();
            const std::string msgPart3 = "\"\n        a TGAError exception was thrown!\n";
            const std::string msgPart4 = " Message  --->  \"" + std::string(tgaErr.msg()) + std::string("\"\n");
            mErrReport_ += msgPart1;
            mErrReport_ += msgPart2;
            mErrReport_ += msgPart3;
            mErrReport_ += msgPart4;
            constructFromEmptyFile();
            return;
        }

        catch (const std::system_error& e) {
            const std::string msgPart1 = "[ERROR] While attempting to load the TGA file \"";
            const std::string msgPart2 = tgaFilepath.string();
            const std::string msgPart3 = "\"\n        a system_error exception was thrown!\n";
            const std::string msgPart4 = "   e.what()  --->  \"" + std::string(e.what()) + std::string("\"\n");
            mErrReport_ += msgPart1;
            mErrReport_ += msgPart2;
            mErrReport_ += msgPart3;
            mErrReport_ += msgPart4;
            constructFromEmptyFile();
            return;
        }

        catch (const std::exception& e) {
            const std::string msgPart1 = "[ERROR] While attempting to load the TGA file \"";
            const std::string msgPart2 = tgaFilepath.string();
            const std::string msgPart3 = "\"\n        a system_error exception was thrown!\n";
            const std::string msgPart4 = "   e.what()  --->  \"" + std::string(e.what()) + std::string("\"\n");
            mErrReport_ += msgPart1;
            mErrReport_ += msgPart2;
            mErrReport_ += msgPart3;
            mErrReport_ += msgPart4;
            constructFromEmptyFile();
            return;
        }

        catch (...) { //Gotta catch 'em all 
            mErrReport_ += "[ERROR] While Attempting To Load The TGA file \"";
            mErrReport_ += tgaFilepath.string();
            mErrReport_ += "\"\n        an unknown exception was thrown!\n";
            constructFromEmptyFile();
            return;
        }
    }


    //MOVE OPERATIONS
    TGAImageImpl(TGAImageImpl&& that) noexcept {
        printf("\n\n TGAImageImpl move constructor was called yo!\n");
        mPath_ = that.mPath_;
        mData_ = that.mData_;
        mErrReport_.swap(that.mErrReport_);

        mHeight_ = that.mHeight_;
        mWidth_ = that.mWidth_;
        mComponents_ = that.mComponents_;

        mpProperties_ = std::move(that.mpProperties_);//std::make_unique<TGAFileProperties>(that.mpProperties_.get());
    }

    TGAImageImpl& operator=(TGAImageImpl&& that) noexcept {
        printf("\n\n TGAImageImpl move operator was called yo!\n");
        if (this != &that) {
            mPath_ = that.mPath_;
            mData_ = that.mData_;
            mErrReport_.swap(that.mErrReport_);

            mHeight_ = that.mHeight_;
            mWidth_ = that.mWidth_;
            mComponents_ = that.mComponents_;

            mpProperties_ = std::move(that.mpProperties_);
        }
        return *this;
    }

    

    std::string_view errorReportString() const noexcept {
        return mErrReport_;
    }


    int width() const noexcept { return mWidth_; }
    int height() const noexcept { return mHeight_; }
    int components() const noexcept { return mComponents_; }
    std::vector<uint8_t>& dataVector() noexcept { return mData_; }


private: 
    //Fields
    std::filesystem::path mPath_;
    std::vector<uint8_t> mData_;
    int mWidth_, mHeight_, mComponents_;
    mutable std::string mErrReport_; //Might not need to be declared as mutable



    typedef struct TGAFileProperties {
        bool valid;

        //Need to explicitly give this struct move operations
        TGAFileProperties(TGAFileProperties&& that) = default;
        TGAFileProperties& operator=(TGAFileProperties&& that) = default;

    } Properties;
    
    std::unique_ptr<TGAFileProperties> mpProperties_;



    //Utility Functions

    void constructFromEmptyFile() noexcept {

        mPath_ = "";
        mpProperties_ = nullptr;

#define DO_PROGRAMED_TEST_PATTERN
#ifndef DO_PROGRAMED_TEST_PATTERN
        mData_ = { 0u, 0u, 255u, 155u,     0u, 255u, 0u, 255u,
                   0u, 255u, 0u, 155u,     0u, 0u, 255u, 255u };
        mWidth_ = 2;
        mHeight_ = 2;
        mComponents_ = 4;

#else
        mWidth_ = 60;
        mHeight_ = 60;
        mComponents_ = 4;

        mData_.reserve(mWidth_ * mHeight_ * 4);
        for (int i = 0; i < mWidth_ * mHeight_; i++) {
            const uint8_t iMod = static_cast<uint8_t>(i/2 % 255);
            const uint8_t iMod_32 = static_cast<uint8_t>((i / 32) % 255);
            //Blue
            mData_.push_back(iMod_32);

            //Green
            if (iMod < 25u)
                mData_.push_back(255u);
            else if (iMod < 50u)
                mData_.push_back(125u);
            else if (iMod < 75u)
                mData_.push_back(55u);
            else if (iMod < 100u)
                mData_.push_back(2u + 2u * (iMod + 100u));
            else
                mData_.push_back(iMod - 97u);

            //Red
            mData_.push_back(255u - (iMod_32));

            //Alpha
            if (iMod % 2u == 0u)
                mData_.push_back(0u);
            else
                mData_.push_back(255u - iMod / 5u);

        }
#endif //DO_PROGRAMED_TEST_PATTERN


    }

};
 






//---------------------------------------------------
//                     TGAImage
//---------------------------------------------------


TGAImage::TGAImage() : pImpl_(std::make_unique<TGAImageImpl>()) { ; }

TGAImage::TGAImage(const std::filesystem::path& tgaFilepath) :
    pImpl_(std::make_unique<TGAImageImpl>(tgaFilepath)) { ; }


TGAImage::~TGAImage() noexcept { ; }


TGAImage::TGAImage(TGAImage&& that) noexcept { pImpl_ = std::move(that.pImpl_); }

TGAImage& TGAImage::operator=(TGAImage&& that) noexcept {
    assert(that.pImpl_);
    if (this != &that)
        pImpl_ = std::move(that.pImpl_);
    return *this;
}


std::string_view TGAImage::errorReportString() const noexcept {
    assert(pImpl_);
    return pImpl_->errorReportString();
}

//Returns the image's width
int TGAImage::width() const noexcept {
    return pImpl_->width();
}

//Returns the image's height
int TGAImage::height() const noexcept {
    return pImpl_->height();
}

//Returns the number of components per pixel the data uses.
int TGAImage::components() const noexcept {
    return pImpl_->components();
}

//Gives access to the loaded image data
std::vector<uint8_t>& TGAImage::dataVector() noexcept {
    return pImpl_->dataVector();
}

/*

int main(int argc, char *argv[])
{
  if (argc != 2)
    return 0;
  TGAFileImpl tgaFile(argv[1]);

  TGA tga(&tgaFile);
  tga.read();

  const TGAHeader& hdr = tga.header();
  const TGAField& field = hdr.field();
  const ColorMapSpecification& cmap = hdr.color_map_specification();
  const ImageSpecification& image_spec = hdr.image_specification();

  auto sizeHeader = hdr.size();

  std::vector<unsigned __int8> vImageID; vImageID.resize((int)tga.image_id_size());
  std::vector<unsigned __int8> vColorMapData; vColorMapData.resize((int)tga.color_map_data_size());
  std::vector<unsigned __int8> vImageData; vImageData.resize((int)tga.image_data_size());

  tga.image_id(vImageID.data());
  tga.color_map_data(vColorMapData.data());
  tga.image_data(vImageData.data());

  return 0;
}
*/