#version 430 core
#define M_PI 3.1415926535897932384626433832795

layout (quads) in;

layout (binding = 2) uniform sampler2D waterNormalMap;

uniform mat4 mvp;
uniform vec3 lightPos;
uniform mat4 modelView_matrix;
//uniform float time;
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
} tes_out;
//out vec3 lightv;
out vec3 viewv;

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

    gl_Position = mvp * p;
	
    tes_out.tc = tc;
	tes_out.id = tes_in[0].id;

	vec3 texNorm = texture(waterNormalMap, tc).xyz;
	tes_out.norm = texNorm;

	vec3 viewdir = cameraPos - gl_Position.xyz;
	viewdir = normalize(viewdir);
	viewv = viewdir;
}