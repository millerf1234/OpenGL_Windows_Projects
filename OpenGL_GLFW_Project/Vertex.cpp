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
Vertex::Vertex(float x) noexcept {
	mComponents_[OFFSET_TO_X] = x;
	mComponents_[OFFSET_TO_Y] = 0.0f;
	mComponents_[OFFSET_TO_Z] = 0.0f;
	mComponents_[OFFSET_TO_W] = 0.0f;
}

Vertex::Vertex(float x, float y) noexcept {
    mComponents_[OFFSET_TO_X] = x;
    mComponents_[OFFSET_TO_Y] = y;
    mComponents_[OFFSET_TO_Z] = 0.0f;
    mComponents_[OFFSET_TO_W] = 0.0f;
}

Vertex::Vertex(float x, float y, float z) noexcept {
    mComponents_[OFFSET_TO_X] = x;
    mComponents_[OFFSET_TO_Y] = y;
    mComponents_[OFFSET_TO_Z] = z;
    mComponents_[OFFSET_TO_W] = 0.0f;
}

Vertex::Vertex(float x, float y, float z, float w) noexcept {
    mComponents_[OFFSET_TO_X] = x;
    mComponents_[OFFSET_TO_Y] = y;
    mComponents_[OFFSET_TO_Z] = z;
    mComponents_[OFFSET_TO_W] = w;
}

Vertex::Vertex(const Vertex& that) noexcept {
	mComponents_ = that.mComponents_; 
}

Vertex::Vertex(Vertex&& that) noexcept {
	mComponents_ = std::move(that.mComponents_);
}

Vertex::~Vertex() noexcept {

}

Vertex& Vertex::operator=(const Vertex& that) noexcept {
	if (this != &that) {
		mComponents_ = that.mComponents_;
	}
	return *this;
}

Vertex& Vertex::operator=(Vertex&& that) noexcept {
	if (this != &that) {
		mComponents_ = std::move(that.mComponents_);
	}
	return *this;
}

bool Vertex::operator==(const Vertex& other) const noexcept {
	if (abs(mComponents_[OFFSET_TO_X] - other.mComponents_[OFFSET_TO_Y]) < fpTolerance) {
		if (abs(mComponents_[OFFSET_TO_Y] - other.mComponents_[OFFSET_TO_Y]) < fpTolerance) {
			if (abs(mComponents_[OFFSET_TO_Z] - other.mComponents_[OFFSET_TO_Z] < fpTolerance)) {
				if (abs(mComponents_[OFFSET_TO_W] - other.mComponents_[OFFSET_TO_W]) < fpTolerance) {
					return true;
				}
			}
		}
	}
	return false;
}

bool Vertex::operator!=(const Vertex& other) const noexcept {
    if (abs(mComponents_[OFFSET_TO_X] - other.mComponents_[OFFSET_TO_Y]) < fpTolerance) {
        if (abs(mComponents_[OFFSET_TO_Y] - other.mComponents_[OFFSET_TO_Y]) < fpTolerance) {
            if (abs(mComponents_[OFFSET_TO_Z] - other.mComponents_[OFFSET_TO_Z] < fpTolerance)) {
                if (abs(mComponents_[OFFSET_TO_W] - other.mComponents_[OFFSET_TO_W]) < fpTolerance) {
					return false;
				}
			}
		}
	}
	return true;
}

//-------------------------------------------------------
//                     PTN VERTEX
//-------------------------------------------------------

//      (Position, Texture, Normal) Vertex

PTNVertex::PTNVertex() noexcept {
    mComponents_[OFFSET_TO_X] = 0.0f;
    mComponents_[OFFSET_TO_Y] = 0.0f;
    mComponents_[OFFSET_TO_Z] = 0.0f;
    mComponents_[OFFSET_TO_S] = 0.0f;
    mComponents_[OFFSET_TO_T] = 0.0f;
    mComponents_[OFFSET_TO_NRML_X] = 0.0f;
    mComponents_[OFFSET_TO_NRML_Y] = 0.0f;
    mComponents_[OFFSET_TO_NRML_Z] = 0.0f;
}


PTNVertex::~PTNVertex() noexcept {

}

PTNVertex::PTNVertex(float x, float y, float z, float s, float t, float xn, float yn, float zn) noexcept {
    mComponents_[OFFSET_TO_X] = x;
    mComponents_[OFFSET_TO_Y] = y;
    mComponents_[OFFSET_TO_Z] = z;
    mComponents_[OFFSET_TO_S] = s;
    mComponents_[OFFSET_TO_T] = t;
    mComponents_[OFFSET_TO_NRML_X] = xn;
    mComponents_[OFFSET_TO_NRML_Y] = yn;
    mComponents_[OFFSET_TO_NRML_Z] = zn;
}


