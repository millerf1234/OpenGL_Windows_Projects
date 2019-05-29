//
//  NOTICE:  This is not my code! This code was available on the internet from 
//           the ShaderFrog website. 
//  SOURCE:   http://shaderfrog.com/view/2843
//
// 
//  Modifications:  Several modifications have been made to bring this shader inline with 
//                  something that the renderer will compile. Any changes made will be marked.
//                  These changes will just be variable renaming.
//

#version 460 core    ///Added by FSM
///precision highp float;    ///Removed by FSM
///precision highp int;      ///Removed by FSM


out vec3 vPosition;      ///varying vec3 vPosition;     ///Modified by FSM
out float vJitter;       ///varying float vJitter;      ///Modified by FSM
out int vInst;

uniform float zoom;     ///FSM -- Added additional uniform to control zoom
uniform float time;

uniform mat4 MVP;                ///Added by FSM
uniform mat4 rotation;           ///Added by FSM
///uniform mat4 modelViewMatrix;        //Modified by FSM
///uniform mat4 projectionMatrix;       //Modified by FSM


layout(location = 0) in vec4 ModelPosition;   ///Added by FSM
layout(location = 1) in vec2 ModelTexCoord;   ///Added by FSM
layout(location = 2) in vec3 ModelNormal;     ///Added by FSM
///attribute vec3 position;
///attribute vec3 normal;

// Permutation polynomial: (34x^2 + x) mod 289
vec3 permute(vec3 x) {
	return mod((34.0 * x + 1.0) * x, 289.0);
}

vec3 dist(vec3 x, vec3 y, vec3 z) {
	return x * x + y * y + z * z ; //Original
    /*
    vec3 x2 = x*x;
    vec3 y2 = y*y;
    vec3 z2 = z*z;
    return (gl_InstanceID)*time*(max(x*x,x2*x2) + max(y*y, y2*y2) + max(z*z, z2*z2));
    */
}

