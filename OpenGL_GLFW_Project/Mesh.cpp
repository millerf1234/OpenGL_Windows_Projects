#include "Mesh.h"



Mesh::Mesh() {

}


Mesh::~Mesh() {

}


DatapointCount Mesh::getMeshSizeBeyondOffset(Offset offset) const {
	if (offset >= mMeshSize_) {
		return 0;
	}
	else {
		return (mMeshSize_ - offset);
	}
}