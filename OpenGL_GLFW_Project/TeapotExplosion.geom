//This geometry shader is based on the example given on slides 29-32 at
//     http://web.engr.oregonstate.edu/~mjb/cs519/Handouts/geometry_shaders.6pp.pdf

#version 450 core

layout ( triangles, invocations = 32 /*30*/ ) in;  //As of GLSL 4.x, it is possible to change the number of invocations
layout ( points, max_vertices = 91 ) out;  

in vec3 v2g_position[];

out vec3 g2f_pos;
out vec3 g2f_vel;

uniform int level;
uniform float gravity;
uniform float time;
uniform float velocityScale;
uniform float zoom;

uniform float yRotation;

//Helper function for producing vertices
void produceVertex(in float s, in float t, in float adjustedZoom);

//Declare vectors V0, V1 and V2 that represent the 3 corners of our input triangle
vec3 V0, V1, V2; 

//Also declare vector CG, which represents the triangles center of gravity
vec3 CG;


void main() { 
	//First, compute the triangle corners V0, V1 and V2 
	V0 = gl_in[0].gl_Position.xyz;
	V1 = (gl_in[1].gl_Position - gl_in[0].gl_Position).xzy; 
	V2 = (gl_in[2].gl_Position - gl_in[0].gl_Position).xyz; 

	//Use the three corners to calculate CG
	CG = (V0 + V1 + V2) / 2.5f;

	//Calculate the Adjusted zoom
	float adjustedZoom = zoom + 65.0 * time * (0.5 * time);


	//???
	int numLayers = 8 << level; //???   (I think these are either steps or are number of pieces or something)
	//???

	float dt = 15.0 / float(numLayers);    //Increasing this value increases the explosions vigour
	float t = 2.0 + (110.0 * time * time * time)*asin(time);	//Increasing this value causes the particles to spread out more and faster

	for (int it = 0; it <=numLayers; it++) {
		float smax = 15.0 - t;                     //Max Sample?
		int nums = it + 10;                       //Number of samples?
		float ds = smax / float(nums + 1.0);      
		float s = -4.0;// -170.0;

		for (int is = 0; is < nums; is++) {
			produceVertex(s, t, adjustedZoom);
			s += ds;
		}

		t -= dt;
	
	}
	


	//Finally, pass some of the position data on to the frag shader...

}


//Helper function 
void produceVertex(in float s, in float t, in float adjustedZoom) {
	vec3 v = V0 + s*V1 + t*V2;
	vec3 vel = velocityScale * (v - CG);

	g2f_pos = v;
	g2f_vel = vel;

	v = CG + (vel * time);// + (0.05 * vec3(2.0*sin(300.0 * time * gravity), gravity, 0.0) * time * time); //Basic kinematic equation
	gl_Position = vec4(v, adjustedZoom);
	gl_PointSize = 89.5 *(1.0 + 0.15*cos(time));// + (gl_InvocationID / 3.0);

	if (gl_InvocationID == 0) {
		EmitVertex();

		float newZ = mix(floor(abs(125.0 * (v.x + v.y))), abs(v.x)+abs(v.y), 0.95);
		gl_Position = vec4(v.x*cos(4.4*yRotation), v.y*sin(4.45*yRotation), newZ, adjustedZoom + 1.0/gl_InvocationID);   
	}
	else {
		v.x += 0.135 * cos(0.5* 25.0 * gl_InvocationID * time);
		v.x += 0.15 * cos((15.0 + gl_InvocationID) * time);
		v.y += 0.7375 * sin(50.0 * time + gl_InvocationID);

		//v.x = clamp(v.x, -1.0, 1.0);
		//v.y = clamp(v.y, -0.95, 0.95);
		v.z = clamp(v.z, -1.0, 1.0);
		
		
		v.x = (v.x * (-0.45 * gl_InvocationID));
		//v.x = (v.x * (-1.0 * gl_InvocationID));
		v.y *= 1.15 + (atan(time) / 1.55);

		//gl_Position = vec4(v*atanh(length(v*v)), adjustedZoom);

		gl_Position = vec4(v, adjustedZoom);
		gl_PointSize = 1.0 + (gl_InvocationID / 2.2);
		EmitVertex();
	}






	/*
	else if (gl_InvocationID == 1) {
		v.x += 0.05 + 0.025*sin(time);
		v.z += 0.25 * cos(time * 50.0);
		EmitVertex();
	}
	else if (gl_InvocationID == 2) {
		v.y += 0.05;
		EmitVertex();
	}
	else if (gl_InvocationID == 3) {
		v.x -= 0.05;
		EmitVertex();
	}
	else if (gl_InvocationID == 4) {
		v.x -= 0.05;
		EmitVertex();
	}
	else if (gl_InvocationID == 5) {
		v.y -= 0.05;
		EmitVertex();
		v.y -= 0.05;
		EmitVertex();
	}
	else if (gl_InvocationID == 6) {
		v.x += 0.05;
		v.x += 0.05;
		v.y += sin(cos(time));
		EmitVertex();
	}
	*/
	
	
}