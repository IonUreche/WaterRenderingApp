#version 430 core
#define M_PI 3.1415926535897932384626433832795

layout (quads) in;

//layout (binding = 1) uniform sampler2D rock;
//layout (binding = 5) uniform sampler2D hhh;

uniform mat4 mvp;
uniform mat4 view;
//uniform vec3 lightPos;
//uniform mat4 modelView_matrix;
//uniform float time;

uniform int gridSize;
uniform int patchSize;

in TCS_OUT
{
    vec2 tc;
} tes_in[];

out TES_OUT
{
	vec3 w_p;
    vec2 tc;
	vec2 ndc;
	//vec3 viewVector;
	//vec3 fromLightVector;
} tes_out;
//out vec3 lightv;
//out vec3 viewv;

//out vec3 viewVector2;

uniform vec4 plane;

uniform vec3 cameraPos;
uniform vec3 lightPos;

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
	//p = vec4(cameraPos - p.xyz, 0.0f);
	vec4 pss = mvp * p;
    gl_Position = pss;

	tes_out.ndc = vec2(pss.xy / pss.w);
    tes_out.tc = tc;

	float patchSizeF = float(patchSize);
	float gridSizeF = float(patchSize);
	float u = (p.x / (gridSize * patchSize));
	float v = (p.z / (gridSize * patchSize));

	//tes_out.w_uv = vec2(u,v);

	float u0 = mod(p.x, patchSizeF) / patchSizeF;
	float v0 = mod(p.y, patchSizeF) / patchSizeF;

	//gl_ClipDistance[0] = dot(p, plane);

	//vec3 viewPos = cameraPos - p.xyz;
	//tes_out.viewVector = viewPos.xyz;
	tes_out.w_p = p.xyz;

	vec3 pdrw = p.xyz;
	
	//viewVector2 = normalize(cameraPos - p.xyz);
	//tes_out.fromLightVector = normalize(lightPos - p.xyz);

	//tes_out.worldPos = p.xyz;
	//vec3 viewVector = normalize(cameraPos - p.xyz);
	//tes_out.frenel = dot(viewVector, vec3(0.0f, 1.0f, 0.0f));
	//tes_out.fromLightVector = lightPos - p.xyz;
}