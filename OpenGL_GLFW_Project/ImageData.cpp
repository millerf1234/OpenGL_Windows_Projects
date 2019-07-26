//
// FILE:                                                         ImageData.cpp
//
//
//
//
//  July 24, 2019
// 


#include "ImageData.h"

#include <string>
#include "RelativeFilepathsToResources.h"
#include "ImageDataLoader.h"


//Used for debugging purposes 
#define OUTPUT_REPORT_TO_FILE_ON_DEFAULT_TEXTURE_GENERATION true



#if (defined OUTPUT_REPORT_TO_FILE_ON_DEFAULT_TEXTURE_GENERATION) &&        \
    (OUTPUT_REPORT_TO_FILE_ON_DEFAULT_TEXTURE_GENERATION)
#define GEN_REPORT
    //These following includes are only needed if going to generate the report
#include <fstream>
typedef struct IJPair {
    GLsizei i, j;
    IJPair(GLsizei i, GLsizei j) : i(i), j(j) { ; }
    std::string to_string() const noexcept {
        std::string iStr = std::to_string(i);
        switch (iStr.length()) {
        default:
            break;
        case 1U:
            iStr = "    " + iStr;
            break;

        case 2U:
            iStr = "   " + iStr;
            break;
        case 3U:
            iStr = "  " + iStr;
            break;
        case 4U:
            iStr = " " + iStr;
            break;
        }
        std::string jStr = std::to_string(j);
        switch (jStr.length()) {
        default:
            break;
        case 1U:
            jStr = "    " + jStr;
            break;

        case 2U:
            jStr = "   " + jStr;
            break;
        case 3U:
            jStr = "  " + jStr;
            break;
        case 4U:
            jStr = " " + jStr;
            break;
        }
        std::string s = "[" + iStr + std::string(",") +
            jStr + std::string("]");
        return s;
    }
} IJPair;

static constexpr const int NUM_LOGIC_PATHS = 6;
static constexpr const int LOGIC_PATH_1 = 0;
static constexpr const int LOGIC_PATH_2 = 1;
static constexpr const int LOGIC_PATH_3 = 2;
static constexpr const int LOGIC_PATH_4 = 3;
static constexpr const int LOGIC_PATH_5 = 4;
static constexpr const int LOGIC_PATH_6 = 5;


#endif //OUTPUT_REPORT_TO_FILE_ON_DEFAULT_TEXTURE_GENERATION



static unsigned long long NEXT_DEFAULT_IMAGE_NAME = 0ULL;

