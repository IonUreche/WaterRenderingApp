#version 400
layout(location=0) in vec3 in_Position;
uniform mat4 mvp;
uniform vec3 pos0;

void main() {
	vec3 newPos = in_Position + pos0;
	gl_Position = mvp * vec4(newPos, 1.0f);
}