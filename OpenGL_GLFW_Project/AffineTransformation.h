//Class: AffineTransformation
//Namespace: ModelInterface
//Programmer: Forrest Miller
//Date(s): 8/6/2018  
//
//Description:      This class uses 4x4 matrices to represent a product of Affine 
//					Transformations of 3-space. Since luckily for us the modeling
//					transformations used in the fixed-function pipeline of OpenGL are
//					all compositions of affine transforms, this class can be used to
//					represent the transforms active on an object.
//			        
//					There are 3 main types of affine transformation:
//							Translate()
//							Rotate()
//							Scale()
//
//					This class is based around a list of these transforms representing the
//					transforms applied to a model. There is also an active 4x4 matrix
//					representing the composition of the active transforms in the list.
//			   
//					The stages a model goes through from it's own coordinate system
//					through world coordinates to the final coordinates rendered to the
//					screen are:
//
//                    [START]                       {model view transformation}
//                        Local (Object) Space ----------------------------------> Eye Space
//																				       |
//																				       |
//																				       |
//																     {Projection	   |
//																	   Transformation} |
//																				       |
//																				       |
//																				       |
//																				       |
//													{Perspective divide}	    	   V
//						         NDC Space <--------------------------------------- Clip Space
//									 |
//									 |
//									 | {Scale and 
//									 |	 Translate}   {aka viewport transform}
//									 |
//									 |
//                                   V
//								Window Space          
//										[END]
//
//
//			Transformation Details:
//						Model View Matrix:            
//							The ModelView matrix is a product of the Viewing Transform Matrix (V) and
//							 the Modeling Transform matrix (M). (i.e. ViewModel = V*M)
//
//						Projection Matrix:
//							Todo... (orthographic and perspective)
//						
//						Viewport Transform Matrix:
//							Todo... (Scale and Translate)
//					    
//
//
//
//UPDATES/Changes:     None yet...
//
//			
//Notes:   -Since most of the time the camera will be symmetric about the Z
//			axis, the frustrum transform can be replaced with the Perspective transform.
//				(see page 94 of Guha book) 
//
//
//		   -S
//			
//			
//			
//			
//
//
//Errors to watch for:  Bad Matrix Conditioning
//						Overflow error
//						CancellationError (Underflow)
//						
//
// References:    I used "Computer Graphics Through OpenGL: From Theory to Experiments, 2nd edition" by Sumanta Guha  
//				  heavily as a reference while writing this class. Especially sections 5.2-5.4 (pages 192-251)
//				
//				  I also used: http://web.cse.ohio-state.edu/~shen.94/781/Site/Slides_files/pipeline.pdf

#pragma once

#ifndef AFFINE_TRANSFORMATION_H_
#define AFFINE_TRANSFORMATION_H_

#include <vector>
#include <list>

#include "ProjectConstants.h"
#include "ProjectParameters.h"

namespace ModelInterface {

	enum class ATType {NULLMAT, IDENTITY, AFFINE_COMPOSITION, TRANSLATION, SCALING, ROTATION, REFLECTION, ORTHOGRAPHIC, PERSPECTIVE, FRUSTUM, LOOKAT, MODEL, VIEW, MODELVIEW, VIEWPORT, ASPECT_RATIO, CUSTOM};

		class AffineTransformation {
		public:
			AffineTransformation();
			AffineTransformation(bool makeNull);

			~AffineTransformation();


			glm::mat4 getMatrix() const { return mTransformationMatrix; }
			ATType type() const { return mType; }
			

			void resetToIdentity();


		private:
			glm::mat4 mTransformationMatrix;

			ATType mType;
			std::list<AffineTransformation> mCompositionTransforms;
			std::vector<AffineTransformation> mTransformationOrder;

			void clearTransformationOrder();
			void rebuildTransformationOrder();
	};


} //namespace ModelInterface

#endif //AFFINE_TRANSFORMATION_H_
