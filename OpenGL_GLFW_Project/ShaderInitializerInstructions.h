//Contains a struct of instructions that can be configured by different pieces of code to tell ShaderInitializer how to do it's thing

#pragma once




typedef struct ShaderInitializerInstructions {


	//Have instructions like: 
	//		-- search for an already compiled shader, if it's there use it, if not, then make another one.
	//		-- mark each shader as destory or keep around, so that shaders are destroyed when necessary


} ShaderInitializerInstructions;