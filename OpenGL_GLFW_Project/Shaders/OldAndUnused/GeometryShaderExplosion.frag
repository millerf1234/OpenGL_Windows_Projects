//Fragment shader for the GeomtryShaderExplosion demo project. 
// Forrest Miller on 9/28/2018

#version 450 core

in vec3 g2f_pos;  //Position data from geomtry shader
in vec3 g2f_vel; //Velocity data from geometry shader

out vec4 FragColor; 

void main() {

	vec3 position = vec3(abs(g2f_pos));

	FragColor = vec4(dot(position, g2f_vel), cross(position, g2f_vel));

	//float minColor = 0.0f;
	//float maxColor = 0.0f;

	//FragColor = vec4(abs(g2f_pos.x), abs(g2f_pos.y), abs(g2f_pos.z), 1.0);

	//clamp(FragColor, minColor, maxColor);
	


}