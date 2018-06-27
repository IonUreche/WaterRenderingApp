#version 430 core

layout (binding = 1) uniform sampler2D normalMap;
layout (binding = 2) uniform sampler2D reflectionTexture;
//layout (binding = 3) uniform samplerCube skyboxTexture;
layout (binding = 4) uniform sampler2D dudvMap;

in TES_OUT
{
    vec2 tc;
	vec3 norm;
	vec3 viewv;
} fs_in;
layout (location = 0) out vec4 color;
layout (location = 1) out vec4 BrightColor;

uniform int debug_mode_enabled = 0;
uniform vec2 screenResolution;
uniform float waves_nm[24];
uniform float time;
uniform float moveFactor;

const float distortStrength = 0.04f;

uniform vec3 lightPos;

const float shineDamper = 20.0;
const float reflectivity = 0.4;

in vec3 lightv;
//in vec3 viewv;


void calc_normal(in vec2 uv, out vec3 N)
{
   float PI = 3.14159265358979323846264;
   N = vec3(0.0, 0.0, 1.0);
   for (int i = 0; i < 24; i += 6) {
       float A = waves_nm[i] * waves_nm[i+3];         // Amplitude
       float omega = 2.0 * PI / waves_nm[i];       // Frequency
       float phi = waves_nm[i+2] * omega;          // Phase
       float k = waves_nm[i+1];
       float term = omega * dot(vec2(waves_nm[i+4], waves_nm[i+5]), uv) + phi * time;
       float C = cos(term);
       float S = sin(term);
       float val = pow(0.5 * (S + 1.0), k - 1.0) * C;
       val = omega * A * k * val;
       N += vec3(waves_nm[i+4] * val,
                 waves_nm[i+5] * val,
                 0.0);
   }
   N = normalize(N); 
}

