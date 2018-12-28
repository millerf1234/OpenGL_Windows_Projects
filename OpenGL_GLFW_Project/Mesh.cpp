#include "Mesh.h"

#ifndef MESH_CLASS_IS_INCOMPLETE_

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

#endif //MESH_CLASS_IS_INCOMPLETE_