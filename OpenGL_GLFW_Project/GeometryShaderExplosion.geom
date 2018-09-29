//This geometry shader is based on the example given on slides 29-32 at
//     http://web.engr.oregonstate.edu/~mjb/cs519/Handouts/geometry_shaders.6pp.pdf

#version 450 core

layout ( triangles, invocations = 1 ) in;  //As of GLSL 4.x, it is possible to change the number of invocations
layout ( points, max_vertices = 100 ) out;  

in vec3 v2g_position[];

out vec3 g2f_pos;

uniform int level;
uniform float gravity;
uniform float time;
uniform float velocityScale;
uniform float zoom;

//Helper function for producing vertices
void produceVertex(in float s, in float t);

//Declare vectors V0, V1 and V2 that represent the 3 corners of our input triangle
vec3 V0, V1, V2; 

//Also declare vector CG, which represents the triangles center of gravity
vec3 CG;

void main() { 
	//First, compute the triangle corners V0, V1 and V2 
	V0 = gl_in[0].gl_Position.xyz;
	V1 = (gl_in[1].gl_Position - gl_in[0].gl_Position).xyz; 
	V2 = (gl_in[2].gl_Position - gl_in[0].gl_Position).xyz; 

	//Use the three corners to calculate CG
	CG = (V0 + V1 + V2) / 3.0f;

	g2f_pos = CG;

	//???
	int numLayers = 1 << level; //???   (I think these are either steps or are number of pieces or sumthin)
	//???

	float dt = 1.0 / float(numLayers);
	float t = -0.0;

	for (int it = 0; it <=numLayers; it++) {
		float smax = 5.0 - t;
		int nums = it + 1;
		float ds = smax / float(nums - 1.0);
		float s = 0.;

		for (int is = 0; is < nums; is++) {
			produceVertex(s, t);
			s += ds;
		}

		t -= dt;
	
	}
	


	//Finally, pass some of the position data on to the frag shader...

	


}


//Helper function 
void produceVertex(in float s, in float t) {
	vec3 v = V0 + s*V1 + t*V2;
	vec3 vel = velocityScale * (v - CG);
	v = CG + (vel * time) + (0.5 * vec3(0.0, gravity, 0.0) * time * time); //Basic kinematic equation
	gl_Position = vec4(v, 1.0 + 15.0*time);
	gl_PointSize = 2.0;
	EmitVertex();
}