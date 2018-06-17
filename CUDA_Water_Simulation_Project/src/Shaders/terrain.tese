#version 430 core
#define M_PI 3.1415926535897932384626433832795

layout (quads) in;

layout (binding = 1) uniform sampler2D rock;
layout (binding = 5) uniform sampler2D hhh;

uniform mat4 mvp;
//uniform vec3 lightPos;
//uniform mat4 modelView_matrix;
//uniform float time;
uniform vec3 cameraPos;

uniform int gridSize;
uniform int patchSize;

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
	vec2 uv;
	vec2 w_uv;
	vec4 color;
} tes_out;
//out vec3 lightv;
//out vec3 viewv;

// vec4(0, -1, 0, 0); for subwater
uniform vec4 plane;

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

	int id = int(round(tes_in[0].id));
	//int x = id & (gridSize - 1);
    //int y = id / gridSize;
	float patchSizeF = float(patchSize);
	float gridSizeF = float(patchSize);
	float u = clamp((p.x / (gridSize * patchSize)), 0.0, 0.9999);
	float v = clamp((p.z / (gridSize * patchSize)), 0.0, 0.9999);

	tes_out.w_uv = vec2(u,v);

	float u0 = mod(p.x, patchSizeF) / patchSizeF;
	float v0 = mod(p.y, patchSizeF) / patchSizeF;

	vec3 heightSample = texture2D(hhh, vec2(u,v)).xyz;
	p.y = 9 + (-heightSample.r * 40.0f);

    gl_Position = mvp * p;
	gl_ClipDistance[0] = dot(p, plane);

    tes_out.tc = tc;
	tes_out.id = tes_in[0].id;
	tes_out.uv = vec2(u, v);
	vec3 ffg = texture2D(rock, vec2(u,v)).rgb;
	tes_out.color = vec4(ffg, 1.0f);

	vec3 texNorm = vec3(0.0f, 1.0f, 0.0f);
	tes_out.norm = texNorm;

	//vec3 viewdir = cameraPos - gl_Position.xyz;
	//viewdir = normalize(viewdir);
	//tes_out.viewv = viewdir;
}