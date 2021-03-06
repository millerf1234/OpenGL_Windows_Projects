//Just a test before I write a better/more-robust version.
//It turns out this is the only working/good version currently (as of November 11, 2018)
//of the several LighsourceBody code attempts I have started

#pragma once

#include <array>
#include <vector>

#include "GlobalIncludes.h"
#include "GlobalConstants.h"

//const float offsetMultiplier = (2.0 * PI) / (float(mPOLYGON_CORNERS_));

class LightEmitterSource final{
public:
	LightEmitterSource(std::array<float, 3u> pos, std::array<float, 3u> col,
		GLuint sides = 12u, GLuint iterations = 24u, float baseGrowth = 0.75f,
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
	

	std::vector<float> getData() { return mData_; }

	std::vector<GLuint> getElemOrder() { return mElemOrder_; }


private:
	GLuint mSides_;
	GLuint mIterations_;
	float mBaseGrowth_;
	float mIncrementalGrowth_;
	std::array<float, 3u> mPosition_;
	std::array<float, 3u> mColor_;

	std::vector<float> mData_;
	std::vector<GLuint> mElemOrder_;

	void build() {
		//Clear existing data
		mData_.clear();
		mData_.reserve((3u + 3u)*(mSides_*mIterations_ + 1u));
		
		//Add the center point
		addVertex(mPosition_, mColor_);

		//float currentRadius = 0.0f;
		float offsetMultiplier = ((2.0f * PI) / mSides_);
		for (GLuint i = 0; i < mIterations_; i++) {
			float growth = (mBaseGrowth_ * (powf(mIncrementalGrowth_, static_cast<float>(i))));
			for (GLuint j = 0; j < mSides_; j++) {
				std::array<float, 3u> pos = mPosition_;
				pos[0] += growth * cosf(offsetMultiplier * static_cast<float>(j));
				pos[1] += growth * sinf(offsetMultiplier * static_cast<float>(j));

				addVertex(pos, computeColor(growth));
				//fprintf(MSGLOG, "\nAdded position %f, %f, %f\n", pos[0], pos[1], pos[2]);
			}
		}
	}

	void generateElementOrder() {

		//clear existing data
		mElemOrder_.clear();
		mElemOrder_.reserve(2u*(mSides_ * 3u) + ((mIterations_ - 1u) * (2u * mSides_))); //This is probably too big...

		GLuint vertCounter = 1u;

		//Create the inner triangles
		for (GLuint i = 0u; i < mSides_; i++) {
			mElemOrder_.push_back(0u);
			mElemOrder_.push_back(static_cast<unsigned int>(vertCounter++));
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
		GLuint firstOuterIndex = vertCounter;
		for (GLuint i = 0u; i < (mIterations_ - 1u); i++) {
			GLuint currentOuterLayerStartingIndex = firstOuterIndex + (mSides_ * i);
			for (GLuint j = 0u; j < mSides_; j++) {
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
		mData_.push_back(pos[0]);  //x
		mData_.push_back(pos[1]);  //y
		mData_.push_back(pos[2]);  //z

		mData_.push_back(color[0]); //r
		mData_.push_back(color[1]); //g
		mData_.push_back(color[2]); //b
	}

	std::array<float, 3> computeColor(float radius) {
		std::array<float, 3> vertColor = mColor_;
		float intensity = 1.0f / exp(3.94f * radius);
		vertColor[0] *= intensity;
		vertColor[1] *= intensity;
		vertColor[2] *= intensity;

		return vertColor;
	}

};