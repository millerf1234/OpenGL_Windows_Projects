[Not every note/thought contained in this document is complete at this time]

This is just an extra text file containing random notes related to a whole range of topics. This means 
that this file includes notes/research/weblinks towards ideas I have going forward but haven't implemented yet,
random thoughts I have/had whilest writing the code (including explination and justification of how I expect/hope for
things to work), and references/links to resource files that either are currently being used or that I think I might
like to use.

I created this file on July 25, 2018,  but the plan is to fill it in as I go...

-Forrest

//RANDOM NOTE: To see vcpkg commands, go to the link: https://docs.microsoft.com/en-us/cpp/vcpkg#command-line-reference


////////////////////////////////////////////////////////////////////////////////////////////
//Random Design Philosophy Note(s)   --   November 2, 2018
// CLASS DESIGN
//    Private Member Variables (henceforth PMVs):
//	    When naming PMVs variables, if the class is used for a utility purpose (which is what
//        ~90% of the classes in this project are), I [have tried to] follow the PMV naming 
//        convention of both prefixing each PMV with a lowercase 'm' and postfixing
//        each PMV with an underscore. For example, if the class has an integer PMV named 'counter',
//        the variables declaration would be: 
//                                  int  mCounter_;
//      The remaining 10% of the classes are the exception to this rule. These are the classes which
//        form the central/primary classes that provide the backbone/interface for the program. For example,
//        these include clases such as 'Application'; or additionally anything inheriting from the abstract
//        base class 'RenderDemoBase.' The reasoning for this is I am expecting much more frequent updates,
//        modifications and even just programmer-reading/debugging to take place. 
//     The purpose of having this distinction in the naming convention is to provide clear seperation between
//        code/pmv's that are part of well tested and coordinated classes versus classes which are more likly 
//        to contain bugs and/or fresher code. 
//
//
//    
////////////////////////////////////////////////////////////////////////////////////////////


//---------------------------------------------------------------------------//
//August 23, 2018
//  Here is a reference for types that may be encountered in OpenGL
//         https://www.khronos.org/opengl/wiki/OpenGL_Type
//---------------------------------------------------------------------------//


//|\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//|\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//|  Web Links:
  |		For Relating OpenGL types to Specific Types, see:  https://stackoverflow.com/questions/8932912/whats-the-advantage-of-using-gluint-instead-of-unsigned-int
  |
  |		For StackOverflow post on anonymous namespaces: https://stackoverflow.com/a/156834
  |
  |		LAMBDA EXPRESSIONS (YOU REALLY SHOULD LEARN THIS!) https://msdn.microsoft.com/en-us/library/dd293608(d=printer).aspx
  |
  |		Difference between glBufferStorage and glBufferData: https://stackoverflow.com/questions/27810542/what-is-the-difference-between-glbufferstorage-and-glbufferdata
  |
  |		Random OpenGL reading that is worth looking at again:
  |				https://www.glprogramming.com/red/chapter03.html#name1
  |				http://web.cse.ohio-state.edu/~shen.94/781/Site/Slides_files/pipeline.pdf
  |
  |		OpenGL Framebuffer intro: http://www.songho.ca/opengl/gl_fbo.html
//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\




//FUTURE IDEAS:

//Overall idea of project in its current state:
// Create a renderer that draws a scene.
//       First I need a camera object and a way to represent/store world object vertices
//		 To do that well, it would be helpful to have a working OpenGL interface to simplyfy shader code 
//				while still maintaining all the flexibility that comes with the low-level programming. This
//				also means not shooting myself in the foot by writing slow/inefficent code.
//
//
// So I have been working for a bit now on the OpenGL interface, and I have been thinking it would really be helpful
// to give my Renderer/Engine a name. I have been doing some research on the internet trying to think of a good name.
// Here are some ideas I have had thus far:
//			(Greek Mathematicins) :
//					Archytas    --  Believed that arithmatic, not geometry, was the true language of proofs.  https://en.wikipedia.org/wiki/Archytas
//									  Defined (but didn't invent) the Harmonic mean, which is important in Projective Geometry   https://en.wikipedia.org/wiki/Harmonic_mean
//									  Pronounced "Ar-he-tas"   (see: https://www.youtube.com/watch?v=GXqZXDd5sUo )
//					Pappus      --  Considered to be the 'father' of projective geometry. 
//
//			(Minearls/Elements)
//					Cerium      --  Metal element
//									  Non-Toxic
//					Galium      --  Semi-Metal element
//					
//			(Catchy made-up words that sounds sorta like real words)
//					GPUlutions  -- like 'GPU' + 'Solutions' (no)
//					UniBuffer   -- like as in one system for all your buffer needs
//                  
//
//
// Idea for dealing with how to proceed when shader compilation fails:
//		Have a generic passthrough shader stored as a static string and that will always compile. Then
//		  have it replace shaders that fail to load/compile.
//
//






//  Types of buffer comment from a deleted file I deleted
//				ARRAY_BUFFER						 -- Not Yet Implemented
//				ATOMIC_COUNTER_BUFFER                -- Not Yet Implemented
//				COPY_READ_BUFFER					 -- Not Yet Implemented
//				COPY_WRITE_BUFFER					 -- Not Yet Implemented
//				DISPATCH_INDIRECT_BUFFER             -- Not Yet Implemented
//				DRAW_INDIRECT_BUFFER	             -- Not Yet Implemented
//				ELEMENT_ARRAY_BUFFER	             -- Not Yet Implemented
//				PIXEL_PACK_BUFFER                    -- Not Yet Implemented	
//				PIXEL_UNPACK_BUFFER	                 -- Not Yet Implemented
//				QUERY_BUFFER                         -- Not Yet Implemented
//				SHADER_STORAGE_BUFFER                -- Not Yet Implemented	
//				TEXTURE_BUFFER						 -- Not Yet Implemented
//				TRANSFORM_FEEDBACK_BUFFER            -- Not Yet Implemented	
//				UNIFORM_BUFFER						 -- Not Yet Implemented
//				


RANDOM NOTES  (read later to remind self):

//Random note on Open-Source library installer that I wasn't sure where else to put:
//see: https://github.com/Microsoft/vcpkg/blob/master/README.md  

//On full-program optimization, read the article linked at: https://msdn.microsoft.com/en-us/magazine/dn904673.aspx