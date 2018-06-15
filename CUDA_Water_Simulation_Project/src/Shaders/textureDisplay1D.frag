#version 430 core

in vec2 uv;
layout(location = 0) out vec4 color;

uniform sampler2D renderedTexture;

float LinearizeDepth(float zoverw){
		float n = 1; // camera z near
		float f = 10000.0; // camera z far
		//return (2.0 * n) / (f + n - zoverw * (f - n));
		return 2.0 * n * f / (f + n - (2.0 * zoverw - 1.0) * (f - n));
	}

void main(){
	float depth = texture(renderedTexture, uv).r;
	depth = LinearizeDepth(depth) / 90.0;
	color = vec4(depth, depth, depth, 1.0);
}