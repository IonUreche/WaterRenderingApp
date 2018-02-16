#version 430 core
in vec4 ps;

out vec4 color;

layout (binding = 1) uniform sampler2D waterColorMap;

in TES_OUT
{
    vec2 tc;
	float id;
	vec3 norm;
} fs_in;


// 2D Random
float random (in vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))
                 * 43758.5453123);
}

// 2D Noise based on Morgan McGuire @morgan3d
// https://www.shadertoy.com/view/4dS3Wd
float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    // Smooth Interpolation

    // Cubic Hermine Curve.  Same as SmoothStep()
    vec2 u = f*f*(3.0-2.0*f);
    // u = smoothstep(0.,1.,f);

    // Mix 4 coorners porcentages
    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}


void main(void)
{
	//float b = fs_in.id / 64;
	//float r = fs_in.id - abs(floor(abs(b))) * 64;
	//float redV = clamp(tan(abs(r) / 64), 0.0001f, 0.999f);
	//float blueV = clamp(tan(abs(b) / 64), 0.0001f, 0.999f);

	//color = vec4(redV, 0.0f, blueV, 1.0f);

	//color = texture(waterColorMap, fs_in.tc);
	color = vec4(fs_in.norm, 1.0);
}