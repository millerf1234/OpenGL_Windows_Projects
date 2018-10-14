//Written with the intention of being used with VertexSet. Contains
//definitions for several different types of vertex class, each 
//of a fixed size. 
//
// October 13, 2018
// Forrest Miller

#pragma once

#ifndef VERTEX_H_
#define VERTEX_H_

static constexpr float INITAL_FLOATING_POINT_TOLERANCE = 0.0000001f;
static constexpr size_t VERTEX_SIZE = 4u;
static constexpr size_t FULL_VERTEX_SIZE = 3u*VERTEX_SIZE;
static constexpr size_t EXTENDED_VERTEX_SIZE = 6u*VERTEX_SIZE; 

#include <array>
#include <cmath>

static float fpTolerance = INITAL_FLOATING_POINT_TOLERANCE; //Floating point tolerance (used in comparing equality)

//To simplify logic and reduce unecessary copying, the following two classes are 
//forward declared so that they can be made friends of the Vertex class.
class FullVertex;
class ExtendedVertex;

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

	float& operator[] (int indx) { return mComponents_[indx]; }

	constexpr size_t getSize() { return VERTEX_SIZE; }
	
	//Returns a reference to this object's stored data. Reference will be 
	//invalidated if object is destoryed.
	std::array<float, VERTEX_SIZE>& data() { return mComponents_; }

	static bool setFloatingPointTolerance(float tolerance) { fpTolerance = tolerance; }
	static float getFloatingPointTolerance() { return fpTolerance; }

private:
	std::array<float, VERTEX_SIZE> mComponents_;
	friend class FullVertex;
	friend class ExtendedVertex;

	//Helper functions:
	//void initialize();
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

	float& operator[] (int indx) { return mComponents_[indx]; }
	
	constexpr size_t getSize() const { return EXTENDED_VERTEX_SIZE; }

	std::array<float, EXTENDED_VERTEX_SIZE>& data() { return mComponents_; }

private:
	std::array<float, EXTENDED_VERTEX_SIZE> mComponents_;
	//eventually... Add other parameters

	//Helper functions
	//void initialize();
};


#endif //VERTEX_H_