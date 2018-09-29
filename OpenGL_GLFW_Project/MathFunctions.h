// Header file containing some basic math functions. Having these functions all reside in the same file
// allows for the avoidance of many different pieces of the project from having to implement their own
// versions of these functions and leads to less bugs. 
//
//Created by Forrest Miller on July 24, 2018
//This file has been edited continuously though during development, so most of the code here was written in the months following that date.

#pragma once


#ifndef FSM_MATH_FUNCTIONS_H_  //FSM is the author's initials
#define FSM_MATH_FUNCTIONS_H_ 

#include <chrono> //For system clock for random seed
#include <functional> //For std::bind
#include <random>




namespace MathFunc {

	//Random number generation
	//   (Future Idea: Use function pointers to swap out random number generation algorithms). Have a static variable that dictates which rand generation method to use.
	//   Include the option to toggle seeding random number generation
	float getRandomInRangef(float, float);
	int getRandomInRangei(int, int);



	template <typename T> int sgn(const T val) {
		return (T(0) < val) - (val < T(0));
	}




	static bool randomUseCustomSeed = false;
	static long long customRandomSeed = 0ll;

	void setCustomRandomSeed(const long long seed);
	void unsetCustomRandomSeed();
	

} //namespace MathFunc

#endif //FSM_MATH_FUNCTIONS_H_