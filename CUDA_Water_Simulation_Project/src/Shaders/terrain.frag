#version 430 core

layout (binding = 1) uniform sampler2D waterColorMap;
layout (binding = 3) uniform samplerCube skybox;

in TES_OUT
{
    vec2 tc;
	float id;
	vec3 norm;
	vec3 viewv;
} fs_in;
layout (location = 0) out vec4 color;

uniform int debug_mode_enabled = 0;


in vec3 lightv;
in vec3 viewv;

void main(void)
{
	//float b = fs_in.id / 64;
	//float r = fs_in.id - abs(floor(abs(b))) * 64;
	//float redV = clamp(tan(abs(r) / 64), 0.0001f, 0.999f);
	//float blueV = clamp(tan(abs(b) / 64), 0.0001f, 0.999f);

	//color = vec4(redV, 0.0f, blueV, 1.0f);

	//color = texture(waterColorMap, fs_in.tc);

	if(debug_mode_enabled != 0) // visualize the normals by color
	{
		color = vec4(fs_in.norm, 1.0);
	}
	else
	{
		//vec3 N = texture2D(normalmap, texcoord*0.125).xyz * 2.0 - 1.0;
        vec3 N = fs_in.norm;//normalize(N);
        vec3 specular = vec3(1.0) * pow(clamp(dot(reflect(normalize(lightv), N), viewv), 0.0, 1.0), 50.0);
        vec3 oceanblue = vec3(0.0, 0.0, 0.2);
        vec3 skyblue = vec3(0.39, 0.52, 0.93) * 0.9;
        const float R_0 = 0.4;
        float fresnel = R_0 + (1.0 - R_0) * pow((1.0 - dot(-normalize(viewv), N)), 5.0);
        fresnel = max(0.0, min(fresnel, 1.0));
		vec3 refl = reflect(-fs_in.viewv, N);
		vec3 skyReflColor = 0.5 * texture(skybox, refl).xyz;
        color = vec4(mix(oceanblue, skyReflColor, fresnel) + specular, 1.0);
		//color = texture(skybox, N);
		//color2 = color + vec4(0.5, 0.0, 0.0, 0.0);
		//color = vec4(1.0, 1.0, 0.0, 1.0);
	}
}



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