//
//  NOTICE:  This is not my code! This code was available on the internet from 
//           the ShaderFrog website. 
//  SOURCE:   http://shaderfrog.com/view/2843
//
// 
//  Modifications:  Several modificationbs have been made to bring this shader inline with 
//                  something that the renderer will compile. Any changes made will be marked.
//                  These changes will just be variable renaming.
//

#version 460 core    ///Added by FSM
///precision highp float;    ///Removed by FSM
///precision highp int;      ///Removed by FSM

// Based off https://github.com/Erkaman/glsl-worley

uniform float time;

uniform float instanceSpiralPatternPeriod_x;
uniform float instanceSpiralPatternPeriod_y;

uniform mat4 rotation;

vec3 color1 = vec3(abs(cos(instanceSpiralPatternPeriod_x)), 0.25 + abs(0.75*cos(0.923*instanceSpiralPatternPeriod_x)), 0.800 - 0.37 * abs(cos(instanceSpiralPatternPeriod_x)));   ///uniform vec3 color1;     ///Modified by FSM
vec3 color2 = vec3(0.095, 0.09972, 0.9975);   ///uniform vec3 color2;     ///Modified by FSM 


in vec3 vPosition;      ///varying vec3 vPosition;     ///Modified by FSM
in float vJitter;       ///varying float vJitter;      ///Modified by FSM
in flat int vInst;

out vec4 outColor;   ///Added by FSM

//#define time time*5.0

// Permutation polynomial: (34x^2 + x) mod 289
vec3 permute(vec3 x) {
	return mod((34.0 * x + 1.0) * x, 289.0);
}

vec3 dist(vec3 x, vec3 y, vec3 z) {
	return x * x + y * y + z * z;
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

void main() {

        vec2 worl = worley(vPosition, vJitter);
        float world = worl.y * worl.x;

        vec3 color = mat3(rotation + rotation + rotation) * mix(color1, color2 + vec3(worley(color1.rgb, gl_FragCoord.x) - worley(color1.gbr, gl_FragCoord.x + instanceSpiralPatternPeriod_y), sin(0.1*time)+0.0), clamp(world * 2.0, 0.0, 1.0));
        //color.r = color.r + 1./1.*color.r*sin(time * 500.);
        //color.r += abs(0.15 * cos(instanceSpiralPatternPeriod_y * 1.2));

        ///gl_FragColor = vec4((color * 0.1) + (color * world), 1.0);   ///Modified by FSM
        outColor = fwidthFine(vec4((color * 0.1) + 10.0*(color * world * gl_FragCoord.x), 1.0) - vec4(0.0, 0.0, 0.5, 0.0));  ///Added by FSM

        outColor *= (2.09 + 0.35*sin(time)) / length(outColor);


        outColor.a = 0.12 * smoothstep(-90.0, 29.0, length(outColor.rgb + color.grg) + (10.0 + 5.0*abs(sin(15.0*time))));//0.24;
        //if (vInst > 500) {
        //	float dropoff = pow(0.9999, float(vInst - 499));
        //	outColor.a *= dropoff;
        //}

        //outColor.rb += gl_SamplePosition;
        //outColor.g += 5000.0*(outColor.r * gl_SampleID);

        outColor.rgb *= mix(outColor.rgb, vec3(0.5 + 0.5*sin(time), worley(outColor.rgb, vJitter)), abs(sin(3.14159 * time / 18.0)));


        if (length(outColor.rgb) < 0.5) {
            outColor.rgb = vec3(1.2) - outColor.rgb;
        }
        outColor.r = (outColor.r + 0.25*sin(time + (50000.0*vPosition.x))); 
        
       
}