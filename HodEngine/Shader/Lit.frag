#version 450 core

layout(location = 0) in vec2 out_uv;
layout(location = 1) in vec3 out_normal;
layout(location = 2) in vec3 out_fragPos;

layout(location = 0) out vec4 frag_color;

layout(set = 0, binding = 0) uniform ViewUniformBufferObject {
	mat4 view;
	mat4 proj;
	mat4 vp;
} viewUbo;

struct PointLight
{    
	vec4 color;
    vec3 pos;
	float intensity;
	float range;
};  

layout(set = 0, binding = 1) uniform LightUniformBufferObject {
	int lightCount;
	PointLight pointLight[32];

	vec4 ambiantColor;
	vec4 eyePos;
} lightUbo;

layout(set = 1, binding = 0) uniform ModelUniformBufferObject {
	mat4 mvp;
	mat4 model;
} modelUbo;

layout(set = 2, binding = 1) uniform sampler2D textureSampler;

void main()
{
	vec3 diffuse = vec3(0.0f, 0.0f, 0.0f);
	vec4 ambiant = lightUbo.ambiantColor;
	
	for (int i = 0; i < lightUbo.lightCount; ++i)
	{
		vec3 norm = normalize(out_normal);
		vec3 lightDir = normalize(lightUbo.pointLight[i].pos.xyz - out_fragPos);
		
		float diff = max(dot(norm, lightDir), 0.0f);
		diffuse += diff * lightUbo.pointLight[i].color.xyz;
		
		//ambiant = vec4(lightUbo.pointLight[i].pos, 1.0f);
	}
	
	vec3 result = (ambiant.xyz + diffuse) * texture(textureSampler, out_uv.xy).xyz;
	
	frag_color = vec4(result, 1.0f);
}
