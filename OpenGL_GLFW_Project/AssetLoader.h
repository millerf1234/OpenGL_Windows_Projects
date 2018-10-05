//File:       AssetLoader.h
//Class:      AssetLoader
//Namespace:  N/A 
//Programmer: Forrest Miller
//Date(s):    10/4/2018
//
//  Description:    This file contains the definition for the abstract base 
//                  class RenderDemoBase. RenderDemoBase provides an interface 
//					that can be called by an application to reduce the amount of
//					code duplication within an application that may run multi RenderDemos.
//					There are 3 key pieces of functionality used within every render demo 
//					project, which are:
//										(i)   Construction 
//									    (ii)  Loading of Assets (including shader compilation)
//										(iii) Run Demo  
//					
//
// UPDATES/Changes:    None (currently)
//					
//			
//Notes:			-All Render Projects inheriting from this class must implement the two required
//					 functions. It is meant to be assumed that the calling application will construct 
//				     an instance of an object derived from this class, will check for error, then will
//                   call load(), check for errors and then will call run(). It is up to objects derived
//					 from this class to handle all of the user's input (except for maybe window resizing)
//                   and to provide an exit condition for the program. It is not yet known whether this 
//				     exit will constitute the termination of the entire application or if the application
//					 will provide a menu system to switch between Render Demos.  
//					 
//
//
//Errors to watch for:  Derived classes should play things safe and check to make sure the Application
//						holding them provides them with valid parameters (this mostly means make sure 
//						the pointer to the render target window is valid/not-NULL). 
//
//

#pragma once


class AssetLoader {
public:
	AssetLoader();
	~AssetLoader();

private:


};

