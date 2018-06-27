#version 430 core
#define M_PI 3.1415926535897932384626433832795

layout (quads) in;

uniform mat4 mvp;
uniform float choppy;
//uniform vec3 lightPos;
//uniform mat4 modelView_matrix;
//uniform float time;
//uniform float waves[24];
//uniform vec3 cameraPos;

layout (binding = 0) uniform sampler2D displ_y;
layout (binding = 1) uniform sampler2D displ_x;
layout (binding = 2) uniform sampler2D displ_z;

in TCS_OUT
{
    vec2 tc;
} tes_in[];

out TES_OUT
{
    vec2 tc;
	//vec3 norm;
	//vec3 viewv;
} tes_out;
//out vec3 lightv;
//out vec3 viewv;

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

    
    p.y += texture(displ_y, vec2(p.x / 128.0, p.z / 128.0)).r;
    if(choppy > 0)
    {
        p.x += texture(displ_x, vec2(p.x / 128.0, p.z / 128.0)).r;
        p.z += texture(displ_z, vec2(p.x / 128.0, p.z / 128.0)).r;
    }
    

    gl_Position = mvp * p;
	
    tes_out.tc = tc;
	//tes_out.norm = N;
	//vec3 viewdir = cameraPos - gl_Position.xyz;
	//viewdir = normalize(viewdir);
	//tes_out.viewv = viewdir;
	//viewv = viewdir;
}