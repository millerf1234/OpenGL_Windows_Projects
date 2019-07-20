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



//I wound up having to slightly modify this following third party code to get
//it up to Visual Studio's standards. The modifications were necessary because 
//the c-function 'fopen()' won't compile in modern visual studio without first  
//disabling some security features. Instead the safe version 'fopen_s()' is
//used. The difference with 'fopen_s()' is instead of returning a FILE*, it 
//now returns a errno_t based upon the result of attempting to open the file. 
//The FILE* is now passed back to the application using a pointer to it as the 
//first argument to 'fopen_s()'. To get this errno_t back out to the 
//TGAImageImpl code, an additional parameter is passed to the constructor which
//is a pointer to an errno_t. The result of opening the file will be written 
//to this pointer. If the pointer is NULL, no information about the operation
//will be returned.

//To keep the distinction clear between my code and not my code, I implement
//my changes all behind a macro
#define REPLACEMENT_CONSTRUCTOR                                               \
TGAFileImpl(const char* file, errno_t* result)                                \
{                                                                             \
    const errno_t fileOpeningError = fopen_s(&m_hFile, file, "r");            \
    if (result)                                                               \
        * result = fileOpeningError;                                          \
}

//----------------------------------------------------------------------------\
//////////////////////////////////////////////////////////////////////////////|
////////////////////////       THIRD PARTY CODE       ////////////////////////|
//////////////////////////////////////////////////////////////////////////////|
//      The TGA SDK library's sample code defines this following class      //|
//      which seems to mesh well with the rest of the library so best       //|
//         save a lot of time and trouble by using it ourselves.            //|
class TGAFileImpl : public ITGAStream                                       //|
{                                                                           //|
    FILE* m_hFile;                                                          //|
    size_t      m_size;                                                     //|
public:                                                                     //|
    /*  ORIGINAL_CONSTRUCOR                                                 //|
    TGAFileImpl(const char* file)                                           //|
        : m_hFile(0)                                                        //|
    {                                                                       //|
        m_hFile = fopen(file, "r"); //Unsafe version using old C func       //|
    }                                                                       //|
    */ REPLACEMENT_CONSTRUCTOR;                                             //|
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


/*
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

        */




        }
        catch (const TGAError& tgaErr) {
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
        catch (...) {
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
        mData_ = { 0u, 0u, 0u, 0u,     0u, 0u, 0u, 0u,
                   0u, 0u, 0u, 0u,     0u, 0u, 0u, 0u };
        mpProperties_ = nullptr;
        mWidth_ = 2;
        mHeight_ = 2;
        mComponents_ = 4;
    }
};







//---------------------------------------------------
//                     TGAImage
//---------------------------------------------------


TGAImage::TGAImage() : mpImpl_(std::make_unique<TGAImageImpl>()) { ; }

TGAImage::TGAImage(const std::filesystem::path& tgaFilepath) :
    mpImpl_(std::make_unique<TGAImageImpl>(tgaFilepath)) { ; }


TGAImage::~TGAImage() noexcept { ; }


TGAImage::TGAImage(TGAImage&& that) noexcept { mpImpl_ = std::move(that.mpImpl_); }

TGAImage& TGAImage::operator=(TGAImage&& that) noexcept {
    assert(that.mpImpl_);
    if (this != &that)
        mpImpl_ = std::move(that.mpImpl_);
    return *this;
}


std::string_view TGAImage::errorReportString() const noexcept {
    assert(mpImpl_);
    return mpImpl_->errorReportString();
}

//Returns the image's width
int TGAImage::width() const noexcept {
    return mpImpl_->width();
}

//Returns the image's height
int TGAImage::height() const noexcept {
    return mpImpl_->height();
}

//Returns the number of components per pixel the data uses.
int TGAImage::components() const noexcept {
    return mpImpl_->components();
}

//Gives access to the loaded image data
std::vector<uint8_t>& TGAImage::dataVector() noexcept {
    return mpImpl_->dataVector();
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