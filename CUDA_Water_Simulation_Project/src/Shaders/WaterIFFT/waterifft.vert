#version 430 core

out VS_OUT
{
    vec2 tc;
} vs_out;

uniform float patchSize;
uniform int gridSize;

void main(void)
{
    const vec4 vertices[] = vec4[](vec4(0, 0, 0, 1),
                                   vec4(1, 0, 0, 1),
                                   vec4(0, 0, 1, 1),
                                   vec4(1, 0, 1, 1));

    int x = gl_InstanceID & (gridSize - 1);
    int y = gl_InstanceID / gridSize;

    vs_out.tc = vertices[gl_VertexID].xz;
    vec3 pos3 = patchSize * (vertices[gl_VertexID].xyz + vec3(x, 0.0, y));
	vec4 pos = vec4(pos3, 1.0);
	gl_Position = pos;
}