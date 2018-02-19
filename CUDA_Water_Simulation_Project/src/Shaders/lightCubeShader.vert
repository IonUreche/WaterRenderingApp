#version 400
layout(location=0) in vec3 in_Position;
layout(location=1) in vec2 texcoord;
uniform mat4 mvp;
uniform vec3 pos0;
out vec2 Texcoord;

void main() {
	Texcoord = texcoord;

	vec3 newPos = in_Position + pos0;
	gl_Position = mvp * vec4(newPos, 1.0f);
}