PTNVertex::PTNVertex(const PTNVertex& that) noexcept {
	mComponents_ = that.mComponents_;
}

PTNVertex::PTNVertex(PTNVertex&& that) noexcept {
	mComponents_ = std::move(that.mComponents_);
}

PTNVertex& PTNVertex::operator=(const PTNVertex& that) noexcept {
	if (this != &that) {
		mComponents_ = that.mComponents_;
	}
	return *this;
}

PTNVertex& PTNVertex::operator=(PTNVertex&& that) noexcept {
	if (this != &that) {
		mComponents_ = std::move(that.mComponents_);
	}
	return *this;
}

bool PTNVertex::operator==(const PTNVertex& other) const noexcept {
	if (abs(mComponents_[OFFSET_TO_X] - other.mComponents_[OFFSET_TO_X]) < fpTolerance) {
		if (abs(mComponents_[OFFSET_TO_Y] - other.mComponents_[OFFSET_TO_Y]) < fpTolerance) {
			if (abs(mComponents_[OFFSET_TO_Z] - other.mComponents_[OFFSET_TO_Z] < fpTolerance)) {
				if (abs(mComponents_[OFFSET_TO_S] - other.mComponents_[OFFSET_TO_S]) < fpTolerance) {
					if (abs(mComponents_[OFFSET_TO_T] - other.mComponents_[OFFSET_TO_T]) < fpTolerance) {
						if (abs(mComponents_[OFFSET_TO_NRML_X] - other.mComponents_[OFFSET_TO_NRML_X]) < fpTolerance) {
							if (abs(mComponents_[OFFSET_TO_NRML_Y] - other.mComponents_[OFFSET_TO_NRML_Y] < fpTolerance)) {
								if (abs(mComponents_[OFFSET_TO_NRML_Z] - other.mComponents_[OFFSET_TO_NRML_Z]) < fpTolerance) {
									return true;
								}
							}
						}
					}
				}
			}
		}
	}
	return false;
}

bool PTNVertex::operator!=(const PTNVertex& other) const noexcept {
    if (abs(mComponents_[OFFSET_TO_X] - other.mComponents_[OFFSET_TO_X]) < fpTolerance) {
        if (abs(mComponents_[OFFSET_TO_Y] - other.mComponents_[OFFSET_TO_Y]) < fpTolerance) {
            if (abs(mComponents_[OFFSET_TO_Z] - other.mComponents_[OFFSET_TO_Z] < fpTolerance)) {
                if (abs(mComponents_[OFFSET_TO_S] - other.mComponents_[OFFSET_TO_S]) < fpTolerance) {
                    if (abs(mComponents_[OFFSET_TO_T] - other.mComponents_[OFFSET_TO_T]) < fpTolerance) {
                        if (abs(mComponents_[OFFSET_TO_NRML_X] - other.mComponents_[OFFSET_TO_NRML_X]) < fpTolerance) {
                            if (abs(mComponents_[OFFSET_TO_NRML_Y] - other.mComponents_[OFFSET_TO_NRML_Y] < fpTolerance)) {
                                if (abs(mComponents_[OFFSET_TO_NRML_Z] - other.mComponents_[OFFSET_TO_NRML_Z]) < fpTolerance) {
									return false;
								}
							}
						}
					}
				}
			}
		}
	}
	return true;
}


//-------------------------------------------------------
//                     FULL VERTEX
//-------------------------------------------------------

FullVertex::~FullVertex() {

}

FullVertex::FullVertex(float x0, float x1, float x2, float x3, float x4, float x5, float x6, float x7, float x8, float x9, float x10, float x11) {
	mComponents_[0] = x0;
	mComponents_[1] = x1;
	mComponents_[2] = x2;
	mComponents_[3] = x3;
	mComponents_[4] = x4;
	mComponents_[5] = x5;
	mComponents_[6] = x6;
	mComponents_[7] = x7;
	mComponents_[8] = x8;
	mComponents_[9] = x9;
	mComponents_[10] = x10;
	mComponents_[11] = x11;
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

FullVertex::FullVertex(FullVertex&& that) noexcept {
	mComponents_ = std::move(that.mComponents_);
}


FullVertex& FullVertex::operator=(const FullVertex& that) {
	if (this != &that) {
		mComponents_ = that.mComponents_;
	}
	return *this;
}

FullVertex& FullVertex::operator=(FullVertex&& that) noexcept {
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

ExtendedVertex::ExtendedVertex(ExtendedVertex&& that) noexcept {
	mExtendedComponents_ = std::move(that.mExtendedComponents_);
}

ExtendedVertex& ExtendedVertex::operator=(const ExtendedVertex& that) {
	if (this != &that) {
		mExtendedComponents_ = that.mExtendedComponents_;
	}
	return *this;
}


ExtendedVertex& ExtendedVertex::operator=(ExtendedVertex&& that) noexcept {
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