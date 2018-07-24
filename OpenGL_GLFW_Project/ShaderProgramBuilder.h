//This class takes in a ShaderInitializerInstructions struct (or something like it) and uses that 
//to construct completed shaderPrograms. This class can also store a stash of compiled shaders so they can be reused for future programs

#pragma once
class ShaderInitializer
{
public:
	ShaderInitializer();
	~ShaderInitializer();
};

