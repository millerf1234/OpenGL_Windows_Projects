//File:   Vertex.cpp
//Description:  Contains implementations for the classes found in Vertex.h. 
//              These classes are Vertex, FullVertex, ExtendedVertex.
//
//Programmer:   Forrest Miller
//Date:         October 14, 2018

#include "Vertex.h"

//-------------------------------------------------------
//                        VERTEX
//-------------------------------------------------------
Vertex::Vertex(float x) {
	mComponents_[0] = x;
	mComponents_[1] = 0.0f;
	mComponents_[2] = 0.0f;
	mComponents_[3] = 0.0f;
}

Vertex::Vertex(float x, float y) {
	mComponents_[0] = x;
	mComponents_[1] = y;
	mComponents_[2] = 0.0f;
	mComponents_[3] = 0.0f;
}

Vertex::Vertex(float x, float y, float z) {
	mComponents_[0] = x;
	mComponents_[1] = y;
	mComponents_[2] = z;
	mComponents_[3] = 0.0f;
}

Vertex::Vertex(float x, float y, float z, float w) {
	mComponents_[0] = x;
	mComponents_[1] = y;
	mComponents_[2] = z;
	mComponents_[3] = w;
}

Vertex::Vertex(const Vertex& that)  {
	mComponents_ = that.mComponents_;
}

Vertex::Vertex(Vertex&& that) {
	mComponents_ = std::move(that.mComponents_);
}

Vertex::~Vertex() {

}

Vertex& Vertex::operator=(const Vertex& that) {
	if (this != &that) {
		mComponents_ = that.mComponents_;
	}
	return *this;
}

Vertex& Vertex::operator=(Vertex&& that) {
	if (this != &that) {
		mComponents_ = std::move(that.mComponents_);
	}
	return *this;
}

bool Vertex::operator==(const Vertex& other) const {
	if (abs(mComponents_[0] - other.mComponents_[0]) < fpTolerance) {
		if (abs(mComponents_[1] - other.mComponents_[1]) < fpTolerance) {
			if (abs(mComponents_[2] - other.mComponents_[2] < fpTolerance)) {
				if (abs(mComponents_[3] - other.mComponents_[3]) < fpTolerance) {
					return true;
				}
			}
		}
	}
	return false;
}



//-------------------------------------------------------
//                     FULL VERTEX
//-------------------------------------------------------

FullVertex::~FullVertex() {

}

FullVertex::FullVertex(const Vertex& v1, const Vertex& v2) {
	mComponents_[0] = v1.mComponents_[0];
	mComponents_[1] = v1.mComponents_[1];
	mComponents_[2] = v1.mComponents_[2];
	mComponents_[3] = v1.mComponents_[3];
	mComponents_[4] = v2.mComponents_[0];
	mComponents_[5] = v2.mComponents_[1];
	mComponents_[6] = v2.mComponents_[2];
	mComponents_[7] = v2.mComponents_[3];
	mComponents_[8] = 0.0f;
	mComponents_[9] = 0.0f;
	mComponents_[10] = 0.0f;
	mComponents_[11] = 0.0f;
}

FullVertex::FullVertex(const Vertex& v1, const Vertex& v2, const Vertex& v3) {
	mComponents_[0] = v1.mComponents_[0];
	mComponents_[1] = v1.mComponents_[1];
	mComponents_[2] = v1.mComponents_[2];
	mComponents_[3] = v1.mComponents_[3];
	mComponents_[4] = v2.mComponents_[0];
	mComponents_[5] = v2.mComponents_[1];
	mComponents_[6] = v2.mComponents_[2];
	mComponents_[7] = v2.mComponents_[3];
	mComponents_[8] = v3.mComponents_[0];
	mComponents_[9] = v3.mComponents_[1];
	mComponents_[10] = v3.mComponents_[2];
	mComponents_[11] = v3.mComponents_[3];
}

FullVertex::FullVertex(const FullVertex& that) {
	//Just copy the array 
	mComponents_ = that.mComponents_;
	//Or could do it manually:
	//for (int i = 0; i < FULL_VERTEX_SIZE; i++) {
	//	  mComponents_[i] = that.mComponents_[i];
	//}
}

FullVertex::FullVertex(FullVertex&& that) {
	mComponents_ = std::move(that.mComponents_);
}


FullVertex& FullVertex::operator=(const FullVertex& that) {
	if (this != &that) {
		mComponents_ = that.mComponents_;
	}
	return *this;
}

FullVertex& FullVertex::operator=(FullVertex&& that) {
	if (this != &that) {
		mComponents_ = std::move(that.mComponents_);
	}
	return *this;
}

bool FullVertex::operator==(const FullVertex& that) const {
	if (this == &that) { return true; }
	//Loop through and do a component-by-component comparison
	for (int i = 0; i < static_cast<int>(FULL_VERTEX_SIZE); i++) {
		if (abs(mComponents_[i] - that.mComponents_[i]) >= fpTolerance) {
			return false;
		}
	}
	return true;
}


