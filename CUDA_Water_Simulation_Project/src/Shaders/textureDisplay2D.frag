#version 430 core

in vec2 uv;
layout(location = 0) out vec4 color;

uniform sampler2D renderedTexture;

void main(){
	color = texture2D(renderedTexture, uv);
}