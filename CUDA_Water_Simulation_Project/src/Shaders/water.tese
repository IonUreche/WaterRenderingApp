#version 430 core
#define M_PI 3.1415926535897932384626433832795

layout (quads) in;

uniform mat4 mvp;
uniform mat4 view;

uniform int gridSize;
uniform int patchSize;

in TCS_OUT
{
    vec2 tc;
} tes_in[];

out TES_OUT
{
	vec3 worldPosition;
    vec2 texCoords;
} tes_out;

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

	vec4 pss = mvp * p;
    gl_Position = pss;

	//tes_out.ndc = vec2(pss.xy / pss.w);
    tes_out.texCoords = tc;

	float patchSizeF = float(patchSize);
	float gridSizeF = float(patchSize);
	float u = (p.x / (gridSize * patchSize));
	float v = (p.z / (gridSize * patchSize));

	float u0 = mod(p.x, patchSizeF) / patchSizeF;
	float v0 = mod(p.y, patchSizeF) / patchSizeF;

	tes_out.worldPosition = p.xyz;
	vec3 pdrw = p.xyz;
}