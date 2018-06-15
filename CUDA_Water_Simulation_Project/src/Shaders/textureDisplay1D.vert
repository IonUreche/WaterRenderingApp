#version 400

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 UV;

out vec2 uv;

uniform int flipV;

void main(){
	uv = UV;
	if(flipV == 1)
	{
		uv.y = 1 - uv.y;
	}
	gl_Position = vec4(pos, 0.0, 1.0);
}
