#version 430 core

layout (vertices = 4) out;

in VS_OUT
{
    vec2 tc;
	//float id;
} tcs_in[];

out TCS_OUT
{
    vec2 tc;
	//float id;
} tcs_out[];

uniform mat4 mvp;
uniform vec3 cameraPos;

void main(void)
{
    if (gl_InvocationID == 0)
    {
        vec4 p0 = mvp * gl_in[0].gl_Position;
        vec4 p1 = mvp * gl_in[1].gl_Position;
        vec4 p2 = mvp * gl_in[2].gl_Position;
        vec4 p3 = mvp * gl_in[3].gl_Position;
        p0 /= p0.w;
        p1 /= p1.w;
        p2 /= p2.w;
        p3 /= p3.w;
        if (p0.z <= 0.0 &&
            p1.z <= 0.0 &&
            p2.z <= 0.0 &&
            p3.z <= 0.0)
        {
             gl_TessLevelOuter[0] = 0.0;
             gl_TessLevelOuter[1] = 0.0;
             gl_TessLevelOuter[2] = 0.0;
             gl_TessLevelOuter[3] = 0.0;
        }
        else
        {
            //float l0 = length(p2.xy - p0.xy) * 4.0 + 1.0;
            //float l1 = length(p3.xy - p2.xy) * 4.0 + 1.0;
            //float l2 = length(p3.xy - p1.xy) * 4.0 + 1.0;
            //float l3 = length(p1.xy - p0.xy) * 4.0 + 1.0;
			//gl_TessLevelOuter[0] = l0;
            //gl_TessLevelOuter[1] = l1;
            //gl_TessLevelOuter[2] = l2;
            //gl_TessLevelOuter[3] = l3;
            //gl_TessLevelInner[0] = min(l1, l3);
            //gl_TessLevelInner[1] = min(l0, l2);

			
			//vec3 averPos = (gl_in[0].gl_Position.xyz + gl_in[1].gl_Position.xyz +
			//				gl_in[2].gl_Position.xyz + gl_in[3].gl_Position.xyz) / 4;
			//float L = length(cameraPos - averPos);
			//float clmDist = clamp(L, 0, 255);
			//float LOD = 32 - clmDist / 16;
			float LOD = 4;

            gl_TessLevelOuter[0] = LOD;
            gl_TessLevelOuter[1] = LOD;
            gl_TessLevelOuter[2] = LOD;
            gl_TessLevelOuter[3] = LOD;
            gl_TessLevelInner[0] = LOD;
            gl_TessLevelInner[1] = LOD;
			

			//float constvvv = 2.0;
			//gl_TessLevelInner[0] = constvvv;
			//gl_TessLevelInner[1] = constvvv;
			//gl_TessLevelOuter[0] = constvvv;
			//gl_TessLevelOuter[1] = constvvv;
			//gl_TessLevelOuter[2] = constvvv;
			//gl_TessLevelOuter[3] = constvvv;
        }
    }

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    tcs_out[gl_InvocationID].tc = tcs_in[gl_InvocationID].tc;
	//tcs_out[gl_InvocationID].id = tcs_in[gl_InvocationID].id;
}