//Class to represent a face loaded from an '.obj' file (and perhaps 
//this class will be used for other reasons as well in the future)
//
//  Programmer:    Forrest Miller
//  Date:          October 18, 2018
//
//Note: This class stores offsets, not actual vertex data values

#pragma once

#ifndef FACE_H_
#define FACE_H_


#include <array>    //std::array

//#include "Mesh.h"
using Offset = size_t;
using DatapointCount = size_t;


namespace AssetLoadingInternal {
	//Dictates how the '==' operator evaluates invalid faces
	static constexpr bool TREAT_INVALID_FACES_AS_EQUIVALENT = true;

	static constexpr const int FACE_COMPONENTS_PER_VERTEX = 3; //A vertex has position, textureCoord and normal components 
	static constexpr const int QUAD_VERTICE_COUNT = 4; //A quad consists of 4 vertices
	static constexpr const int TRIANGLE_VERTICE_COUNT = 3; //A triangle consists of 3 vertices


	class Face {
		//Class that represents the component of a face (such as position, normal or tex coord). There are
		//up to four vertices per face if the input mesh uses quads.
		class FaceComponent {
		public:
			FaceComponent() : mVertIndices_({ 0, 0, 0, 0 }) { ; }
			FaceComponent(Offset off1, Offset off2, Offset off3) : mVertIndices_({ off1, off2, off3, 0 }) { ; }
			FaceComponent(Offset off1, Offset off2, Offset off3, Offset off4) : mVertIndices_({ off1, off2, off3, off4 }) { ; }
			
			//Desctructor, Copying and Moving are all default
			~FaceComponent() = default;
			FaceComponent(const FaceComponent&) = delete;// default;
			FaceComponent(FaceComponent&&) = default;
			FaceComponent& operator=(const FaceComponent&) = delete;// default;
			FaceComponent& operator=(FaceComponent&&) = default;

			//Compares indices for equality (will always compare all 4 components)
			bool operator==(const FaceComponent& that) const { return (mVertIndices_ == that.mVertIndices_); }
			//Compares indices for inequality (will always compare all 4 components)
			bool operator!=(const FaceComponent& that) const { return (mVertIndices_ != that.mVertIndices_); }

			//Compares just the first 3 indices of two FaceComponent objects
			bool compareAsTriangles(const FaceComponent& that) const;

			//Getters
			Offset& operator[] (int indx) { return mVertIndices_.data()[indx]; }
			//std::array<Offset, QUAD_VERTICE_COUNT>& get() { return mVertIndices_; }
		private:
			std::array<Offset, QUAD_VERTICE_COUNT> mVertIndices_;
		};


	public:
		//Constructs a face with initial invalid data. Data should not
		//attempt to be accessed, object will issue a warning.
		Face();

		//Desctructor, Copying and Moving are all default 
		~Face() = default;
		Face(const Face&) = delete;// default;
		Face(Face&&) = default;
		Face& operator=(const Face&) = delete;// default;
		Face& operator=(Face&&) = default;

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

		/*  //For now, have parsing be done by each face
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

		*/

		//Interface functions

		//Checks to see if the object contains valid data
		bool dataValid() const { return mValidData_; }
		//Checks to see if the face is a quadrilateral
		bool isQuad() const { return mIsQuad_; }
		//Checks to see if the face has Texture Coordinates
		bool hasTexCoord() const { return mHasTexCoords_; }
		//Checks to see if the face has data for normals
		bool hasNormals() const { return mHasNormals_; }

		//Returns an array of 3 arrays which each hold the three offsets that define the 
		//vertices of a face. This is more intuitive than it seems. For example, if the face
		//was stored as 
		//      "f 1/2/1 2/2/1 3/3/1" 
		//then this function would return 
		//       {{1,2,1}, {2,2,1}, {3,3,1}}
		//Any components that were not set in the model data will be returned as 0, it is up to 
		//any code calling this function to check this objects state to see if there is actual 
		//valid data being store for these components
		std::array<std::array<Offset, FACE_COMPONENTS_PER_VERTEX>, TRIANGLE_VERTICE_COUNT> getTriangleFace();

		//Returns an array of 4 arrays which each hold the three offsets that define each 
		//vertex of a face. This is more intuitive than it seems. For example, if the face
		//was stored as 
		//      "f 1/2/1 2/2/1 3/3/1 4/3/1" 
		//then this function would return 
		//       {{1,2,1}, {2,2,1}, {3,3,1}, {4,3,1}}
		//Any components that were not set in the model data will be returned as 0, it is up to 
		//any code calling this function to check this objects state to see if there is actual 
		//valid data being store for these components. Additionally, it should be verified that the 
		//face actually is a Quad face before calling this function.
		std::array<std::array<Offset, FACE_COMPONENTS_PER_VERTEX>, QUAD_VERTICE_COUNT> getQuadFace();

		//Does a component-by-component comparison of two faces. Note that a quad and a triangle
		//face will never compare as equal, even if their first 3 vertices match.
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

		/////////////////////////////////////////////////////////////////////////////
		//This class's implementation uses the following private helper functions:
		
		//Puts the object into a defined initial state  
		void initialize();

		//Parses a line of text 
		bool parseFaceLine(const char * faceLine);
		//The following functions are used within the parseFaceLine() function
		bool checkForNoForwardSlashes(const char * stringIter) const;
		bool parseLineWithNoForwardSlashes(const char * stringIter, Offset * positionData, int & readPositions) const;
		bool verifyPositions(const int & parsedPositions, const char * faceLine) const;
		bool verifyTexCoords(const int & parsedTexCoord, const char * faceLine) const;
		bool verifyNormals(const int & parsedNormals, const char * faceLine) const;
		//Takes the temporary arrays that were filled over the course of the parse loop and sets this objects FaceComponent members from that data
		void fillFaceComponents(Offset* positions, Offset* texCoord, Offset* normals);
		//Checks to see if a string character is between '0' (i.e. 48u in ASCII) and '9' (i.e. 57u in ASCII) (i.e. if it's a number)
		bool isNumber(const char * c) const { return ((*c >= static_cast<char>(48u)) && (*c <= static_cast<char>(57u))); }
		//Checks to see if a chracter is the number '0'  (to make sure a face that might look like "10/0/3" is marked invalid)
		bool isZero(const char * c) const { return (*c == static_cast<char>(48u)); }

		//Shifts the objects stored offsets down by 1 each to convert indexing from starting at 
		//1 to starting at 0. Object's state is querried to determine which indexes to shift 
		//(to prevent shifting an unassigned index which may already be 0). Having a valid 
		//index at 0 already when this function is called is an error. 
		void convertIndexingToBeginAtZero();
		/////////////////////////////////////////////////////////////////////////////
		
	};

} //namespace AssetLoadingInternal

#endif  //FACE_H_