//Helper function for generating default data
std::vector<uint8_t> generateImageData(GLsizei width, GLsizei height) {
 
    
    assert(width > 0);
    assert(height > 0);


    std::vector<uint8_t> generatedData;
    generatedData.reserve(width * height * 4);


    //Check to see if should generate a detailed report on which loop 
    //options get selected
#ifdef GEN_REPORT
    static constexpr const bool generateReportOnDefaultTextureCreation = true;
    int timesPathTaken[NUM_LOGIC_PATHS] = { 0 };
    std::vector<IJPair> ijPathRecords[NUM_LOGIC_PATHS];
    FilesystemDirectory* debugOutputDir = debugOutputDirectory();
    assert(debugOutputDir);
#endif 


    for (GLsizei i = 0; i < height; i++) {
        for (GLsizei j = 0; j < width; j++) {

            ///////////////////////////////////////////
            //    ~~ LOGIC PATH 1 
            if (((j % 33) <= 16) && 
                ((i % 17 >= 12) && (i % 17 <= 15)) ||
                ((j % 19 >=  8) && (i % 17 <= 13)) &&
                !((i + j) % 21 > 17)) {                   
#ifdef GEN_REPORT
                timesPathTaken[LOGIC_PATH_1] += 1;
                ijPathRecords[LOGIC_PATH_1].push_back(IJPair(i, j));
#endif
                generatedData.push_back(static_cast<uint8_t>(25U + (25U*i)%220U));
                generatedData.push_back(static_cast<uint8_t>(115U + j));
                generatedData.push_back(static_cast<uint8_t>(185U - (i%3U)*20U));
                generatedData.push_back(static_cast<uint8_t>(85U));
            }

            ///////////////////////////////////////////
            //    ~~ LOGIC PATH 2 
            else if ((j > (i - 3)) && (j < (i + 3))) {              
#ifdef GEN_REPORT
                timesPathTaken[LOGIC_PATH_2] += 1;
                ijPathRecords[LOGIC_PATH_2].push_back(IJPair(i,j));
#endif
                generatedData.push_back(static_cast<uint8_t>((i * j * 17)%256)); //B
                generatedData.push_back(static_cast<uint8_t>(151-(j*29%77))); //G
                generatedData.push_back(static_cast<uint8_t>((81U + 2*i/(j+1))));  //R
                generatedData.push_back(static_cast<uint8_t>(56 + (i*j)%156));   //A
            }

            ///////////////////////////////////////////
            //    ~~ LOGIC PATH 3 
            else if ( ((i % 44) < 5)  &&  ((j%11) < 5) ) {          
#ifdef GEN_REPORT
                timesPathTaken[LOGIC_PATH_3] += 1;
                ijPathRecords[LOGIC_PATH_3].push_back(IJPair(i, j));
#endif
                const uint8_t fadeFactor = static_cast<uint8_t>(j % 157);
                generatedData.push_back(static_cast<uint8_t>(233U) - fadeFactor);    //B
                generatedData.push_back(static_cast<uint8_t>(75u) + fadeFactor);  //G
                generatedData.push_back(static_cast<uint8_t>(21U) + fadeFactor);   //R
                generatedData.push_back(static_cast<uint8_t>((7 * i) % 256));  //A
            }

            ///////////////////////////////////////////
            //    ~~ LOGIC PATH 4 
            else if (i > (j + 3)) {
#ifdef GEN_REPORT
                timesPathTaken[LOGIC_PATH_4] += 1;
                ijPathRecords[LOGIC_PATH_4].push_back(IJPair(i, j));
#endif
                const uint8_t fadeFactor = static_cast<uint8_t>(j*i % 89);
                generatedData.push_back(static_cast<uint8_t>(233U) - fadeFactor);    //B
                generatedData.push_back(static_cast<uint8_t>(75u) + fadeFactor);  //G
                generatedData.push_back(static_cast<uint8_t>(21U) + fadeFactor);   //R
                generatedData.push_back(static_cast<uint8_t>((1781 * i) % 254));  //A
            }
            ///////////////////////////////////////////
            //    ~~ LOGIC PATH 5 
            else {
#ifdef GEN_REPORT
                timesPathTaken[LOGIC_PATH_5] += 1;
                ijPathRecords[LOGIC_PATH_5].push_back(IJPair(i, j));
#endif
                const uint8_t fadeFactor = static_cast<uint8_t>(i % 157);
                generatedData.push_back(static_cast<uint8_t>(233U) - fadeFactor);    //B
                generatedData.push_back(static_cast<uint8_t>(75u) + fadeFactor);  //G
                generatedData.push_back(static_cast<uint8_t>(21U) + fadeFactor);   //R
                generatedData.push_back(static_cast<uint8_t>(255U));  //A
            }
        }
    }

#ifdef GEN_REPORT
    constexpr const char* defaultTexGenerationReportFilename = "DefaultTextureGenerationReport_";
    auto reportFilename = debugOutputDir->getNextUniqueFilenameFor(defaultTexGenerationReportFilename);
    
    //Remove any Filesystem delimiter characters ('\\' and '/') 
        //from the end of the filename 
    auto nameStr = reportFilename.string();
    auto nameStringIter = nameStr.cend();
    bool removeChar = true;
    int counter = 0;
    while (removeChar) {
        nameStringIter--;
        if ((*nameStringIter == '\\') ||
            (*nameStringIter == '/')) {
            counter++;
            continue;
        }
        else
            removeChar = false;
    }
    while (counter-- > 0)
        nameStr.pop_back();
    reportFilename = nameStr + std::string(".txt");
    bool ya = !(std::filesystem::exists(reportFilename));
    try {
        std::ofstream fileWriter(reportFilename);
        if (!fileWriter) {
            fprintf(ERRLOG, "Error creating std::ofstream!\n");
            return generatedData;
        }
        else {
            //HERE IS WHERE WE WRITE DATA TO THE FILE
            //Write Some General Information At Top Of The File
            fileWriter << "\n\t\t\tDefault Texture Generation Report\n\n";
            fileWriter << "Width:       " << width << "\n";
            fileWriter << "Height:      " << height << "\n";
            fileWriter << "Total Pairs: " << width * height << "\n";
            fileWriter << "\n\n";
            fileWriter << "Here is the spread of how each pixel was ";
            fileWriter << "assigned to the texture:\n"; 
            
            fileWriter << "   Condition 1: " << std::setw(5) << timesPathTaken[LOGIC_PATH_1] <<
                "                  Condition 2: " << std::setw(5) <<
                timesPathTaken[LOGIC_PATH_2] << "\n";
            fileWriter << "   Condition 3: " << std::setw(5) << timesPathTaken[LOGIC_PATH_3] <<
                "                  Condition 4: " << std::setw(5) <<
                timesPathTaken[LOGIC_PATH_4] << "\n";
            fileWriter << "   Condition 5: " << std::setw(5) << timesPathTaken[LOGIC_PATH_5] <<
                "                  Condition 6: " << std::setw(5) <<
                timesPathTaken[LOGIC_PATH_5] << "\n";
            
            fileWriter << "\n\n\n\n";

            fileWriter << "\n\t\tNow Onto The Data:\n";
            for (int i = 0; i < NUM_LOGIC_PATHS; i++) {
                fileWriter << "\n\n\n";
                fileWriter << "    +----------------------------+\n";
                fileWriter << "    | IJ Pairs For Logic-Path " << (i + 1) << "  |\n";
                fileWriter << "    +----------------------------+\n";

                size_t newlineCounter = 0;
                for (const IJPair& ij : ijPathRecords[i]) {
                    newlineCounter++;
                    fileWriter << ij.to_string();
                    if (((newlineCounter-1U) % 8U) != 7U)
                        fileWriter << " | ";
                    else 
                        fileWriter << "\n";
                }
            }

            fileWriter << "\n\n\n\t\t\tAnd now for the data...\n\n";
            constexpr const int BYTE_GROUP_SIZE = 4;
            int byteGroupCounter = 0; //For formatting bytes into groups
            int groupLineCounter = 0; //For formatting byteGroups so they fit on each line
            fileWriter << "{";
            for (uint8_t dataByte : generatedData) {

                byteGroupCounter = (byteGroupCounter + 1) % BYTE_GROUP_SIZE;
                fileWriter << std::setw(3) << std::to_string(static_cast<unsigned int>(dataByte));
                if (byteGroupCounter != 0)
                    fileWriter << ",";
                else {
                    groupLineCounter = (groupLineCounter + 1) % 12;
                    if (groupLineCounter != 0)
                        fileWriter << "}  {";
                    else
                        fileWriter << "}\n{";
                }
            }
            fileWriter.flush();
            fileWriter.close();
        }
        
    }
    catch (...) {
        fprintf(ERRLOG, "\nError writing file for %s\n", reportFilename.string().c_str());
        return generatedData;
    }
#endif //GEN_REPORT

    return generatedData;
}


