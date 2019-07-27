
//Class that interacts with 'stb_image.h'

//Currently this code is more me experimenting with how stb_image reacts to
//various files to get a sense of how my code should approach interfacing with
//it

#pragma once

#ifdef IMAGE_FILE_LOADER_H_
#define IMAGE_FILE_LOADER_H_

#include <filesystem>

class ImageFileLoader {
public:
    ImageFileLoader(std::filesystem::path imgFile);
    ~ImageFileLoader() noexcept;
private:
    class ImageFileLoaderImpl;
    ImageFileLoaderImpl* pImpl_;
};

#endif //IMAGE_FILE_LOADER_H_

