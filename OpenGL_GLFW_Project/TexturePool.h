//This is a class that handles allocating multiple textures at once from OpenGL and keeping track
//of them.

#pragma once
#ifndef TEXTURE_POOL_H_
#define TEXTURE_POOL_H_

#include <memory>
#include <vector>
#include <tuple>
#include "Texture.h"

typedef std::pair<const char *, GLuint> texID;  //hmm this isn't right.


class TexturePool {
public:
	TexturePool();
	~TexturePool();

	void loadTextures(const std::vector<texID> textureFiles);



private:

	//Array of 

};



#endif //TEXTURE_POOL_H_