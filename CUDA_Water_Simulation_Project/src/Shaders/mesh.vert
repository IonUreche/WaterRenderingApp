#version 430 core

out VS_OUT
{
	int id;
} vs_out;

uniform int patchSize;
uniform int gridSize;

void main(void)
{
    const vec4 vertices[] = vec4[](vec4(-0.5, 0.0, -0.5, 1.0),
                                   vec4( 0.5, 0.0, -0.5, 1.0),
                                   vec4(-0.5, 0.0,  0.5, 1.0),
                                   vec4( 0.5, 0.0,  0.5, 1.0));

    int x = gl_InstanceID & (gridSize - 1);
    int y = gl_InstanceID / gridSize;

    gl_Position = patchSize * (vertices[gl_VertexID] + vec4(x + 0.5, 0.0, y + 0.5, 0.0));

	vs_out.id = gl_InstanceID;
}