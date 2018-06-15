#version 430 core

in TES_OUT
{
	vec4 color;
} fs_in;
layout (location = 0) out vec4 out_color;
void main(void)
{
	out_color = fs_in.color;
}