// Updated Description:
//	This file contains the definitions for several classes used to represent
//  vertices consisting of floating point data components. There are several 
//  different types of vertices, each of a pre-determined fixed size. The idea
//  of keeping them as a fixed size is to allow for them to be stored contiguously
//  in memory. They also have equality operators defined for their types to help
//  with determining/eliminating duplicate vertices (to help with a 'DrawElements' 
//  render implementation).
//  
//  Currently the types defined in this class and the number of components they hold
//  are as follows:
//          Type Name     |          Components
//   -------------------------------------------------------
//           Vertex       |              4
//          PTNVertex     |              8
//         FullVertex     |             12
//       ExtendedVertex   |             24
//
//	 Notes:
//       - (Aside from PTNVertex) There are no assumptions/restictions made
//          by this class regarding the layout/ordering of the data, or if 
//          even all components are used to hold valid data within a vertex.
//          It is left up to any code using this class to keep track of the 
//          locations and meanings of data stored within these types.
//         (For PTNVertex) It is assumed that the components are 3 position, 
//          2 texture and 3 normal and that they follow that layout order.
//
//		  - Any constructor that requires less data to be provided than could 
//           be stored within a vertex of its respective type will set all of 
//           its extra components to 0.0f
//
//        - This class has the option to have its floating point tolerance 
//           tweaked to be a value seperate from the global constant FP_TOLERANCE. 
//           This value is a static value shared across all instances of this class,
//           so be aware of potential side-effects when modifying the 
//
// October 13-21, 2018
// Forrest Miller

#pragma once

#ifndef VERTEX_H_
#define VERTEX_H_

#include <array>
#include <cmath>

#include "FloatingPointTolerance.h"

static constexpr const size_t OFFSET_TO_X = 0ULL;
static constexpr const size_t OFFSET_TO_Y = 1ULL;
static constexpr const size_t OFFSET_TO_Z = 2ULL;
static constexpr const size_t OFFSET_TO_W = 3ULL; //Note: Offset to W and S are intended to be same value
static constexpr const size_t OFFSET_TO_S = 3ULL;
static constexpr const size_t OFFSET_TO_T = 4ULL;
static constexpr const size_t OFFSET_TO_NRML_X = 5ULL;
static constexpr const size_t OFFSET_TO_NRML_Y = 6ULL;
static constexpr const size_t OFFSET_TO_NRML_Z = 7ULL;

static constexpr const size_t VERTEX_SIZE = 4u; 
static constexpr const size_t PTN_VERTEX_SIZE = 3u + 2u + 3u;
static constexpr const size_t FULL_VERTEX_SIZE = 3u*VERTEX_SIZE;
static constexpr const size_t EXTENDED_VERTEX_SIZE = 6u*VERTEX_SIZE; 

//fpTolerance is set 
static float fpTolerance = FP_TOLERANCE; //Floating point tolerance (used in comparing equality)

//Forward declare all four Vertex classes to allow them to reference each-other
class Vertex;
class PTNVertex;
class FullVertex;
class ExtendedVertex;



//Basic 4-component vertex. Construct with 1-4 floats, any components not specified will 
//be initialized to 0.0f
class Vertex {
public:
	//No default constructors
	Vertex() = delete;
	~Vertex() noexcept;
	//But a ton of other constructors 
	Vertex(float) noexcept;
	Vertex(float, float) noexcept;
	Vertex(float, float, float) noexcept;
	Vertex(float, float, float, float) noexcept;
	
	Vertex(const Vertex&) noexcept;
	/* Vertex(const Vert&, const Vert&);
	Vertex(const Vert&, const Vert&, const Vert&);*/

	Vertex(Vertex&&) noexcept;

	Vertex& operator=(const Vertex&) noexcept;
	Vertex& operator=(Vertex&&) noexcept;

	bool operator==(const Vertex& other) const noexcept;
	bool operator!=(const Vertex& other) const noexcept;

    
	float& operator[] (size_t indx) { return mComponents_[indx]; }
    