//-------------------------------------------------------
//                       EXTENDED VERTEX
//-------------------------------------------------------

ExtendedVertex::~ExtendedVertex() {
	//fprintf(stderr, "abc\n");
}

ExtendedVertex::ExtendedVertex(const Vertex& v1, const Vertex& v2, const Vertex& v3, const Vertex& v4) {
	mExtendedComponents_[0] = v1.mComponents_[0];
	mExtendedComponents_[1] = v1.mComponents_[1];
	mExtendedComponents_[2] = v1.mComponents_[2];
	mExtendedComponents_[3] = v1.mComponents_[3];
	mExtendedComponents_[4] = v2.mComponents_[0];
	mExtendedComponents_[5] = v2.mComponents_[1];
	mExtendedComponents_[6] = v2.mComponents_[2];
	mExtendedComponents_[7] = v2.mComponents_[3];
	mExtendedComponents_[8] = v3.mComponents_[0];
	mExtendedComponents_[9] = v3.mComponents_[1];
	mExtendedComponents_[10] = v3.mComponents_[2];
	mExtendedComponents_[11] = v3.mComponents_[3];
	mExtendedComponents_[12] = v4.mComponents_[0];
	mExtendedComponents_[13] = v4.mComponents_[1];
	mExtendedComponents_[14] = v4.mComponents_[2];
	mExtendedComponents_[15] = v4.mComponents_[3];
	mExtendedComponents_[16] = 0.0f;
	mExtendedComponents_[17] = 0.0f;
	mExtendedComponents_[18] = 0.0f;
	mExtendedComponents_[19] = 0.0f;
	mExtendedComponents_[20] = 0.0f;
	mExtendedComponents_[21] = 0.0f;
	mExtendedComponents_[22] = 0.0f;
	mExtendedComponents_[23] = 0.0f;
}

ExtendedVertex::ExtendedVertex(const Vertex& v1, const Vertex& v2, const Vertex& v3, const Vertex& v4, const Vertex& v5) {
	mExtendedComponents_[0] = v1.mComponents_[0];
	mExtendedComponents_[1] = v1.mComponents_[1];
	mExtendedComponents_[2] = v1.mComponents_[2];
	mExtendedComponents_[3] = v1.mComponents_[3];
	mExtendedComponents_[4] = v2.mComponents_[0];
	mExtendedComponents_[5] = v2.mComponents_[1];
	mExtendedComponents_[6] = v2.mComponents_[2];
	mExtendedComponents_[7] = v2.mComponents_[3];
	mExtendedComponents_[8] = v3.mComponents_[0];
	mExtendedComponents_[9] = v3.mComponents_[1];
	mExtendedComponents_[10] = v3.mComponents_[2];
	mExtendedComponents_[11] = v3.mComponents_[3];
	mExtendedComponents_[12] = v4.mComponents_[0];
	mExtendedComponents_[13] = v4.mComponents_[1];
	mExtendedComponents_[14] = v4.mComponents_[2];
	mExtendedComponents_[15] = v4.mComponents_[3];
	mExtendedComponents_[16] = v5.mComponents_[0];
	mExtendedComponents_[17] = v5.mComponents_[1];
	mExtendedComponents_[18] = v5.mComponents_[2];
	mExtendedComponents_[19] = v5.mComponents_[3];
	mExtendedComponents_[20] = 0.0f;
	mExtendedComponents_[21] = 0.0f;
	mExtendedComponents_[22] = 0.0f;
	mExtendedComponents_[23] = 0.0f;
}

ExtendedVertex::ExtendedVertex(const Vertex& v1, const Vertex& v2, const Vertex& v3, const Vertex& v4, const Vertex& v5, const Vertex& v6) {
	mExtendedComponents_[0] = v1.mComponents_[0];
	mExtendedComponents_[1] = v1.mComponents_[1];
	mExtendedComponents_[2] = v1.mComponents_[2];
	mExtendedComponents_[3] = v1.mComponents_[3];
	mExtendedComponents_[4] = v2.mComponents_[0];
	mExtendedComponents_[5] = v2.mComponents_[1];
	mExtendedComponents_[6] = v2.mComponents_[2];
	mExtendedComponents_[7] = v2.mComponents_[3];
	mExtendedComponents_[8] = v3.mComponents_[0];
	mExtendedComponents_[9] = v3.mComponents_[1];
	mExtendedComponents_[10] = v3.mComponents_[2];
	mExtendedComponents_[11] = v3.mComponents_[3];
	mExtendedComponents_[12] = v4.mComponents_[0];
	mExtendedComponents_[13] = v4.mComponents_[1];
	mExtendedComponents_[14] = v4.mComponents_[2];
	mExtendedComponents_[15] = v4.mComponents_[3];
	mExtendedComponents_[16] = v5.mComponents_[0];
	mExtendedComponents_[17] = v5.mComponents_[1];
	mExtendedComponents_[18] = v5.mComponents_[2];
	mExtendedComponents_[19] = v5.mComponents_[3];
	mExtendedComponents_[20] = v6.mComponents_[0];
	mExtendedComponents_[21] = v6.mComponents_[0];
	mExtendedComponents_[22] = v6.mComponents_[0];
	mExtendedComponents_[23] = v6.mComponents_[0];
}


