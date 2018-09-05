#version 430 core

layout (binding = 1) uniform sampler2D reflectionTexture;
layout (binding = 2) uniform sampler2D refractionTexture;
layout (binding = 3) uniform sampler2D dudvMap;
layout (binding = 4) uniform sampler2D normalMap;
layout (binding = 5) uniform sampler2D depthMap;

in TES_OUT
{
	vec3 worldPosition;
    vec2 texCoords;
} fs_in;

layout (location = 0) out vec4 out_color;

//uniform int debug_mode_enabled = 0;

uniform float distortStrength = 0.02f;
uniform float moveFactor;

uniform vec3 cameraPos;
uniform vec3 lightPos;

uniform float shineDamper = 20.0;
uniform float reflectivity = 0.2;

uniform vec2 screenResolution;

float LinearizeDepth(float zoverw){
		float n = 0.1; // camera z near
		float f = 1000.0; // camera z far
		return 2.0 * n * f / (f + n - (2.0 * zoverw - 1.0) * (f - n));
	}

//return (2.0 * n) / (f + n - zoverw * (f - n));

void main(void)
{
	vec2 distortedTexCoords = texture(dudvMap, vec2(fs_in.texCoords.x + moveFactor, fs_in.texCoords.y)).rg*0.1;
	distortedTexCoords = fs_in.texCoords + vec2(distortedTexCoords.x, distortedTexCoords.y+moveFactor);
	vec2 totalDistortion = (texture(dudvMap, distortedTexCoords).rg * 2.0 - 1.0) * distortStrength;

	vec2 v2 = vec2((gl_FragCoord.x / screenResolution.x), (gl_FragCoord.y / screenResolution.y));
	vec2 refleTexCoord = v2 + totalDistortion;
	vec2 refraTexCoord = v2 + totalDistortion;

	refleTexCoord = clamp(refleTexCoord, 0.001, 0.999);
	refraTexCoord = clamp(refraTexCoord, 0.001, 0.999);

	vec4 refleColor = texture2D(reflectionTexture, refleTexCoord);
	vec4 refraColor = texture2D(refractionTexture, refraTexCoord);

	vec4 normCol = texture2D(normalMap, distortedTexCoords);
	vec3 normal = vec3(normCol.r * 2 - 1, normCol.b * 3, normCol.g * 2 - 1);
	normal = normalize(normal);

	vec3 fromLightVector = normalize(lightPos - fs_in.worldPosition);
	vec3 viewVector = normalize(cameraPos - fs_in.worldPosition);

	float fresnel = clamp(dot(viewVector, normal), 0, 1);
	fresnel = pow(fresnel, 1.5);

	
	float floorDepth = LinearizeDepth(texture2D(depthMap, refraTexCoord).r);
	float waterDepth = LinearizeDepth(gl_FragCoord.z);
	float diffDepth = (floorDepth - waterDepth);

	vec3 lightColour = vec3(1.0, 0.9, 1.0);
	
	vec3 reflectedLight = reflect(normalize(fromLightVector), normal);
	float specular = max(dot(reflectedLight, viewVector), 0);
	specular = pow(specular, shineDamper);
	vec3 specularHighlights = lightColour * specular * reflectivity * clamp(diffDepth / 0.5, 0, 1);
	
	out_color = mix(refleColor, refraColor, fresnel);
	out_color = mix(out_color, vec4(0.0, 0.3, 0.5, 1.0), 0.2f);
	out_color = out_color + vec4(specularHighlights, 0.0);
	out_color.a = clamp(diffDepth / 0.5, 0, 1);

	//out_color = vec4(diffDepth / 5.0, diffDepth / 5.0, diffDepth / 5.0, 1.0);
	//out_color = vec4(0.0, 0.3, 0.5, 1.0);
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