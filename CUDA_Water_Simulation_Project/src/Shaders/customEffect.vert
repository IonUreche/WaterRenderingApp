#version 400

layout(location=0) in vec2 pos;
layout(location=1) in vec2 UV;

out vec2 uv;
out vec2 poss;
//uniform mat4 orto_matrix;
uniform float time;

void main(){
	uv = UV;
	//uv = vec2(abs(sin(time)), abs(cos(time)));
	poss = /*orto_matrix * */pos;
	gl_Position = vec4(poss, 0.0, 1.0);
}
