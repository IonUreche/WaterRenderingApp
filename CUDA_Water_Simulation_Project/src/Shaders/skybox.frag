#version 400

in vec3 TexCoords;
layout (location = 0) out vec4 color;
layout (location = 1) out vec4 BrightColor;

uniform samplerCube skybox;

void main()
{    
    color = texture(skybox, TexCoords);
	BrightColor = BrightColor = vec4(1.0, 1.0, 1.0, 1.0);
}