ImageData::ImageData() : mDataType_(GL_UNSIGNED_BYTE) {
    mValid_ = true;
    mErrorMessage_ = "";
    mName_ = "DefaultImage_";
    mName_ += std::to_string(NEXT_DEFAULT_IMAGE_NAME++);
    mWidth_ = DEFAULT_IMAGE_WIDTH;
    mHeight_ = DEFAULT_IMAGE_HEIGHT;
    mComponents_ = 4;
    mFormat_ = GL_BGRA;
    mInternalFormat_ = GL_RGBA8;
    
    mData_ = generateImageData(mWidth_, mHeight_);

}
//Upload your own image data. Or just specify a width and height to get
//an all-black image.
ImageData::ImageData(std::string name,
                     GLsizei width,
                     GLsizei height,
                     GLenum format,
                     GLenum internalFormat,
                     std::vector<uint8_t>* data) : mName_(name),
                                                   mWidth_(width),
                                                   mHeight_(height),
                                                   mFormat_(format),
                                                   mInternalFormat_(internalFormat),
                                                   mDataType_(GL_UNSIGNED_BYTE) {
    assert(width > 0);
    assert(height > 0);
    if (data == nullptr) {
        assert(internalFormat == GL_RGBA8); //Others are not yet supported
        assert(format == GL_BGRA);         //Others are not yet supported
        mComponents_ = 4;
        mData_ = generateImageData(mWidth_, mHeight_);
        return;
    }
    else {
        mData_ = *data;
        mComponents_ = mData_.size() / (width * height);
    }

}


