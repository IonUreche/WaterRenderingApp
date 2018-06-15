#version 430 core

layout (binding = 1) uniform sampler2D rock;
layout (binding = 2) uniform sampler2D grass;
layout (binding = 3) uniform sampler2D noise;

in TES_OUT
{
    vec2 tc;
	float id;
	vec3 norm;
	vec2 uv;
	vec2 w_uv;
	vec4 color;
} fs_in;
layout (location = 0) out vec4 out_color;
layout (location = 1) out float out_depth;

//uniform int gridSize;
//uniform int debug_mode_enabled = 0;
//in vec3 lightv;
//in vec3 viewv;

// 2D Random
float random (in vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))
                 * 43758.5453123);
}

void main(void)
{
	//int id = fs_in.id;
	//int x = id & (gridSize - 1);
    //int y = id / gridSize;
	//float u = (float(x) / float(gridSize));
	//float v = (float(y) / float(gridSize));
	//vec3 clr = texture(heightMap, fs_in.tc).rgb;
	//color = vec4(clr, 1.0f);

	vec3 noiseVal = texture2D(noise, fs_in.w_uv).rgb;
	vec3 rockVal = texture2D(rock, fs_in.tc).rgb;
	vec3 grassVal = texture2D(grass, fs_in.tc).rgb;
	vec4 newCol = vec4(grassVal, 1.0f);
	//vec4 newCol = vec4(mix(grassVal, rockVal, random(fs_in.w_uv)), 1.0f);
	//vec4 newCol = vec4(noiseVal.r, noiseVal.r, noiseVal.r, 1.0f);

	out_color = newCol;
	out_depth = gl_FragCoord.z;

	//color = vec4(1.0f, clr.g, 1.0f, 1.0f);
	//color = vec4(1.0f, 1.0f, 0.0f, 1.0f);
	//float b = fs_in.id / 64;
	//float r = fs_in.id - abs(floor(abs(b))) * 64;
	//float redV = clamp(tan(abs(r) / 64), 0.0001f, 0.999f);
	//float blueV = clamp(tan(abs(b) / 64), 0.0001f, 0.999f);
	//color = vec4(redV, 0.0f, blueV, 1.0f);
	//color = texture(waterColorMap, fs_in.tc);
}






//if(debug_mode_enabled != 0) // visualize the normals by color
	//{
	//	color = vec4(fs_in.norm, 1.0);
	//}
	//else
	//{
	//	//vec3 N = texture2D(normalmap, texcoord*0.125).xyz * 2.0 - 1.0;
    //    vec3 N = fs_in.norm;//normalize(N);
    //    vec3 specular = vec3(1.0) * pow(clamp(dot(reflect(normalize(lightv), N), viewv), 0.0, 1.0), 50.0);
    //    vec3 oceanblue = vec3(0.0, 0.25, 0.63);
    //    vec3 skyblue = vec3(0.39, 0.52, 0.93) * 0.9;
    //    const float R_0 = 0.4;
    //    float fresnel = R_0 + (1.0 - R_0) * pow((1.0 - dot(-normalize(viewv), N)), 5.0);
    //    fresnel = max(0.0, min(fresnel, 1.0));
	//	vec3 refl = reflect(-fs_in.viewv, N);
	//	vec3 skyReflColor = 0.5 * texture(skybox, refl).xyz;
    //    color = vec4(mix(oceanblue, skyReflColor, fresnel) + specular, 1.0);
	//	//color = texture(skybox, N);
	//	//color2 = color + vec4(0.5, 0.0, 0.0, 0.0);
	//	//color = vec4(1.0, 1.0, 0.0, 1.0);
	//}