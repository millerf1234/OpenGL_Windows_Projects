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

#include "Mesh.h"   //Use the types defined within Mesh

namespace AssetLoadingInternal {

	class Line {
	public:
		Line() : mEndpoints_{0u, 0u} { ; }
		//Creates a line from the 'endpoints' parameter
		Line(std::array<Offset, 2> endpoints, bool fixIndexing = true) : mEndpoints_(endpoints) { 
			if (fixIndexing) {
				if (mEndpoints_[0u] != 0u) {  //Can't decrement Offset below 0
					mEndpoints_[0u]--;
				}
				if (mEndpoints_[1u] != 0u) {  
					mEndpoints_[1u]--;
				}
			}
		}
		~Line() { ; }

		bool operator==(const Line& that) const { return (mEndpoints_ == that.mEndpoints_); }

		//Returns a reference to the stored endpoint at the specified index
		Offset& operator[](int indx) { return (mEndpoints_[indx]); }

		//Returns a copy of this object's endpoints
		std::array<Offset, 2> get() const { return mEndpoints_; }
		////Returns a pointer to this objects endpoints
		//std::array<Offset, 2>* getEndpoints() { return &mEndpoints_; }

	private:
		std::array<Offset, 2> mEndpoints_;
	};

} //namespace AssetLoadingInternal

#endif //LINE_H_