void main(void)
{
	if(debug_mode_enabled != 0) // visualize the normals by color
	{
		//vec3 N;
		//calc_normal(gl_FragCoord.st/128.0, N);
		//N = (N * 0.5) + 0.5;
		//gl_FragColor = vec4(N.xyz, 1.0);
		//color = vec4(N.xyz, 1.0);
		vec2 distortedTexCoords = texture(dudvMap, vec2(fs_in.tc.x + moveFactor, fs_in.tc.y)).rg*0.1;
		distortedTexCoords = fs_in.tc + vec2(distortedTexCoords.x, distortedTexCoords.y+moveFactor);
		//vec2 totalDistortion = (texture(dudvMap, distortedTexCoords).rg * 2.0 - 1.0) * distortStrength;

		vec4 normCol = texture2D(normalMap, distortedTexCoords);
		vec3 normal = 0.7 * fs_in.norm + 0.3 * vec3(normCol.r * 2 - 1, normCol.b, normCol.g * 2 - 1);
		vec3 N = normalize(normal);

		color = vec4(N, 1.0);
	}
	else
	{
		//vec2 v2 = vec2((gl_FragCoord.x / screenResolution.x), (gl_FragCoord.y / screenResolution.y));
        //
		////vec3 N;
		////calc_normal(gl_FragCoord.st/128.0, N);
		////N = (N * 0.5) + 0.5;
		////vec3 N = fs_in.norm;//normalize(N);
		//vec3 N = texture2D(normalMap, fs_in.tc).xyz;
		////N = normalize(N + 0.001 * vec3(2 * normRBG.r - 1, normRBG.b * 3, 2 * normRBG.g - 1));
        ////vec3 specular = vec3(1.0) * pow(clamp(dot(reflect(normalize(lightv), N), viewv), 0.0, 1.0), 50.0);

		vec2 distortedTexCoords = texture(dudvMap, vec2(fs_in.tc.x + moveFactor, fs_in.tc.y)).rg*0.1;
		distortedTexCoords = fs_in.tc + vec2(distortedTexCoords.x, distortedTexCoords.y+moveFactor);
		vec2 totalDistortion = (texture(dudvMap, distortedTexCoords).rg * 2.0 - 1.0) * distortStrength;

		vec2 v2 = vec2((gl_FragCoord.x / screenResolution.x), (gl_FragCoord.y / screenResolution.y));
		vec2 refleTexCoord = v2 + totalDistortion;

		vec4 refleColor = texture2D(reflectionTexture, refleTexCoord);

		//vec3 oceanGreenColor = vec3(33 / 255.0, 135.0 / 255.0, 221.0 / 255.0);
		vec3 oceanGreenColor = vec3(0 / 255.0, 112.0 / 255.0, 92.0 / 255.0);
		vec3 oceanBlueColor = vec3(63.0 / 255.0, 167.0 / 255.0, 252.0 / 255.0);

		//N = normalize(N + vec3(0.0, N.y * 2, 0));
		vec4 normCol = texture2D(normalMap, distortedTexCoords);
		vec3 normal = 0.7 * fs_in.norm + 0.3 * vec3(normCol.r * 2 - 1, normCol.b, normCol.g * 2 - 1);
		vec3 N = normalize(normal);

        vec3 oceanblue = vec3(0.11, 0.44, 0.92);

        //const float R_0 = 0.4;
        //float fresnel = R_0 + (1.0 - R_0) * pow((1.0 - dot(-normalize(viewv), N)), 5.0);
        //fresnel = max(0.0, min(fresnel, 1.0));
		float fresnel = clamp(dot(fs_in.viewv, N), 0, 1);
		fresnel = pow(fresnel, 1.5);

		//color = vec4(mix(oceanBlueColor, oceanGreenColor, fresnel) /*+ specular*/, 1.0);
		//vec3 refl = reflect(-fs_in.viewv, N);
		//vec3 skyReflColor = texture(skyboxTexture, vec3(refl.x, refl.y, refl.z)).xyz;
		//vec3 skyReflColor = 0.3 * texture2D(reflectionTexture, v2).xyz;
		//color = vec4(mix(oceanBlueColor, oceanGreenColor, 1) /*+ specular*/, 1.0);

		// ********* light
		vec3 fromLightVector = lightv;//normalize(lightPos - fs_in.worldPosition);

		vec3 lightColour = vec3(1.0, 0.9, 0.3);

		vec3 reflectedLight = reflect(normalize(fromLightVector), normal);
		float specular = max(dot(reflectedLight, fs_in.viewv), 0);
		specular = pow(specular, shineDamper);
		vec3 specularHighlights = lightColour * specular * reflectivity/* * clamp(diffDepth / 0.5, 0, 1)*/;

        color = mix(refleColor, vec4(oceanGreenColor, 1.0), fresnel);//vec4(refleColor, 1.0f); //vec4(mix(color.rgb, skyReflColor, 1 - fresnel) /*+ specular*/, 1.0);
		color = mix(color, vec4(0.0, 0.3, 0.5, 1.0), 0.2f);
		color = color + vec4(specularHighlights, 0.0);
		//out_color = mix(refleColor, refraColor, fresnel);
		//out_color = mix(out_color, vec4(0.0, 0.3, 0.5, 1.0), 0.2f);

		//color = vec4(1.0, 0.4, 0.8, 1.0);

		//out_color = mix(refleColor, vec4(oceanGreenColor, 1.0), fresnel);

		// check whether fragment output is higher than threshold, if so output as brightness color
		float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
		//if(brightness > 1.0)
		//	BrightColor = vec4(color.rgb, 1.0);
		//else
		//	BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
		BrightColor = vec4(1.0, 1.0, 1.0, 1.0);
	}
}


//"uniform float time;"
//"uniform float waves_nm[300];"
            
//void main(void)
//{
//   vec3 N;
//   calc_normal(gl_FragCoord.st/128.0, N);
//   N = (N * 0.5) + 0.5;
//   gl_FragColor = vec4(N.xyz, 1.0);
//}