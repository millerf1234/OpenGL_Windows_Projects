//This file contains definitions for several utility functions that can
//be used in other fragment shaders. 
//Note that the majority of these functions work by modifying their parameters
//instead of by returning new values.

#version 450 core



//Convert Degrees to Radians 
float dg2Rad(float degrees) {
	return (degrees * 3.1415927 / 180.0f);
}


//Convert radians to degrees
float rad2Dg(float radians) {
	return (radians * 180.0f / 3.1415927);
}

//--------------------------------------------------------
// COLOR ROTATIONS
//--------------------------------------------------------

//Rotate a rgb vector about the 'red' axis
void rotateRed(inout vec3 color, in float theta) {
	mat3 redRotation = mat3( 1.0,     0.0    ,     0.0    ,
					         0.0,  cos(theta), -sin(theta),
		                     0.0,  sin(theta),  cos(theta)  );
	color = redRotation * color;
}

//Rotate a rgb vector about the 'green' axis
void rotateGreen(inout vec3 color, in float theta) {
	mat3 greenRotation = mat3( cos(theta),  0.0,   -sin(theta),
					              0.0    ,  1.0,       0.0    ,
					           sin(theta),  0.0,    cos(theta) );
	color = (rotateGreen * color);
}

//Rotate a rgb vector about the 'blue' axis
void rotateBlue(inout vec3 color, in float theta) {
	mat3 blueRotation = mat3( cos(theta), -sin(theta),  0.0,
					          sin(theta),  cos(theta),  0.0,
					              0.0   ,      0.0   ,  1.0 );
	color = (blueRotation * color);
}

//--------------------------------------------------------
// COLOR ROTATION SEQUENCES
//--------------------------------------------------------

//Rotate a color by Red, then by Green, then by Blue
void rotateColorRGB(inout vec3 color, in float redTheta, in float greenTheta, in float blueTheta) {
	float theta;

	//Create the rotation Matrices

	//RED
	theta = redTheta;
	mat3 redRotation = mat3( 1.0,     0.0    ,     0.0    ,
					         0.0,  cos(theta), -sin(theta),
		                     0.0,  sin(theta),  cos(theta)  );

	//GREEN
	theta = greenTheta;
	mat3 greenRotation = mat3( cos(theta),  0.0,   -sin(theta),
					              0.0    ,  1.0,       0.0    ,
					           sin(theta),  0.0,    cos(theta) );
	//BLUE
	theta = blueTheta;
	mat3 blueRotation = mat3( cos(theta), -sin(theta),  0.0,
					          sin(theta),  cos(theta),  0.0,
					              0.0   ,      0.0   ,  1.0 );

	//Compute rotations
	color = (blueRotation * (greenRotation * (redRotation * color))); //Red Rotation is performed first
}


//Rotate color by Green, then Red, then Blue
void rotateColorGRB(inout vec3 color, in float redTheta, in float greenTheta, in float blueTheta) {
	float theta;

	//Create the rotation Matrices

	//RED
	theta = redTheta;
	mat3 redRotation = mat3( 1.0,     0.0    ,     0.0    ,
					         0.0,  cos(theta), -sin(theta),
		                     0.0,  sin(theta),  cos(theta)  );

	//GREEN
	theta = greenTheta;
	mat3 greenRotation = mat3( cos(theta),  0.0,   -sin(theta),
					              0.0    ,  1.0,       0.0    ,
					           sin(theta),  0.0,    cos(theta) );
	//BLUE
	theta = blueTheta;
	mat3 blueRotation = mat3( cos(theta), -sin(theta),  0.0,
					          sin(theta),  cos(theta),  0.0,
					              0.0   ,      0.0   ,  1.0 );

	//Compute rotations
	color = (blueRotation * (redRotation * (greenRotation * color))); //Green Rotation is performed first
}

//Rotate color by Blue, then Green, then Red
void rotateColorBGR(inout vec3 color, in float redTheta, in float greenTheta, in float blueTheta) {
	float theta;

	//Create the rotation Matrices

	//RED
	theta = redTheta;
	mat3 redRotation = mat3( 1.0,     0.0    ,     0.0    ,
					         0.0,  cos(theta), -sin(theta),
		                     0.0,  sin(theta),  cos(theta)  );

	//GREEN
	theta = greenTheta;
	mat3 greenRotation = mat3( cos(theta),  0.0,   -sin(theta),
					              0.0    ,  1.0,       0.0    ,
					           sin(theta),  0.0,    cos(theta) );
	//BLUE
	theta = blueTheta;
	mat3 blueRotation = mat3( cos(theta), -sin(theta),  0.0,
					          sin(theta),  cos(theta),  0.0,
					              0.0   ,      0.0   ,  1.0 );

	//Compute rotations
	color = (redRotation * (greenRotation * (blueRotation * color))); //Green Rotation is performed first
}



//--------------------------------------------------------
// COLOR TINT-SHIFT Functions
//--------------------------------------------------------

//Shifts the color by 'percentage' percent. Percentage should be in closed interval [0,1], values outside
//this range will be clamped to the range.
void applyRedTint(inout vec3 color, in float percentage) {
	
	percentage = clamp(percentage, 0.0, 1.0);

	float initialRed = abs(color.r);
	color.r += (color.r * percentage); //Increase RED by percentage 
	color.g -= abs(((initialRed / color.r) * percentage));  //Lessen the other 2 colors 
	color.b -= abs(((initialRed / color.r) * percentage));  //Lessen the other 2 colors 

}

//Shifts the color by 'percentage' percent. Percentage should be in closed interval [0,1], values outside
//this range will be clamped to the range.
void applyGreenTint(inout vec3 color, in float percentage) {

	percentage = clamp(percentage, 0.0, 1.0);

	float initialGreen = abs(color.g);
	color.g += (color.g * percentage); //Increase GREEN by percentage 
	color.r -= abs(((initialGreen / color.g) * percentage));  //Lessen the other 2 colors 
	color.b -= abs(((initialGreen / color.g) * percentage));  //Lessen the other 2 colors 

}

//Shifts the color by 'percentage' percent. Percentage should be in closed interval [0,1], values outside
//this range will be clamped to the range.
void applyBlueTint(inout vec3 color, in float percentage) {

	percentage = clamp(percentage, 0.0, 1.0);

	float initialBlue = abs(color.b);
	color.b += (color.b * percentage); //Increase RED by percentage 
	color.g -= abs(((initialBlue / color.b) * percentage));  //Lessen the other 2 colors 
	color.b -= abs(((initialBlue / color.b) * percentage));  //Lessen the other 2 colors 

}

