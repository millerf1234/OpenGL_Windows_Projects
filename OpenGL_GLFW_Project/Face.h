//Class to represent a face loaded from an '.obj' file (and perhaps 
//this class will be used for other reasons as well in the future)
//
//  Programmer:    Forrest Miller
//  Date:          October 18, 2018

//Note: This class stores offsets, not actual vertex data values

#pragma once

#ifndef FACE_H_
#define FACE_H_

#include <cstdlib>

#include "Mesh.h"

namespace AssetLoadingInternal {
	//Dictates how the '==' operator evaluates invalid faces
	static constexpr bool INVALID_FACES_HAVE_EQUIVALENCE = true;

	static constexpr const int QUAD_VERTICE_COUNT = 4;
	static constexpr const int TRIANGLE_VERTICE_COUNT = 3;
	class Face {
		//Class that represents the component of a face (such as position, normal or tex coord). There are
		//up to four vertices per face if the input mesh uses quads.
		class FaceComponent {
		public:
			FaceComponent() : mVertIndices_(0, 0, 0, 0) { ; }
			FaceComponent(Offset off1, Offset off2, Offset off3) : mVertIndices_(off1, off2, off3, 0) { ; }
			FaceComponent(Offset off1, Offset off2, Offset off3, Offset off4) : mVertIndices_(off1, off2, off3, off4) { ; }
			~FaceComponent();

			//std::tuple<Offset, Offset, Offset, Offset> get() const { return mVertIndices_; }
			auto& get() const { return mVertIndices_; } //Gonna test out 'auto'

			//Compares indices for equality
			bool operator==(const FaceComponent& that) const { return (mVertIndices_ == that.mVertIndices_); }
			//Compares indices for inequality
			bool operator!=(const FaceComponent& that) const { return (!(mVertIndices_ == that.mVertIndices_)); }

			//Compares just the first 3 indices of two FaceComponent objects
			bool compareAsTriangles(const FaceComponent& that) const {
				bool equality = (
					(std::get<0>(mVertIndices_) == std::get<0>(that.mVertIndices_)) &&
					(std::get<1>(mVertIndices_) == std::get<1>(that.mVertIndices_)) &&
					(std::get<2>(mVertIndices_) == std::get<2>(that.mVertIndices_)));
				return equality;
			}

		private:
			std::tuple<Offset, Offset, Offset, Offset> mVertIndices_;
		};


	public:
		//Constructs a face with invalid data. Data should not
		//attempt to be accessed, object will issue a warning.
		Face();

		~Face();

		/*  Note: Since the '.obj' standard allows for faces to be negative numbers instead of
				  positive offsets into a list, and since this object will have no way of knowing
				  where to start counting backwards from in the list of vertices, this constructor
				  will choke if it encounters negative numbers. Somewhere above this class the case
				  of negative face offsets will need to be handled.*/
				  //Constructs a face from an entire line of input, including the 'f'. Failure to include 
				  //the 'f' will result in a failure to parse the line and an invalid face.
				  //Indexing of '.obj' files begins at 1, but OpenGL expects elements to begin at 0. Thus
				  //this class's constructor will automatically perform the conversion unless specifically 
				  //asked not to. Please don't pass in a nullptr for the line of text.
		Face(const char * faceLine, bool fixIndexing = true);


		//Creates a face consisting of entirly just 3 positions (no texCoords/normals). 
		//Indexing of '.obj' files begins at 1, but OpenGL expects elements to begin at 0. Thus
		//this class's constructor will automatically perform the conversion unless specifically 
		//asked not to.
		Face(std::array<Offset, 3> positions, bool fixIndexing = true);

		//Creates a face consisting of 4 positions (no texCoords/normals). Please note that vertex
		//indexing of '.obj' files begins at 1, but OpenGL expects elements to begin at 0. Thus
		//this class's constructor will automatically perform the conversion unless specifically 
		//asked not to.
		Face(std::array<Offset, 4> positions, bool fixIndexing = true);

