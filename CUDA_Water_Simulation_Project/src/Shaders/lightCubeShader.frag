#version 400

in vec2 Texcoord;
out vec4 outColor;

uniform sampler2D texFramebuffer;

void main() {
	float mixFactor = 0.5;
	vec3 colorr = vec3(1.0, 0.5, 0.1);
	//outColor = vec4(mix(texture(texFramebuffer, Texcoord).xyz, colorr, mixFactor), 1.0);
	outColor = texture(texFramebuffer, Texcoord);
}