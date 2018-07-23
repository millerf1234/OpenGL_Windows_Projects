//This was the header I wrote for the UniformLocationCache (manager/tracker) class that I didn't wind up coding when I instead
//decided to code this class (the CachedUniformLocation (wrapper/handle) class).
//As you can see for this commit I have not yet actualy coded the CachedUniformLocation class yet. My intention is to write this
//message that is being read, then committing all my files and deleting this message. 
//Then I will write a message for the CachedUniformLocation class, and code it. 
//
//
//
//
//
//CachedUniformLocation wrapper 
//
//
//This class is supposed to supplement/co-exist with the UniformLocationTracker object. 
//While the UniformLocationTracker will be automated and easy to use, each time it is
//used results in a GL function call (actually this is not true!)
//This class provides an alternative for direct program uniform updates based off a uniform location
//...
// As maybe can be seen, I am still thinking this through. This class might be surperfluous, or it might actually 
// be faster/better due to the lack of a need for string comparisons. 
//Created by Forrest Miller on July 22, 2018

//The whole reason I wanted to write this class is: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glProgramUniform.xhtml\
//As opposed to the usual look-up location then update process I have been using 




#pragma once


class CachedUniformLocation {
public:
	CachedUniformLocation();
	~CachedUniformLocation();
};

