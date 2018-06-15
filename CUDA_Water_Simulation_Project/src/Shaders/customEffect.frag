#version 430 core

in vec2 uv;
//in vec2 Pos;
layout(location = 0) out vec4 color;

//uniform float time;
uniform sampler2D renderedTexture;

void main(){
    //color = vec4(texture( renderedTexture, uv + 0.005*vec2( sin(time+1024.0*uv.x),cos(time+768.0*uv.y)) ).xyz, 1.0);
	color = texture(renderedTexture, uv);
	//color = vec4(1.0f, 0.5f, 0.3f, 1.0f);
}