    float xVal() const noexcept { return mComponents_[OFFSET_TO_X]; }
    float& xRef() noexcept { return mComponents_[OFFSET_TO_X]; }
    float yVal() const noexcept { return mComponents_[OFFSET_TO_Y]; }
    float& yRef() noexcept { return mComponents_[OFFSET_TO_Y]; }
    float zVal() const noexcept { return mComponents_[OFFSET_TO_Z]; }
    float& zRef() noexcept { return mComponents_[OFFSET_TO_Z]; }

	constexpr size_t getSize() const noexcept { return VERTEX_SIZE; }
	
	//Returns a reference to this object's stored data. Reference will be 
	//invalidated if object is destoryed.
	std::array<float, VERTEX_SIZE>& data() noexcept { return mComponents_; }

	static bool setFloatingPointTolerance(float tolerance) noexcept { fpTolerance = tolerance; }
	static float getFloatingPointTolerance() noexcept { return fpTolerance; }

private:
	std::array<float, VERTEX_SIZE> mComponents_; 
    
    friend class PTNVertex;
	friend class FullVertex;
	friend class ExtendedVertex;

	//Helper functions:
	//void initialize();
};


//A PTNVertex is designed to be just the right size to contain data loaded from a typical '.obj' file. 
//PTN is an abreviation for 'Position, Texture, Normal', which are expected to be of size 3-2-3 components.
class PTNVertex {
public:
	//Default constructor. Sets all components to 0.0f
	PTNVertex() noexcept;

	~PTNVertex() noexcept;
	//But a ton of other constructors 

	//Intended to construct a vertex from 3 Position datapoints (x, y, z), two texture coordinate datapoints 
	//(t, n), and 3 normal-vector datapoints (xn, yn, zn)
	PTNVertex(float x, float y, float z, float s, float t,  float xn, float yn, float zn) noexcept;
	

	PTNVertex(const PTNVertex&) noexcept;
	/* Vertex(const Vert&, const Vert&);
	Vertex(const Vert&, const Vert&, const Vert&);*/

	PTNVertex(PTNVertex&&) noexcept;

	PTNVertex& operator=(const PTNVertex&) noexcept;
	PTNVertex& operator=(PTNVertex&&) noexcept;

	bool operator==(const PTNVertex& other) const noexcept;
	bool operator!=(const PTNVertex& other) const noexcept;


	float& operator[] (size_t indx) { return mComponents_[indx]; }


    float xVal() const noexcept { return mComponents_[OFFSET_TO_X]; }
    float& xRef() noexcept { return mComponents_[OFFSET_TO_X]; }
    float yVal() const noexcept { return mComponents_[OFFSET_TO_Y]; }
    float& yRef() noexcept { return mComponents_[OFFSET_TO_Y]; }
    float zVal() const noexcept { return mComponents_[OFFSET_TO_Z]; }
    float& zRef() noexcept { return mComponents_[OFFSET_TO_Z]; }
    float sVal() const noexcept { return mComponents_[OFFSET_TO_S]; }
    float& sRef() noexcept { return mComponents_[OFFSET_TO_S]; }
    float tVal() const noexcept { return mComponents_[OFFSET_TO_T]; }
    float& tRef() noexcept { return mComponents_[OFFSET_TO_T]; }
    float nxVal() const noexcept { return mComponents_[OFFSET_TO_NRML_X]; }
    float& nxRef() noexcept { return mComponents_[OFFSET_TO_NRML_X]; }
    float nyVal() const noexcept { return mComponents_[OFFSET_TO_NRML_Y]; }
    float& nyRef() noexcept { return mComponents_[OFFSET_TO_NRML_Y]; }
    float nzVal() const noexcept { return mComponents_[OFFSET_TO_NRML_Z]; }
    float& nzRef() noexcept { return mComponents_[OFFSET_TO_NRML_Z]; }


    constexpr size_t getSize() const noexcept { return VERTEX_SIZE; }

	//Returns a reference to this object's stored data. Reference will be 
	//invalidated if object is destoryed.
	std::array<float, PTN_VERTEX_SIZE>& data() noexcept { return mComponents_; }
	
