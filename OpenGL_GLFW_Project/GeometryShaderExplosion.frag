//Fragment shader for the GeomtryShaderExplosion demo project. 
// Forrest Miller on 9/28/2018

#version 450 core

in vec3 g2f_pos;  //Position data from geomtry shader

out vec4 FragColor; 

void main() {

	float minColor = 0.0f;
	float maxColor = 0.0f;

	FragColor = vec4(abs(g2f_pos.x), abs(g2f_pos.y), abs(g2f_pos.z), 1.0);

	//clamp(FragColor, minColor, maxColor);
	
}