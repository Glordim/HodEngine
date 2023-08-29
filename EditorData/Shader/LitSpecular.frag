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

struct DirLight
{
	vec3 dir;
	vec4 color;
	float intensity;
};

struct PointLight
{    
    vec3 pos;
	vec4 color;
	float intensity;
	float constant;
    float linear;
    float quadratic;
};

struct SpotLight
{
	vec3 pos;
	vec3 dir;
	vec4 color;
	float intensity;
	float constant;
    float linear;
    float quadratic;
	float radius;
	float outer;
	float inner;
}; 

layout(set = 0, binding = 1) uniform LightUniformBufferObject {
	int dirLightCount;
	DirLight dirLight[4];

	int pointLightCount;
	PointLight pointLight[32];
	
	int spotLightCount;
	SpotLight spotLight[32];

	vec4 ambiantColor;
	vec3 eyePos;
} lightUbo;

layout(set = 0, binding = 2) uniform sampler2D skyboxSampler;

layout(set = 1, binding = 0) uniform ModelUniformBufferObject {
	mat4 mvp;
	mat4 model;
} modelUbo;


layout(set = 2, binding = 0) uniform MatUniformBufferObject {
	float shininess;
	float specularStrength;
} matUbo;

layout(set = 2, binding = 1) uniform sampler2D textureSampler;
layout(set = 2, binding = 2) uniform sampler2D specularTextureSampler;

void main()
{
	vec3 viewDir = normalize(lightUbo.eyePos - out_fragPos);
	
	vec3 diffuse = vec3(0.0f, 0.0f, 0.0f);
	vec3 specular = vec3(0.0f, 0.0f, 0.0f);
	
	for (int i = 0; i < lightUbo.pointLightCount; ++i)
	{
		float distance = length(lightUbo.pointLight[i].pos.xyz - out_fragPos);
		float attenuation = 1.0 / (lightUbo.pointLight[i].constant + lightUbo.pointLight[i].linear * distance + lightUbo.pointLight[i].quadratic * (distance * distance));
		
		vec3 norm = normalize(out_normal);
		vec3 lightDir = normalize(lightUbo.pointLight[i].pos.xyz - out_fragPos);
		
		float diff = max(dot(norm, lightDir), 0.0f);
		diffuse += (diff * lightUbo.pointLight[i].color.xyz) * attenuation;
		
		vec3 reflectDir = normalize(reflect(-lightDir, norm));
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), matUbo.shininess);
		specular += (spec * lightUbo.pointLight[i].color.xyz * (texture(specularTextureSampler, out_uv.xy).x * matUbo.specularStrength)) * attenuation;
	}
	
	vec3 result = (lightUbo.ambiantColor.xyz + diffuse + specular) * texture(textureSampler, out_uv.xy).xyz;
	
	frag_color = vec4(result, 1.0f);
}
