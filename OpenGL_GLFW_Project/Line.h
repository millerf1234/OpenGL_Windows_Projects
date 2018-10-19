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

#include "Mesh.h"

namespace AssetLoadingInternal {

	class Line {
	public:
		Line() : mEndpoints_(0, 0) { ; }
		//Creates a line from the 'endpoints' parameter
		Line(std::pair<Offset, Offset> endpoints) : mEndpoints_(endpoints) { ; }
		~Line() { ; }

		bool operator==(const Line& that) const { return (mEndpoints_ == that.mEndpoints_); }

		//std::pair<Offset, Offset> get() const { return mEndpoints_; }
		std::pair<Offset, Offset> get() const { return mEndpoints_; }

	private:
		std::pair<Offset, Offset> mEndpoints_;
	};

} //namespace AssetLoadingInternal

#endif //LINE_H_