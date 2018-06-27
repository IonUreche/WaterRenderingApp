#version 430 core
#define M_PI 3.1415926535897932384626433832795

layout (quads) in;

uniform mat4 mvp;
uniform vec3 lightPos;
uniform mat4 modelView_matrix;
uniform float time;
uniform float waves[24];
uniform vec3 cameraPos;

in TCS_OUT
{
    vec2 tc;
} tes_in[];

out TES_OUT
{
    vec2 tc;
	vec3 norm;
	vec3 viewv;
} tes_out;
out vec3 lightv;
//out vec3 viewv;

// ********* WAVE FUNCTION ****************
void wave_function(in float waves[24], in float time, in vec3 pos,
					out vec3 P, out vec3 N, out vec3 B, out vec3 T)
{
    float PI = 3.14159265358979323846264;
    P = pos;
    for (int i = 0; i < 24; i += 6) 
	{
        float A = waves[i] * waves[i+1];         // Amplitude
        float omega = 2.0 * PI / waves[i];       // Frequency
        float phi = waves[i+5] * omega;          // Phase
        float Qi = waves[i+4]/(omega * A * 4.0); // Steepness

        float term = omega * dot(vec2(waves[i+2], waves[i+3]), vec2(pos.x, pos.z)) + phi * time;
        float C = cos(term);
        float S = sin(term);
        P += vec3(Qi * A * waves[i+2] * C,
                  A * S,
                  Qi * A * waves[i+3] * C);
    }
    B = vec3(0.0);
    T = vec3(0.0);
    N = vec3(0.0);
    for (int i = 0; i < 24; i += 6) 
	{
        float A = waves[i] * waves[i+1];         // Amplitude
        float omega = 2.0 * PI / waves[i];       // Frequency
        float phi = waves[i+5] * omega;          // Phase
        float Qi = waves[i+4]/(omega * A * 4.0); // Steepness

        float WA = omega * A;
        float term = omega * dot(vec2(waves[i+2], waves[i+3]), vec2(P.x, P.z)) + phi * time;
        float C = cos(term);
        float S = sin(term);
        B += vec3 (Qi * waves[i+2]*waves[i+2] * WA * S,
                   Qi * waves[i+2] * waves[i+3] * WA * S,
                   waves[i+2] * WA * C);

        T += vec3 (Qi * waves[i+2] * waves[i+3] * WA * S,
                   Qi * waves[i+3] * waves[i+3] * WA * S,
                   waves[i+3] * WA * C);

        N += vec3 (waves[i+2] * WA * C,
                   waves[i+3] * WA * C,
                   Qi * WA * S);
    }
    B = normalize(vec3(1.0 - B.x, -B.y, B.z));
    T = normalize(vec3(-T.x, 1.0 - T.y, T.z));
    N = normalize(vec3(-N.x, -N.y, 1.0 - N.z));
}

//	Classic Perlin 3D Noise 
//	by Stefan Gustavson
//
vec4 permute(vec4 x){return mod(((x*34.0)+1.0)*x, 289.0);}
vec4 taylorInvSqrt(vec4 r){return 1.79284291400159 - 0.85373472095314 * r;}
vec3 fade(vec3 t) {return t*t*t*(t*(t*6.0-15.0)+10.0);}

