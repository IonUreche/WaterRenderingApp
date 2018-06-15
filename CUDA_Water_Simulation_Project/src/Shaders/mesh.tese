#version 430 core

layout (quads) in;

uniform mat4 mvp;

in TCS_OUT
{
	int id;
} tes_in[];
out TES_OUT
{
	vec4 color;
} tes_out;

// 2D Random
float random (in vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))
                 * 43758.5453123);
}
vec4 GetRandomColor(vec2 rnd_clr)
{
	float r = random(rnd_clr);
	float g = random(rnd_clr + r);
	float b = random(rnd_clr + g);
	return vec4(r, g, b, 1.0f);
}
void main(void)
{
	vec4 p1 = mix(gl_in[0].gl_Position,
                  gl_in[1].gl_Position,
                  gl_TessCoord.x);
    vec4 p2 = mix(gl_in[2].gl_Position,
                  gl_in[3].gl_Position,
                  gl_TessCoord.x);
    vec4 p = mix(p2, p1, gl_TessCoord.y);
	p.y = 25 * GetRandomColor(vec2(tes_in[0].id, tes_in[0].id)).x;
    gl_Position = mvp * p;
	tes_out.color = GetRandomColor(vec2(tes_in[0].id, tes_in[0].id));
}