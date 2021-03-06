
//Created by Forrest Miller on 7/24/2018
//See header file also for templated-function definitions

#include "MathFunctions.h"

namespace MathFunc {

	//This function seems really inefficient since it looks like it has to recreate the twister each time it runs...
	float getRandomInRangef(float min, float max) {
		long long seed;
		if (randomUseCustomSeed) 
			seed = customRandomSeed;
		else 
			seed = std::chrono::high_resolution_clock::now().time_since_epoch().count(); //Gets a number representing the current time
		std::mt19937 mt_rand(static_cast<unsigned int>(seed));
		auto real_rand = std::bind(std::uniform_real_distribution<float>(min, max), mt_rand);
		return real_rand();
	}

	int getRandomInRangei(int min, int max) {
		long long seed;
		if (randomUseCustomSeed) 
			seed = customRandomSeed;
		else 
			seed = std::chrono::high_resolution_clock::now().time_since_epoch().count(); //Gets a number representing the current time
		std::mt19937 mt_rand(static_cast<unsigned int>(seed));
		auto int_rand = std::bind(std::uniform_int_distribution<int>(min, max), mt_rand);
		return int_rand();
	}

	void setCustomRandomSeed(const long long seed) {
		randomUseCustomSeed = true;
		customRandomSeed = seed;
	}

	void unsetCustomRandomSeed() {
		randomUseCustomSeed = false;
		customRandomSeed = 0ll;
	}


} //namespace MathFunc