float cnoise(vec3 P){
  vec3 Pi0 = floor(P); // Integer part for indexing
  vec3 Pi1 = Pi0 + vec3(1.0); // Integer part + 1
  Pi0 = mod(Pi0, 289.0);
  Pi1 = mod(Pi1, 289.0);
  vec3 Pf0 = fract(P); // Fractional part for interpolation
  vec3 Pf1 = Pf0 - vec3(1.0); // Fractional part - 1.0
  vec4 ix = vec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
  vec4 iy = vec4(Pi0.yy, Pi1.yy);
  vec4 iz0 = Pi0.zzzz;
  vec4 iz1 = Pi1.zzzz;

  vec4 ixy = permute(permute(ix) + iy);
  vec4 ixy0 = permute(ixy + iz0);
  vec4 ixy1 = permute(ixy + iz1);

  vec4 gx0 = ixy0 / 7.0;
  vec4 gy0 = fract(floor(gx0) / 7.0) - 0.5;
  gx0 = fract(gx0);
  vec4 gz0 = vec4(0.5) - abs(gx0) - abs(gy0);
  vec4 sz0 = step(gz0, vec4(0.0));
  gx0 -= sz0 * (step(0.0, gx0) - 0.5);
  gy0 -= sz0 * (step(0.0, gy0) - 0.5);

  vec4 gx1 = ixy1 / 7.0;
  vec4 gy1 = fract(floor(gx1) / 7.0) - 0.5;
  gx1 = fract(gx1);
  vec4 gz1 = vec4(0.5) - abs(gx1) - abs(gy1);
  vec4 sz1 = step(gz1, vec4(0.0));
  gx1 -= sz1 * (step(0.0, gx1) - 0.5);
  gy1 -= sz1 * (step(0.0, gy1) - 0.5);

  vec3 g000 = vec3(gx0.x,gy0.x,gz0.x);
  vec3 g100 = vec3(gx0.y,gy0.y,gz0.y);
  vec3 g010 = vec3(gx0.z,gy0.z,gz0.z);
  vec3 g110 = vec3(gx0.w,gy0.w,gz0.w);
  vec3 g001 = vec3(gx1.x,gy1.x,gz1.x);
  vec3 g101 = vec3(gx1.y,gy1.y,gz1.y);
  vec3 g011 = vec3(gx1.z,gy1.z,gz1.z);
  vec3 g111 = vec3(gx1.w,gy1.w,gz1.w);

  vec4 norm0 = taylorInvSqrt(vec4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
  g000 *= norm0.x;
  g010 *= norm0.y;
  g100 *= norm0.z;
  g110 *= norm0.w;
  vec4 norm1 = taylorInvSqrt(vec4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
  g001 *= norm1.x;
  g011 *= norm1.y;
  g101 *= norm1.z;
  g111 *= norm1.w;

  float n000 = dot(g000, Pf0);
  float n100 = dot(g100, vec3(Pf1.x, Pf0.yz));
  float n010 = dot(g010, vec3(Pf0.x, Pf1.y, Pf0.z));
  float n110 = dot(g110, vec3(Pf1.xy, Pf0.z));
  float n001 = dot(g001, vec3(Pf0.xy, Pf1.z));
  float n101 = dot(g101, vec3(Pf1.x, Pf0.y, Pf1.z));
  float n011 = dot(g011, vec3(Pf0.x, Pf1.yz));
  float n111 = dot(g111, Pf1);

  vec3 fade_xyz = fade(Pf0);
  vec4 n_z = mix(vec4(n000, n100, n010, n110), vec4(n001, n101, n011, n111), fade_xyz.z);
  vec2 n_yz = mix(n_z.xy, n_z.zw, fade_xyz.y);
  float n_xyz = mix(n_yz.x, n_yz.y, fade_xyz.x); 
  return 2.2 * n_xyz;
}

void main(void)
{
    vec2 tc1 = mix(tes_in[0].tc, tes_in[1].tc, gl_TessCoord.x);
    vec2 tc2 = mix(tes_in[2].tc, tes_in[3].tc, gl_TessCoord.x);
    vec2 tc = mix(tc2, tc1, gl_TessCoord.y);

    vec4 p1 = mix(gl_in[0].gl_Position,
                  gl_in[1].gl_Position,
                  gl_TessCoord.x);
    vec4 p2 = mix(gl_in[2].gl_Position,
                  gl_in[3].gl_Position,
                  gl_TessCoord.x);
    vec4 p = mix(p2, p1, gl_TessCoord.y);

	vec3 P, N, B, T;
    wave_function(waves, time, p.xyz, P, N, B, T);
    lightv = vec3(dot(lightPos, B),
				  dot(lightPos, T),
				  dot(lightPos, N));
    lightv = normalize(lightv);


	vec3 pos = (modelView_matrix * vec4(P.xyz, 1.0)).xyz;
	//viewv = vec3(dot(pos, B),
	//			 dot(pos, T),
	//			 dot(pos, N));
    //viewv = normalize(viewv);

	//P.x = p.x;
	//P.z = p.z;
	//P.y = cnoise(vec3(p.x * 0.2, p.z  * 0.3, time * 0.1));

	p = vec4(P, 1.0);

    gl_Position = mvp * p;
	
    tes_out.tc = tc;
	tes_out.norm = N;
	vec3 viewdir = cameraPos - gl_Position.xyz;
	viewdir = normalize(viewdir);
	tes_out.viewv = viewdir;
	//viewv = viewdir;
}