		//Creates a face consisting of 3 positions and 3 texture coordinates.  Please note that vertex
		//indexing of '.obj' files begins at 1, but OpenGL expects elements to begin at 0. Thus
		//this class's constructor will automatically perform the conversion unless specifically 
		//asked not to.
		Face(std::array<Offset, 3> positions, std::array<Offset, 3> texCoords, bool fixIndexing = true);

		//Creates a face consisting of 4 positions and 4 texture coordinates.  Please note that vertex
		//indexing of '.obj' files begins at 1, but OpenGL expects elements to begin at 0. Thus
		//this class's constructor will automatically perform the conversion unless specifically 
		//asked not to.
		Face(std::array<Offset, 4> positions, std::array<Offset, 4> texCoords, bool fixIndexing = true);

		//This one is a bit weird. To distinguish between texutre and normal parameters, if constructing
		//a face with just positions and normals, then an extra encapsulating array is used. Please note
		//that vertex indexing of '.obj' files begins at 1, but OpenGL expects elements to begin at 0. Thus
		//this class's constructor will automatically perform the conversion unless specifically 
		//asked not to.
		Face(std::array<std::array<Offset, 3>, 2> positionsAndNormals, bool fixIndexing = true);

		//This one is a bit weird. To distinguish between texutre and normal parameters, if constructing
		//a face with just positions and normals, then an extra encapsulating array is used. Please note
		//that vertex indexing of '.obj' files begins at 1, but OpenGL expects elements to begin at 0. Thus
		//this class's constructor will automatically perform the conversion unless specifically 
		//asked not to.
		Face(std::array<std::array<Offset, 4>, 2> positionsAndNormals, bool fixIndexing = true);

		//Creates a face consisting of 3 positions, 3 texture coordinates and 3 normals.  Please note
		//that vertex indexing of '.obj' files begins at 1, but OpenGL expects elements to begin at 0.
		//Thus this class's constructor will automatically perform the conversion unless specifically 
		//asked not to.
		Face(std::array<Offset, 3> positions, std::array<Offset, 3> texCoords, std::array<Offset, 3> normals, bool fixIndexing = true);

		//Creates a face consisting of 4 positions, 4 texture coordinates and 4 normals.  Please note
		//that vertex indexing of '.obj' files begins at 1, but OpenGL expects elements to begin at 0.
		//Thus this class's constructor will automatically perform the conversion unless specifically 
		//asked not to.
		Face(std::array<Offset, 4> positions, std::array<Offset, 4> texCoords, std::array<Offset, 4> normals, bool fixIndexing = true);


		//Interface functions

		//Checks to see if the object contains valid data
		bool dataValid() const { return mValidData_; }
		//Checks to see if the face is a quadrilateral
		bool isQuad() const { return mIsQuad_; }
		//Checks to see if the face has Texture Coordinates
		bool hasTexCoord() const { return mHasTexCoords_; }
		//Checks to see if the face has data for normals
		bool hasNormals() const { return mHasNormals_; }


		bool operator==(const Face &) const;
		bool operator!=(const Face & that) const { return (!(*this == that)); }

	private:
		bool mValidData_;
		bool mIsQuad_;
		bool mHasTexCoords_;
		bool mHasNormals_;
		FaceComponent mPositions_;
		FaceComponent mTexCoords_;
		FaceComponent mNormals_;

		void initialize();
		bool parseFaceLine(const char * faceLine);
		bool verifyPositions(const int & parsedPositions, const char * faceLine) const;
		bool verifyTexCoords(const int & parsedTexCoord, const char * faceLine) const;
		bool verifyNormals(const int & parsedNormals, const char * faceLine) const;
		void fillFaceComponents(Offset* positions, Offset* texCoord, Offset* normals);
		//Checks to see if a string character is between '0' (i.e. 48u in ASCII) and '9' (i.e. 57u in ASCII) (i.e. if it's a number)
		bool isNumber(const char * c) const { return ((*c >= static_cast<char>(48u)) && (*c <= static_cast<char>(57u))); }
		
	};

} //namespace AssetLoadingInternal

#endif  //FACE_H_
