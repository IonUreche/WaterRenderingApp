#version 400

in vec3 position;
out vec3 TexCoords;

uniform mat4 mvp;

void main()
{
    gl_Position = mvp * vec4(position, 1.0);
    TexCoords = normalize(position);
}  