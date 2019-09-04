#version 450 core

layout(location = 0) out vec4 frag_color;

layout(location = 0) in vec2 out_uv;
layout(location = 1) in vec3 out_normal;
layout(location = 2) in vec3 out_fragPos;

layout(set = 0, binding = 0) uniform ViewUniformBufferObject {
	mat4 view;
	mat4 proj;
	mat4 vp;
} viewUbo;

layout(set = 1, binding = 0) uniform ModelUniformBufferObject {
	mat4 mvp;
	mat4 model;
} modelUbo;

struct PointLight
{    
    vec4 pos;
	vec4 color;
	float intensity;
	float range;
};

layout(set = 2, binding = 0) uniform MatUniformBufferObject {
	int lightCount;
	PointLight pointLight[32];

	vec4 ambiantColor;
	vec4 eyePos;

	float shininess;
	float specularStrength;
} matUbo;

layout(set = 2, binding = 1) uniform sampler2D textureSampler;
layout(set = 2, binding = 2) uniform sampler2D specularTextureSampler;

void main()
{
	vec3 viewDir = normalize(matUbo.eyePos.xyz - out_fragPos);
	
	vec3 diffuse = vec3(0.0f, 0.0f, 0.0f);
	vec3 specular = vec3(0.0f, 0.0f, 0.0f);
	
	for (int i = 0; i < matUbo.lightCount; ++i)
	{
		vec3 norm = normalize(out_normal);
		vec3 lightDir = normalize(matUbo.pointLight[i].pos.xyz - out_fragPos);
		
		float diff = max(dot(norm, lightDir), 0.0f);
		diffuse += diff * matUbo.pointLight[i].color.xyz;
		
		vec3 reflectDir = reflect(-lightDir, norm);		
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), matUbo.shininess);
		specular += spec * matUbo.pointLight[i].color.xyz * (texture(specularTextureSampler, out_uv.xy).x * matUbo.specularStrength);
	}
	
	vec3 result = (matUbo.ambiantColor.xyz + diffuse + specular) * texture(textureSampler, out_uv.xy).xyz;
	
	frag_color = vec4(result, 1.0f);
}
