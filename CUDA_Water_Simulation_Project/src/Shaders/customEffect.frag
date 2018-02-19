#version 400

in vec2 uv;
in vec2 Pos;
out vec4 color;

uniform float time;
uniform sampler2D renderedTexture;

void main(){
    //color = vec4(texture( renderedTexture, uv + 0.005*vec2( sin(time+1024.0*uv.x),cos(time+768.0*uv.y)) ).xyz, 1.0);
	vec3 texColor = texture(renderedTexture, uv).xyz;
	color = vec4(texColor, 1.0);
}