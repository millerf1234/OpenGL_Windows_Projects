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
// October 13-21, 2018
// Forrest Miller

#pragma once

#ifndef VERTEX_H_
#define VERTEX_H_

static constexpr float INITAL_FLOATING_POINT_TOLERANCE = 0.0000001f;
static constexpr size_t VERTEX_SIZE = 4u; 
static constexpr size_t PTN_VERTEX_SIZE = 3u + 2u + 3u;
static constexpr size_t FULL_VERTEX_SIZE = 3u*VERTEX_SIZE;
static constexpr size_t EXTENDED_VERTEX_SIZE = 6u*VERTEX_SIZE; 

#include <array>
#include <cmath>

static float fpTolerance = INITAL_FLOATING_POINT_TOLERANCE; //Floating point tolerance (used in comparing equality)

//To simplify logic and reduce unnecessary copying, the following three classes are 
//forward declared so that they can be made friends of the Vertex class.
class PTNVertex;
class FullVertex;
class ExtendedVertex;

//Basic 4-component vertex. Construct with 1-4 floats, any components not specified will 
//be initialized to 0.0f
class Vertex {
public:
	//No default constructors
	Vertex() = delete;
	~Vertex();
	//But a ton of other constructors 
	Vertex(float);
	Vertex(float, float);
	Vertex(float, float, float);
	Vertex(float, float, float, float);
	
	Vertex(const Vertex&);
	/* Vertex(const Vert&, const Vert&);
	Vertex(const Vert&, const Vert&, const Vert&);*/

	Vertex(Vertex&&);

	Vertex& operator=(const Vertex&);
	Vertex& operator=(Vertex&&);

	bool operator==(const Vertex& other) const;
	bool operator!=(const Vertex& other) const;

	float& operator[] (int indx) { return mComponents_[indx]; }

	constexpr size_t getSize() { return VERTEX_SIZE; }
	
	//Returns a reference to this object's stored data. Reference will be 
	//invalidated if object is destoryed.
	std::array<float, VERTEX_SIZE>& data() { return mComponents_; }

	static bool setFloatingPointTolerance(float tolerance) { fpTolerance = tolerance; }
	static float getFloatingPointTolerance() { return fpTolerance; }

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
	PTNVertex();

	~PTNVertex();
	//But a ton of other constructors 

	//Intended to construct a vertex from 3 Position datapoints (x, y, z), two texture coordinate datapoints 
	//(t, n), and 3 normal-vector datapoints (xn, yn, zn)
	PTNVertex(float x, float y, float z, float s, float t,  float xn, float yn, float zn);
	

	PTNVertex(const PTNVertex&);
	/* Vertex(const Vert&, const Vert&);
	Vertex(const Vert&, const Vert&, const Vert&);*/

	PTNVertex(PTNVertex&&);

	PTNVertex& operator=(const PTNVertex&);
	PTNVertex& operator=(PTNVertex&&);

	bool operator==(const PTNVertex& other) const;
	bool operator!=(const PTNVertex& other) const;

	float& operator[] (int indx) { return mComponents_[indx]; }

	constexpr size_t getSize() { return VERTEX_SIZE; }

	//Returns a reference to this object's stored data. Reference will be 
	//invalidated if object is destoryed.
	std::array<float, PTN_VERTEX_SIZE>& data() { return mComponents_; }
	
	//Returns a copy of this PTNVertex's 3 positions as an array
	std::array<float, 3> position() const { return { mComponents_[0], mComponents_[1], mComponents_[2] }; }
	//Returns a copy of this PTNVertex's 2 texture coordiantes as an array
	std::array<float, 2> texCoord() const { return { mComponents_[3], mComponents_[4] }; }
	//Returns a copy of this PTNVertex's 3 normal components as an array
	std::array<float, 3> normal() const { return { mComponents_[5], mComponents_[6], mComponents_[7] }; }

	static bool setFloatingPointTolerance(float tolerance) { fpTolerance = tolerance; }
	static float getFloatingPointTolerance() { return fpTolerance; }

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
	FullVertex(FullVertex&&);

	FullVertex& operator=(const FullVertex&);
	FullVertex& operator=(FullVertex&&);

	bool operator==(const FullVertex& other) const;

	float& operator[] (int indx) { return mComponents_[indx]; }

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
	ExtendedVertex(ExtendedVertex&&);
	ExtendedVertex& operator=(const ExtendedVertex&);
	ExtendedVertex& operator=(ExtendedVertex&&);

	bool operator==(const ExtendedVertex& other) const;

	float& operator[] (int indx) { return mExtendedComponents_[indx]; }
	
	constexpr size_t getSize() const { return EXTENDED_VERTEX_SIZE; }

	std::array<float, EXTENDED_VERTEX_SIZE>& data() { return mExtendedComponents_; }

private:
	std::array<float, EXTENDED_VERTEX_SIZE> mExtendedComponents_; //can't be called mComponents_ because of friendship with class Vertex, which has it's own mComponents_
	//eventually... Add other parameters

	//Helper functions
	//void initialize();
};


#endif //VERTEX_H_