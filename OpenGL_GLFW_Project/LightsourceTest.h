//Just a test before I write a better/more-robust version

#pragma once

#include <array>
#include <vector>
#include "ProjectSetup.h"
#include "ProjectConstants.h"

//const float offsetMultiplier = (2.0 * PI) / (float(mPOLYGON_CORNERS_));

class LightEmitterSource final{
public:
	LightEmitterSource(std::array<float, 3> pos, std::array<float, 3> col,
		size_t sides = 12u, size_t iterations = 24u, float baseGrowth = 0.75f,
		float incrementalGrowth = 1.1f) {
		mSides_ = (sides > 3u) ? sides : 3u;
		mIterations_ = iterations;
		mPosition_ = pos;
		mColor_ = col;
		mBaseGrowth_ = baseGrowth;
		mIncrementalGrowth_ = incrementalGrowth;
		build();
		generateElementOrder();
	}
	LightEmitterSource(float x, float y, float z, float r, float g, float b, float size = 1.0f) {

	}

	std::vector<float> getData() { return mData_; }

	std::vector<GLuint> getElemOrder() { return mElemOrder_; }


private:
	size_t mSides_;
	size_t mIterations_;
	float mBaseGrowth_;
	float mIncrementalGrowth_;
	std::array<float, 3> mPosition_;
	std::array<float, 3> mColor_;

	std::vector<float> mData_;
	std::vector<GLuint> mElemOrder_;

	void build() {
		//Clear existing data
		mData_.clear();
		mData_.reserve((3u + 3u)*mSides_*mIterations_);
		
		//Add the center point
		addVertex(mPosition_, mColor_);

		//float currentRadius = 0.0f;
		float offsetMultiplier = ((2.0f * PI) / mSides_);
		for (size_t i = 0; i < mIterations_; i++) {
			float growth = (mBaseGrowth_ * (powf(mIncrementalGrowth_, static_cast<float>(i))));
			for (size_t j = 0; j < mSides_; j++) {
				std::array<float, 3> pos = mPosition_;
				pos[0] += growth * cosf(offsetMultiplier * static_cast<float>(j));
				pos[1] += growth * sinf(offsetMultiplier * static_cast<float>(j));

				addVertex(pos, computeColor(growth));
				fprintf(MSGLOG, "\nAdded position %f, %f, %f\n", pos[0], pos[1], pos[2]);
			}
		}
	}

	void generateElementOrder() {

		//Add some padding to the end of the Vertex data in case I accidentally add too many elements here!
		for (int i = 0; i < 45; i++) {
			mData_.push_back(0.75f);
			mData_.push_back(-0.75f);
			mData_.push_back(0.2f);
		}
		//clear existing data
		mElemOrder_.clear();
		mElemOrder_.reserve(2u*(mSides_ * 3u) + ((mIterations_ - 1u) * (2u * mSides_))); //This is probably too big...

		size_t vertCounter = 1u;

		//Create the inner triangles
		for (size_t i = 0u; i < mSides_; i++) {
			mElemOrder_.push_back(0u);
			mElemOrder_.push_back(vertCounter++);
			if (i != (mSides_ - 1u)) {
				mElemOrder_.push_back(vertCounter);
			}
			else {
				mElemOrder_.push_back(1u);
			}
		}
		if (mIterations_ < 2u) {
			return;
		}
		//Now create the outer layers
		size_t firstOuterIndex = vertCounter;
		for (size_t i = 0u; i < (mIterations_ - 1u); i++) {
			size_t currentOuterLayerStartingIndex = firstOuterIndex + (mSides_ * i);
			for (size_t j = 0u; j < mSides_; j++) {
				mElemOrder_.push_back(currentOuterLayerStartingIndex);
				mElemOrder_.push_back(currentOuterLayerStartingIndex - mSides_);
				if (j != (mSides_ - 1u)) {
					mElemOrder_.push_back(currentOuterLayerStartingIndex + 1u);
				}
				else {
					mElemOrder_.push_back(firstOuterIndex + (mSides_ * i));
				}
				mElemOrder_.push_back(currentOuterLayerStartingIndex - mSides_);
				if (j != (mSides_ - 1u)) {
					mElemOrder_.push_back(currentOuterLayerStartingIndex - mSides_ + 1u);
					currentOuterLayerStartingIndex++;
					mElemOrder_.push_back(currentOuterLayerStartingIndex);
				}
				else {
					mElemOrder_.push_back(firstOuterIndex + (mSides_ * (i - 1u)));
					mElemOrder_.push_back(currentOuterLayerStartingIndex - mSides_ + 1u);
					currentOuterLayerStartingIndex++;
				}
			}
			//mElemOrder_.push_back(mSides_ + 1u + (mSides_ * (i-1u)));
			//mElemOrder_.push_back(mSides_ + 1u + (mSides_ * (i));
			//mElemOrder_.push_back(mSides_ + 2u + (mSides_ * (i - 1u)));

		}
	}

	void addVertex(std::array<float, 3> pos, std::array<float, 3> color) {
		mData_.push_back(pos[0]);
		mData_.push_back(pos[1]);
		mData_.push_back(pos[2]);

		mData_.push_back(color[0]);
		mData_.push_back(color[1]);
		mData_.push_back(color[2]);
	}

	std::array<float, 3> computeColor(float radius) {
		std::array<float, 3> vertColor = mColor_;
		float intensity = 5.0f / exp(30.94f * radius);
		vertColor[0] *= intensity;
		vertColor[1] *= intensity;
		vertColor[2] *= intensity;

		return vertColor;
	}

};