//This GLSL shader is intended to be used with TeapotExplosion.vert and 
//TeapotOutline.geom. This file determines how the teapot outline will be colored.

#version 450 core

in /*flat*/ vec3 vertPos; //Vertex position from vertex shader
in /*flat*/ vec3 vertNormal; //Vertex normal from the vertex shader

out vec4 FragColor;


uniform float time;


void computePhong(inout vec3 ambient, in vec3 vertPos, in vec3 nrml, in vec3 diffuse, in vec3 spec, in vec3 lightPos, in vec3 lightColor, in vec3 eyePos, in float shiny);


void main() {

	vec3 tempColor = abs(dot(vertPos, vertNormal))*abs(vertPos);

	//tempColor = normalize(cross(dFdx(tempColor), dFdy(tempColor)));

	tempColor = normalize(tempColor);
	//FragColor = vec4(0.25, 0.25, 0.25, 1.0) + vec4(tempColor, 0.0);

	//if (length(FragColor.xyz) > 1.0) {
	//	FragColor.xyz = abs(vec3(0.750) - FragColor.xyz);
	//}
	
	vec3 ambient = vec3(0.91, 0.1, 0.1);
	vec3 diffuse = vec3(0.8, 0.8, 0.8);
	vec3 specular = vec3(0.8, 0.8, 0.8);
	vec3 lightPos = vec3(0.0, 5.0, 0.0);
	vec3 lightColor = vec3(0.4, 0.5, 0.7);
	vec3 eyePos = vec3(0.0, 0.0, 5.0);
	float shiny = 0.7;

	computePhong(ambient, vertPos, vertNormal, diffuse, specular, lightPos, lightColor, eyePos, shiny);
	FragColor = vec4(ambient, 1.0);
}


void computePhong(inout vec3 ambient, in vec3 vertPos, in vec3 nrml, in vec3 diffuse, in vec3 spec, in vec3 lightPos, in vec3 lightColor, in vec3 eyePos, in float shiny) {
	ambient = ambient + vec3(0.15, 0.15, 0.15);

	vec3 lightDirection = normalize(lightPos - vertPos);
	
	ambient += dot(nrml, lightDirection) * normalize(eyePos - vertPos);

}