#version 450 core

struct PointLight
{    
    vec4 pos;
	vec4 color;
	float intensity;
	float range;
};  

layout(binding = 0) uniform UniformBufferObject {
	float time;
	
	int lightCount;
	PointLight pointLight[32];

	vec4 ambiantColor;
	vec4 eyePos;
} ubo;

layout(binding = 1) uniform sampler2D textureSampler;


layout(location = 0) out vec4 frag_color;

layout(location = 0) in vec2 out_uv;
layout(location = 1) in vec3 out_normal;
layout(location = 2) in vec3 out_fragPos;

void main()
{
	vec3 diffuse = vec3(0.0f, 0.0f, 0.0f);
	
	for (int i = 0; i < ubo.lightCount; ++i)
	{
		vec3 norm = normalize(out_normal);
		vec3 lightDir = normalize(ubo.pointLight[i].pos.xyz - out_fragPos);
		
		float diff = max(dot(norm, lightDir), 0.0f);
		diffuse += diff * ubo.pointLight[i].color.xyz;
	}
	
	vec3 result = (ubo.ambiantColor.xyz + diffuse) * texture(textureSampler, out_uv.xy).xyz;
	
	frag_color = vec4(result, 1.0f);
}

