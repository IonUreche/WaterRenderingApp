#version 430 core
#define M_PI 3.1415926535897932384626433832795

layout (quads) in;

layout (binding = 0) uniform sampler2D tex_displacement;
layout (binding = 2) uniform sampler2D waterNormalMap;

uniform mat4 mvp;
uniform vec3 lightPos;
uniform mat4 modelView_matrix;
uniform mat4 proj_matrix;
//uniform float dmap_depth;
uniform float time;
uniform float waves[24];
uniform vec3 cameraPos;

in TCS_OUT
{
    vec2 tc;
	float id;
} tes_in[];

out TES_OUT
{
    vec2 tc;
	float id;
	vec3 norm;
	vec3 viewv;
} tes_out;
out vec3 lightv;
out vec3 viewv;

// ********* WAVE FUNCTION ****************
void wave_function(in float waves[24], in float time, in vec3 pos,
					out vec3 P, out vec3 N, out vec3 B, out vec3 T)
{
    float PI = 3.14159265358979323846264;
    P = pos;
    for (int i = 0; i < 24; i += 6) 
	{
        float A = waves[i] * waves[i+3];        // Amplitude
        float omega = 2.0 * PI / waves[i];       // Frequency
        float phi = waves[i+2] * omega;          // Phase
        float Qi = waves[i+1]/(omega * A * 4.0); // Steepness

        float term = omega * dot(vec2(waves[i+4], waves[i+5]), vec2(pos.x, pos.z)) + phi * time;
        float C = cos(term);
        float S = sin(term);
        P += vec3(Qi * A * waves[i+4] * C,
                  A * S,
                  Qi * A * waves[i+5] * C);
    }
    B = vec3(0.0);
    T = vec3(0.0);
    N = vec3(0.0);
    for (int i = 0; i < 24; i += 6) 
	{
        float A = waves[i] * waves[i+3];         // Amplitude
        float omega = 2.0 * PI / waves[i];       // Frequency
        float phi = waves[i+2] * omega;          // Phase
        float Qi = waves[i+1]/(omega * A * 4.0); // Steepness

        float WA = omega * A;
        float term = omega * dot(vec2(waves[i+4], waves[i+5]), vec2(P.x, P.z)) + phi * time;
        float C = cos(term);
        float S = sin(term);
        B += vec3 (Qi * waves[i+4]*waves[i+4] * WA * S,
                   Qi * waves[i+4] * waves[i+5] * WA * S,
                   waves[i+4] * WA * C);

        T += vec3 (Qi * waves[i+4] * waves[i+5] * WA * S,
                   Qi * waves[i+5] * waves[i+5] * WA * S,
                   waves[i+5] * WA * C);

        N += vec3 (waves[i+4] * WA * C,
                   waves[i+5] * WA * C,
                   Qi * WA * S);
    }
    B = normalize(vec3(1.0 - B.x, -B.y, B.z));
    T = normalize(vec3(-T.x, 1.0 - T.y, T.z));
    N = normalize(vec3(-N.x, -N.y, 1.0 - N.z));
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
	//float arg = sqrt(p.x * p.x + p.z * p.z);
    //p.y += 10 * sin(time + arg) / arg; //texture(tex_displacement, tc).r * 2 * sin(time * p.x) + cos(time * p.z);
	//p.y = texture(waterNormalMap, tc).r;
	//ps = p;

	vec3 P, N, B, T;
    wave_function(waves, time, p.xyz, P, N, B, T);
    lightv = vec3(dot(lightPos, B),
				  dot(lightPos, T),
				  dot(lightPos, N));
    lightv = normalize(lightv);


	vec3 pos = (modelView_matrix * vec4(P.xyz, 1.0)).xyz;
	viewv = vec3(dot(pos, B),
				 dot(pos, T),
				 dot(pos, N));
    viewv = normalize(viewv);

	p = vec4(P, 1.0);

    gl_Position = mvp * p;
	
    tes_out.tc = tc;
	tes_out.id = tes_in[0].id;
	tes_out.norm = N;
	vec3 viewdir = cameraPos - gl_Position.xyz;
	viewdir = normalize(viewdir);
	tes_out.viewv = viewdir;
}