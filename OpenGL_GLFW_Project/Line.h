//A line is like a face, expect it only has 2 vertices
//
//
// Date:  October 18, 2018
// Programmer:  Forrest Miller
//
//
//Note: This class stores offsets, not actual vertex data values

#pragma once

#ifndef LINE_H_
#define LINE_H_

#include <cstdlib>  //strtoul  (Used to convert numbers in a string to unsigned long 'Offset')
#include <array>    //std::array

#include "Mesh.h"   //Use the types defined within Mesh

namespace AssetLoadingInternal {

	static constexpr const int LINE_VERTICE_COUNT = 2;

	class Line {
	public:
		Line() : mEndpoints_{0u, 0u} { mValidData_ = false; }
		
		//Creates a line from the 'endpoints' parameter, and will decrement each
		//endpoint by 1 if fixIndexing is requested. 
		Line(std::array<Offset, LINE_VERTICE_COUNT> endpoints, bool fixIndexing = false); //Have fixIndexing off/on by default?

		//Constructs a Line object from a line of text. A line will always consist entirly
		//of just position vertices. The second parameter fixIndexing will be true by 
		//default and if left as true, will shift all loaded values down by 1 to match an
		//indexing that begins at 0.
		Line(const char * lineLine, bool fixIndexing = true); 


		~Line() { ; }

		bool operator==(const Line& that) const { return (mEndpoints_ == that.mEndpoints_); }
		bool operator!=(const Line& that) const { return (mEndpoints_ != that.mEndpoints_); }

		//Checks to see if the object contains valid data
		bool dataValid() const { return mValidData_; }

		//Returns a reference to the stored endpoint at the specified index
		Offset& operator[](int indx) { return (mEndpoints_[indx]); }

		//Returns a copy of this object's endpoints
		std::array<Offset, LINE_VERTICE_COUNT> get() const { return mEndpoints_; }

	private:
		std::array<Offset, LINE_VERTICE_COUNT> mEndpoints_;
		bool mValidData_;

		//Helper functions:
		//Checks to see if a string character is between '0' (i.e. 48u in ASCII) and '9' (i.e. 57u in ASCII) (i.e. if it's a number)
		bool isNumber(const char * c) const { return ((*c >= static_cast<char>(48u)) && (*c <= static_cast<char>(57u))); }
		//Checks to see if a chracter is the number '0'  (to make sure a face that might look like "10/0/3" is marked invalid)
		bool isZero(const char * c) const { return (*c == static_cast<char>(48u)); }

	};

} //namespace AssetLoadingInternal

#endif //LINE_H_