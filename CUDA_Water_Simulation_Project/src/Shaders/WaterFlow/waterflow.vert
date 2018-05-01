#version 430 core

out VS_OUT
{
    vec2 tc;
	float id;
} vs_out;

void main(void)
{
    const vec4 vertices[] = vec4[](vec4(-0.5, 0.0, -0.5, 1.0),
                                   vec4( 0.5, 0.0, -0.5, 1.0),
                                   vec4(-0.5, 0.0,  0.5, 1.0),
                                   vec4( 0.5, 0.0,  0.5, 1.0));

    int x = gl_InstanceID & 63;
    int y = gl_InstanceID >> 6;

    vs_out.tc = (vertices[gl_VertexID].xz + vec2(0.5));
    gl_Position = 8 * (vertices[gl_VertexID] + vec4(x + 0.5, 0.0, y + 0.5, 0.0));

	vs_out.id = float(gl_InstanceID);
}