	//Returns a copy of this PTNVertex's 3 positions as an array
	std::array<float, 3> position() const noexcept { return { mComponents_[OFFSET_TO_X], mComponents_[OFFSET_TO_Y], mComponents_[OFFSET_TO_Z] }; }
	//Returns a copy of this PTNVertex's 2 texture coordiantes as an array
	std::array<float, 2> texCoord() const noexcept { return { mComponents_[OFFSET_TO_S], mComponents_[OFFSET_TO_T] }; }
	//Returns a copy of this PTNVertex's 3 normal components as an array
	std::array<float, 3> normal() const noexcept { return { mComponents_[OFFSET_TO_NRML_X], mComponents_[OFFSET_TO_NRML_Y], mComponents_[OFFSET_TO_NRML_Z] }; }

	static bool setFloatingPointTolerance(float tolerance) noexcept { fpTolerance = tolerance; }
	static float getFloatingPointTolerance() noexcept { return fpTolerance; }

private:
	std::array<float, PTN_VERTEX_SIZE> mComponents_;
	friend class FullVertex;
	friend class ExtendedVertex;

};
 

//A FullVertex takes up more storage space than just a plain Vertex (3x as much), and 
//is intended to be used to store several different types of information (position,
//normal, tex, etc..). 
class FullVertex {
public:
	//No default constructors
	FullVertex() = delete;
	~FullVertex();
	
	//But a ton of other constructors 

	//Constructed from 12 floating point values
	FullVertex(float, float, float, float, float, float, float, float, float, float, float, float);
	//FullVertex(const PTNVertex&, float, float, float, float); //Not needed?
	//FullVertex(const PTNVertex&, const Vertex&);              //Not needed?
	FullVertex(const Vertex&, const Vertex&);
	FullVertex(const Vertex&, const Vertex&, const Vertex&);

	FullVertex(const FullVertex&);
	FullVertex(FullVertex&&) noexcept;

	FullVertex& operator=(const FullVertex&);
	FullVertex& operator=(FullVertex&&) noexcept;

	bool operator==(const FullVertex& other) const;

	float& operator[] (size_t indx) { return mComponents_[indx]; }

	constexpr size_t getSize() { return FULL_VERTEX_SIZE; }

	std::array<float, FULL_VERTEX_SIZE>& data() { return mComponents_; }

private:
	std::array<float, FULL_VERTEX_SIZE> mComponents_;
	friend class ExtendedVertex; //Allow ExtendedVertex access to this class's internals
	//eventually... Add other parameters

	//Helper functions
	//void initialize();
};



//An ExtendedVertex is intended to be used to store a large amount of per-vertex information.
class ExtendedVertex {
public:
	ExtendedVertex() = delete;
	~ExtendedVertex();

	//But a ton of other constructors 
	ExtendedVertex(const Vertex&, const Vertex&, const Vertex&, const Vertex&);
	ExtendedVertex(const Vertex&, const Vertex&, const Vertex&, const Vertex&, const Vertex&);
	ExtendedVertex(const Vertex&, const Vertex&, const Vertex&, const Vertex&, const Vertex&, const Vertex&);
	
	ExtendedVertex(const FullVertex&, const Vertex&);
	ExtendedVertex(const FullVertex&, const Vertex&, const Vertex&);

	//Copying:
	ExtendedVertex(const ExtendedVertex&);
	ExtendedVertex(ExtendedVertex&&) noexcept;
	ExtendedVertex& operator=(const ExtendedVertex&);
	ExtendedVertex& operator=(ExtendedVertex&&) noexcept;

	bool operator==(const ExtendedVertex& other) const;

	float& operator[] (size_t indx) { return mExtendedComponents_[indx]; }
	
	constexpr size_t getSize() const { return EXTENDED_VERTEX_SIZE; }

	std::array<float, EXTENDED_VERTEX_SIZE>& data() { return mExtendedComponents_; }

private:
	std::array<float, EXTENDED_VERTEX_SIZE> mExtendedComponents_; //can't be called mComponents_ because of friendship with class Vertex, which has it's own mComponents_
	//eventually... Add other parameters

	//Helper functions
	//void initialize();
};


#endif //VERTEX_H_