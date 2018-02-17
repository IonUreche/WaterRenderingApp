#version 400
layout(location=0) in vec3 in_Position;
layout(location=1) in vec3 in_Color;
uniform mat4 mvp;
uniform vec3 pos0;
out vec4 ex_Color;

void main() {
	vec3 newPos = in_Position + pos0;
	gl_Position = mvp * vec4(newPos, 1.0f);
	ex_Color = vec4(in_Color, 1.0f);
}