vec2 worley(vec3 P, float jitter) {
	float K = 0.142857142857; // 1/7
	float Ko = 0.428571428571; // 1/2-K/2
	float  K2 = 0.020408163265306; // 1/(7*7)
	float Kz = 0.166666666667; // 1/6
	float Kzo = 0.416666666667; // 1/2-1/6*2

	vec3 Pi = mod(floor(P), 289.0);
	vec3 Pf = fract(P) - 0.5;

	vec3 Pfx = Pf.x + vec3(1.0, 0.0, -1.0);
	vec3 Pfy = Pf.y + vec3(1.0, 0.0, -1.0);
	vec3 Pfz = Pf.z + vec3(1.0, 0.0, -1.0);

	vec3 p = permute(Pi.x + vec3(-1.0, 0.0, 1.0));
	vec3 p1 = permute(p + Pi.y - 1.0);
	vec3 p2 = permute(p + Pi.y);
	vec3 p3 = permute(p + Pi.y + 1.0);

	vec3 p11 = permute(p1 + Pi.z - 1.0);
	vec3 p12 = permute(p1 + Pi.z);
	vec3 p13 = permute(p1 + Pi.z + 1.0);

	vec3 p21 = permute(p2 + Pi.z - 1.0);
	vec3 p22 = permute(p2 + Pi.z);
	vec3 p23 = permute(p2 + Pi.z + 1.0);

	vec3 p31 = permute(p3 + Pi.z - 1.0);
	vec3 p32 = permute(p3 + Pi.z);
	vec3 p33 = permute(p3 + Pi.z + 1.0);

	vec3 ox11 = fract(p11*K) - Ko;
	vec3 oy11 = mod(floor(p11*K), 7.0)*K - Ko;
	vec3 oz11 = floor(p11*K2)*Kz - Kzo; // p11 < 289 guaranteed

	vec3 ox12 = fract(p12*K) - Ko;
	vec3 oy12 = mod(floor(p12*K), 7.0)*K - Ko;
	vec3 oz12 = floor(p12*K2)*Kz - Kzo;

	vec3 ox13 = fract(p13*K) - Ko;
	vec3 oy13 = mod(floor(p13*K), 7.0)*K - Ko;
	vec3 oz13 = floor(p13*K2)*Kz - Kzo;

	vec3 ox21 = fract(p21*K) - Ko;
	vec3 oy21 = mod(floor(p21*K), 7.0)*K - Ko;
	vec3 oz21 = floor(p21*K2)*Kz - Kzo;

	vec3 ox22 = fract(p22*K) - Ko;
	vec3 oy22 = mod(floor(p22*K), 7.0)*K - Ko;
	vec3 oz22 = floor(p22*K2)*Kz - Kzo;

	vec3 ox23 = fract(p23*K) - Ko;
	vec3 oy23 = mod(floor(p23*K), 7.0)*K - Ko;
	vec3 oz23 = floor(p23*K2)*Kz - Kzo;

	vec3 ox31 = fract(p31*K) - Ko;
	vec3 oy31 = mod(floor(p31*K), 7.0)*K - Ko;
	vec3 oz31 = floor(p31*K2)*Kz - Kzo;

	vec3 ox32 = fract(p32*K) - Ko;
	vec3 oy32 = mod(floor(p32*K), 7.0)*K - Ko;
	vec3 oz32 = floor(p32*K2)*Kz - Kzo;

	vec3 ox33 = fract(p33*K) - Ko;
	vec3 oy33 = mod(floor(p33*K), 7.0)*K - Ko;
	vec3 oz33 = floor(p33*K2)*Kz - Kzo;

	vec3 dx11 = Pfx + jitter * ox11;
	vec3 dy11 = Pfy.x + jitter * oy11;
	vec3 dz11 = Pfz.x + jitter * oz11;

	vec3 dx12 = Pfx + jitter * ox12;
	vec3 dy12 = Pfy.x + jitter * oy12;
	vec3 dz12 = Pfz.y + jitter * oz12;

	vec3 dx13 = Pfx + jitter * ox13;
	vec3 dy13 = Pfy.x + jitter * oy13;
	vec3 dz13 = Pfz.z + jitter * oz13;

	vec3 dx21 = Pfx + jitter * ox21;
	vec3 dy21 = Pfy.y + jitter * oy21;
	vec3 dz21 = Pfz.x + jitter * oz21;

	vec3 dx22 = Pfx + jitter * ox22;
	vec3 dy22 = Pfy.y + jitter * oy22;
	vec3 dz22 = Pfz.y + jitter * oz22;

	vec3 dx23 = Pfx + jitter * ox23;
	vec3 dy23 = Pfy.y + jitter * oy23;
	vec3 dz23 = Pfz.z + jitter * oz23;

	vec3 dx31 = Pfx + jitter * ox31;
	vec3 dy31 = Pfy.z + jitter * oy31;
	vec3 dz31 = Pfz.x + jitter * oz31;

	vec3 dx32 = Pfx + jitter * ox32;
	vec3 dy32 = Pfy.z + jitter * oy32;
	vec3 dz32 = Pfz.y + jitter * oz32;

	vec3 dx33 = Pfx + jitter * ox33;
	vec3 dy33 = Pfy.z + jitter * oy33;
	vec3 dz33 = Pfz.z + jitter * oz33;

	vec3 d11 = dist(dx11, dy11, dz11);
	vec3 d12 = dist(dx12, dy12, dz12);
	vec3 d13 = dist(dx13, dy13, dz13);
	vec3 d21 = dist(dx21, dy21, dz21);
	vec3 d22 = dist(dx22, dy22, dz22);
	vec3 d23 = dist(dx23, dy23, dz23);
	vec3 d31 = dist(dx31, dy31, dz31);
	vec3 d32 = dist(dx32, dy32, dz32);
	vec3 d33 = dist(dx33, dy33, dz33);

	vec3 d1a = min(d11, d12);
	d12 = max(d11, d12);
	d11 = min(d1a, d13); // Smallest now not in d12 or d13
	d13 = max(d1a, d13);
	d12 = min(d12, d13); // 2nd smallest now not in d13
	vec3 d2a = min(d21, d22);
	d22 = max(d21, d22);
	d21 = min(d2a, d23); // Smallest now not in d22 or d23
	d23 = max(d2a, d23);
	d22 = min(d22, d23); // 2nd smallest now not in d23
	vec3 d3a = min(d31, d32);
	d32 = max(d31, d32);
	d31 = min(d3a, d33); // Smallest now not in d32 or d33
	d33 = max(d3a, d33);
	d32 = min(d32, d33); // 2nd smallest now not in d33
	vec3 da = min(d11, d21);
	d21 = max(d11, d21);
	d11 = min(da, d31); // Smallest now in d11
	d31 = max(da, d31); // 2nd smallest now not in d31
	d11.xy = (d11.x < d11.y) ? d11.xy : d11.yx;
	d11.xz = (d11.x < d11.z) ? d11.xz : d11.zx; // d11.x now smallest
	d12 = min(d12, d21); // 2nd smallest now not in d21
	d12 = min(d12, d22); // nor in d22
	d12 = min(d12, d31); // nor in d31
	d12 = min(d12, d32); // nor in d32
	d11.yz = min(d11.yz, d12.xy); // nor in d12.yz
	d11.y = min(d11.y, d12.z); // Only two more to go
	d11.y = min(d11.y, d11.z); // Done! (Phew!)
	return sqrt(d11.xy); // F1, F2
}


//Output:
//out vec3 vPosition;      ///varying vec3 vPosition;     ///Modified by FSM
//out float vJitter;       ///varying float vJitter;      ///Modified by FSM
//out int vInst;