ExtendedVertex::ExtendedVertex(const FullVertex& fv, const Vertex& v) {
	mExtendedComponents_[0] = fv.mComponents_[0];
	mExtendedComponents_[1] = fv.mComponents_[1];
	mExtendedComponents_[2] = fv.mComponents_[2];
	mExtendedComponents_[3] = fv.mComponents_[3];
	mExtendedComponents_[4] = fv.mComponents_[4];
	mExtendedComponents_[5] = fv.mComponents_[5];
	mExtendedComponents_[6] = fv.mComponents_[6];
	mExtendedComponents_[7] = fv.mComponents_[7];
	mExtendedComponents_[8] = fv.mComponents_[8];
	mExtendedComponents_[9] = fv.mComponents_[9];
	mExtendedComponents_[10] = fv.mComponents_[10];
	mExtendedComponents_[11] = fv.mComponents_[11];
	mExtendedComponents_[12] = v.mComponents_[0];
	mExtendedComponents_[13] = v.mComponents_[1];
	mExtendedComponents_[14] = v.mComponents_[2];
	mExtendedComponents_[15] = v.mComponents_[3];
	mExtendedComponents_[16] = 0.0f;
	mExtendedComponents_[17] = 0.0f;
	mExtendedComponents_[18] = 0.0f;
	mExtendedComponents_[19] = 0.0f;
	mExtendedComponents_[20] = 0.0f;
	mExtendedComponents_[21] = 0.0f;
	mExtendedComponents_[22] = 0.0f;
	mExtendedComponents_[23] = 0.0f;
}

ExtendedVertex::ExtendedVertex(const FullVertex& fv, const Vertex& v1, const Vertex& v2) {
	mExtendedComponents_[0] = fv.mComponents_[0];
	mExtendedComponents_[1] = fv.mComponents_[1];
	mExtendedComponents_[2] = fv.mComponents_[2];
	mExtendedComponents_[3] = fv.mComponents_[3];
	mExtendedComponents_[4] = fv.mComponents_[4];
	mExtendedComponents_[5] = fv.mComponents_[5];
	mExtendedComponents_[6] = fv.mComponents_[6];
	mExtendedComponents_[7] = fv.mComponents_[7];
	mExtendedComponents_[8] = fv.mComponents_[8];
	mExtendedComponents_[9] = fv.mComponents_[9];
	mExtendedComponents_[10] = fv.mComponents_[10];
	mExtendedComponents_[11] = fv.mComponents_[11];
	mExtendedComponents_[12] = v1.mComponents_[0];
	mExtendedComponents_[13] = v1.mComponents_[1];
	mExtendedComponents_[14] = v1.mComponents_[2];
	mExtendedComponents_[15] = v1.mComponents_[3];
	mExtendedComponents_[16] = v2.mComponents_[0];
	mExtendedComponents_[17] = v2.mComponents_[1];
	mExtendedComponents_[18] = v2.mComponents_[2];
	mExtendedComponents_[19] = v2.mComponents_[3];
	mExtendedComponents_[20] = 0.0f;
	mExtendedComponents_[21] = 0.0f;
	mExtendedComponents_[22] = 0.0f;
	mExtendedComponents_[23] = 0.0f;
}


//Copying:

ExtendedVertex::ExtendedVertex(const ExtendedVertex& that) {
	mExtendedComponents_ = that.mExtendedComponents_;
}

ExtendedVertex::ExtendedVertex(ExtendedVertex&& that) {
	mExtendedComponents_ = std::move(that.mExtendedComponents_);
}

ExtendedVertex& ExtendedVertex::operator=(const ExtendedVertex& that) {
	if (this != &that) {
		mExtendedComponents_ = that.mExtendedComponents_;
	}
	return *this;
}


ExtendedVertex& ExtendedVertex::operator=(ExtendedVertex&& that) {
	if (this != &that) {
		mExtendedComponents_ = std::move(that.mExtendedComponents_);
	}
	return *this;
}

bool ExtendedVertex::operator==(const ExtendedVertex& that) const {
	if (this == &that) { return true; }
	//Loop through and do a component-by-component comparison
	for (int i = 0; i < static_cast<int>(EXTENDED_VERTEX_SIZE); i++) {
		if (abs(mExtendedComponents_[i] - that.mExtendedComponents_[i]) >= fpTolerance) {
			return false;
		}
	}
	return true;
}