//Please ensure the image file has a proper file extension which matches 
//one of the expected formats.Accepted formats currently are: [None] 
ImageData::ImageData(const std::filesystem::path& imageFile) : ImageData(){
    (void)imageFile;
    fprintf(MSGLOG, "\nFile Loading is not yet supported!\n");

}

ImageData::~ImageData() {

}

ImageData::ImageData(const ImageData& that) noexcept {
    mValid_ = that.mValid_;
    mErrorMessage_ = that.mErrorMessage_;
    mName_ = that.mName_;
    mWidth_ = that.mWidth_;
    mHeight_ = that.mHeight_;
    mComponents_ = that.mComponents_;
    mDataType_ = that.mDataType_;
    mFormat_ = that.mFormat_;
    mInternalFormat_ = that.mInternalFormat_;
    mData_.reserve(that.mData_.size());
    mData_ = that.mData_;
}

ImageData::ImageData(ImageData&& that) noexcept {
    mValid_ = that.mValid_;
    mErrorMessage_ = that.mErrorMessage_;
    mName_ = that.mName_;
    mWidth_ = that.mWidth_;
    mHeight_ = that.mHeight_;
    mComponents_ = that.mComponents_;
    mDataType_ = that.mDataType_;
    mFormat_ = that.mFormat_;
    mInternalFormat_ = that.mInternalFormat_;
    mData_.swap(that.mData_);
}

ImageData& ImageData::operator=(const ImageData& that) noexcept {
    if (this != &that) {
        mValid_ = that.mValid_;
        mErrorMessage_ = that.mErrorMessage_;
        mName_ = that.mName_;
        mWidth_ = that.mWidth_;
        mHeight_ = that.mHeight_;
        mComponents_ = that.mComponents_;
        mDataType_ = that.mDataType_;
        mFormat_ = that.mFormat_;
        mInternalFormat_ = that.mInternalFormat_;
        mData_.reserve(that.mData_.size());
        mData_ = that.mData_;
    }
    return *this;
}

ImageData& ImageData::operator=(ImageData&& that) noexcept {
    if (this != &that) {
        mValid_ = that.mValid_;
        mErrorMessage_ = that.mErrorMessage_;
        mName_ = that.mName_;
        mWidth_ = that.mWidth_;
        mHeight_ = that.mHeight_;
        mComponents_ = that.mComponents_;
        mDataType_ = that.mDataType_;
        mFormat_ = that.mFormat_;
        mInternalFormat_ = that.mInternalFormat_;
        mData_.reserve(that.mData_.size());
        mData_.swap(that.mData_);
    }
    return *this;
}