void main() {
gl_PointSize = 14.0 + 16.0*sin(0.1*time + (22203.14159 / float(1+34*gl_VertexID)));
    //vInst will always be this globally for every option
    //vInst = gl_InstanceID + -9 * gl_InstanceID;
    vInst = gl_InstanceID + max(-20, (-19 + int(floor(13.0*sin(2.0*time))) * gl_InstanceID));


#define CRAZY 1
#define INSTS_GROW_TO_THE_RIGHT 1

//#define ALTERNATIVE_VERT 1

#ifndef  ALTERNATIVE_VERT 

	///FSM updated main implementation
	///vJitter = 0.0 + 0.00308 * sin(30.*time + gl_VertexID);

    #ifdef CRAZY
    float crazyFactor = (900.0 + 600.0*sin(0.1*time + 3.14159));
    #else 
    float crazyFactor = 100.0f;
    #endif 
	vJitter =  crazyFactor * (float((gl_VertexID % 12) + 1)*(0.00180090308 * sin(3.*time + float(gl_VertexID) + float(gl_VertexID%7)*0.25*time)));
	vPosition = (ModelPosition.xyz * 0.10);// + 0.7*vec3(sin(time), cos(time), 0.0);

	vec2 worl = worley(vPosition, vJitter);
	vec3 pos = ModelPosition.xyz - (0.3 + 0.003*vInst)*(length(worl) * ModelNormal * 0.2);
      
	float instGrowth =  ( 0.04/(2.0+abs(3.0*sin(time)))*float(floor(gl_InstanceID / 10)*int(pow(float(gl_InstanceID), 1.005)) / 1000));

    #ifdef  INSTS_GROW_TO_THE_RIGHT
	gl_Position = (MVP * vec4(pos, ModelPosition.w + 0.0* min((2.0 * length(ModelPosition)*vJitter), 1.0))) + 
    vec4(2.0 * instGrowth * gl_InstanceID, 10.0 * instGrowth * sin(2.3*gl_InstanceID), -12.0, zoom);
    #else 
    gl_Position = (MVP * vec4(pos, ModelPosition.w + (zoom + (-2.0 + 3.*exp(0.03*instGrowth)))));
    #endif //INSTS_GROW_TO_THE_RIGHT


	//Original Main implementation
	/* 
	vJitter = 1.0;
	vPosition = (position * 2.0) + vec3(
		sin(time),
		cos(time),
		cos(time + 3.14)
	);

	vec2 worl = worley(vPosition, vJitter);
	vec3 pos = position - (length(worl) * normal * 0.2);

	gl_Position = projectionMatrix * modelViewMatrix * vec4(pos, 1.0);
	*/


#elif defined  ALTERNATIVE_VERT 
  #if (ALTERNATIVE_VERT == 1) 
    vec3 pos = ModelPosition.xzy + ModelPosition.yxz;
    vPosition = vec3(max(max(ModelPosition.x, ModelPosition.y), ModelPosition.z), ModelPosition.yz);
    vJitter = 0.0 + 0.1*sin(time);
  
    gl_Position = (MVP *  vec4(pos + 0.05*sin(time + vInst)*ModelNormal, ModelPosition.w +(zoom)));
  #else 
    vec3 pos = ModelPosition.xyz;
    vPosition = ModelPosition.xyz;
    vJitter = 1.0 + 0.1*sin(time);
    gl_Position = (MVP *  vec4(pos + 0.05*sin(time)*ModelNormal, ModelPosition.w +(zoom)));
    //gl_Position.x += (0.95 + gl_InstanceID)*sin(time + gl_InstanceID + 0.25*gl_VertexID);
    //gl_Position.y += (0.95 + gl_InstanceID)*sin(time + gl_InstanceID + 0.25*gl_VertexID);
    //gl_Position.z += 0.0*float(gl_InstanceID);
  #endif //ALTERNATIVE_VERT == n


#endif // ALTERNATIVE_VERT 
    
//    //Scale the position back a bit to allow room for growth
//    gl_Position.xyz *= 0.8;
//    gl_Position.z *= 0.9;
//
//    if (true || gl_InstanceID > 12) {
//       float scale = 1.0 + (0.01 * float(gl_Position));
//       gl_Position.xy /= (scale * worley(ModelNormal, pow(scale, float(gl_InstanceID))));
//       //gl_Position.xz *= (scale * worley(ModelPosition.xyz, pow(scale, float(gl_InstanceID))));
//       //gl_Position.yz *= (scale * worley(vec3(MVP[0][0], MVP[1][0], MVP[2][2]), 15.0 + 4.0*sin(time)));
//    }

    //gl_Position.xyz *= (1.0 / gl_Position.z);
    
    //gl_Position.xy += (0.15 + ((0.25*gl_InstanceID)*sin(time))+(0.6001 * gl_InstanceID));
    //gl_Position.xy += float(gl_InstanceID)* worley(vec3(float(gl_InstanceID), float(gl_VertexID % gl_InstanceID), 5.0*cos(3.915*time)), 15.0 + 15.0*sin(time));



    //float scale = 1.0 + (0.1 * float(gl_Position));
    //float factor = sin(float(gl_InstanceID) * (3.14159 / 12.0));
    //vec2 noise = worley(vec3(sin(time), cos(time), sin(cos(time))), pow(scale, float(gl_InstanceID)));
    //gl_Position.x += abs(abs(noise.x)* factor) - abs(abs(noise.y)*factor);
    //gl_Position.y += abs(abs(noise.y)* factor) - abs(abs(noise.x)*factor);

} 