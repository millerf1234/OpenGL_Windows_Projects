#include "AffineTransformation.h"


namespace ModelInterface {

	AffineTransformation::AffineTransformation() {
		mType = ATType::IDENTITY;
		mTransformationMatrix = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
										  0.0f, 1.0f, 0.0f, 0.0f,
										  0.0f, 0.0f, 1.0f, 0.0f,
										  0.0f, 0.0f, 0.0f, 1.0f);

	}

	AffineTransformation::AffineTransformation(bool makeNull) {
		mType = ATType::NULLMAT;
	}


	AffineTransformation::~AffineTransformation() {




	}


	void AffineTransformation::resetToIdentity() {


	}


	void AffineTransformation::clearTransformationOrder() {
		std::vector<AffineTransformation>().swap(mTransformationOrder);
	}

	void AffineTransformation::rebuildTransformationOrder() {
		clearTransformationOrder();
		if (0u == mCompositionTransforms.size())
			resetToIdentity();
		else if (1u == mCompositionTransforms.size()) {
			mType = mCompositionTransforms.front().type();
			mTransformationMatrix = mCompositionTransforms.front().getMatrix();
			
		}
		else {



		}
	}
} //namespace ModelInterface