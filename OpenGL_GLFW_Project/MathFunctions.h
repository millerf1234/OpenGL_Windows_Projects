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

#include "FloatingPointTolerance.h"  //for constant FP_TOLERANCE for use with floating point calculations
#include "ProjectSetup.h" //for glm

namespace MathFunc {

	//Computes a 3x3 rotation matrix from 3 floating-point parameters. This matrix is vulnerable to 
	//Gimble-Lock. 
	static inline glm::mat3x3 computeRotationMatrix3x3(float head, float pitch, float roll) {
		return glm::mat3x3( 
			cos(roll)*cos(head)-sin(roll)*sin(pitch)*sin(head),
			sin(roll)*cos(head)+cos(roll)*sin(pitch)*sin(head),
			               -cos(pitch)*sin(head),
			                                                      -sin(roll)*cos(pitch),
			                                                       cos(roll)*cos(pitch),
			                                                            sin(pitch),
			                                                                                 cos(roll)*sin(head)+sin(roll)*sin(pitch)*cos(head),
																                             sin(roll)*sin(head)-cos(roll)*sin(pitch)*cos(head),
		                                                                                                       cos(pitch)*cos(head)	                                                                                                               
		);
	}

	//Computes a 4x4 rotation matrix from 3 floating-point parameters. This matrix is vulnerable to 
	//Gimble-Lock
	static inline glm::mat4x4 computeRotationMatrix4x4(float head, float pitch, float roll) {
		return glm::mat4x4( 
			cos(roll)*cos(head)-sin(roll)*sin(pitch)*sin(head),
			sin(roll)*cos(head)+cos(roll)*sin(pitch)*sin(head),
			               -cos(pitch)*sin(head),
			                        0.0f,
			                                                      -sin(roll)*cos(pitch),
			                                                       cos(roll)*cos(pitch),
			                                                            sin(pitch),
			                                                                0.0f,
			                                                                                 cos(roll)*sin(head)+sin(roll)*sin(pitch)*cos(head),
																                             sin(roll)*sin(head)-cos(roll)*sin(pitch)*cos(head),
		                                                                                                       cos(pitch)*cos(head),
			                                                                                                             0.0f,
			                                                                                                                                        0.0f,
				                                                                                                                                    0.0f,
				                                                                                                                                    0.0f,
				                                                                                                                                    1.0f
		);
	}


	//Random number generation
	//   (Future Idea: Use function pointers to swap out random number generation algorithms). Have a static variable that dictates which rand generation method to use.
	//   Include the option to toggle re-seeding the random number generation
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