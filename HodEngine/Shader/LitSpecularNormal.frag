#version 450 core

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec2 TexCoords;
layout(location = 2) in mat3 TBN;

layout(location = 0) out vec4 frag_color;

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

layout(set = 1, binding = 0) uniform ModelUniformBufferObject {
	mat4 mvp;
	mat4 model;
} modelUbo;


layout(set = 2, binding = 0) uniform MatUniformBufferObject {
	float shininess;
	float specularStrength;
	vec4 tilingOffset;
} matUbo;

layout(set = 2, binding = 1) uniform sampler2D textureSampler;
layout(set = 2, binding = 2) uniform sampler2D specularTextureSampler;
layout(set = 2, binding = 3) uniform sampler2D normalTextureSampler;

void main()
{
	vec3 viewDir = TBN * normalize(lightUbo.eyePos - FragPos);
	
	vec2 uv = (TexCoords * matUbo.tilingOffset.xy) + matUbo.tilingOffset.zw;
	
	vec3 norm = texture(normalTextureSampler, uv.xy).rgb;
	norm = normalize(norm * 2.0f - 1.0f);
	
	vec3 diffuse = vec3(0.0f, 0.0f, 0.0f);
	vec3 specular = vec3(0.0f, 0.0f, 0.0f);
	
	for (int i = 0; i < lightUbo.dirLightCount; ++i)
	{
		vec3 lightDir = TBN * normalize(-lightUbo.dirLight[i].dir.xyz);
		
		float diff = max(dot(norm, lightDir), 0.0f);
		diffuse += (diff * lightUbo.dirLight[i].color.xyz) * lightUbo.dirLight[i].intensity;
		
		vec3 reflectDir = normalize(reflect(-lightDir, norm));
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), matUbo.shininess);
		specular += (spec * lightUbo.dirLight[i].color.xyz * (texture(specularTextureSampler, uv.xy).x * matUbo.specularStrength)) * lightUbo.dirLight[i].intensity;
	}
	
	for (int i = 0; i < lightUbo.pointLightCount; ++i)
	{
		float distance = length(lightUbo.pointLight[i].pos.xyz - FragPos);
		float attenuation = 1.0f / (lightUbo.pointLight[i].constant + lightUbo.pointLight[i].linear * distance + lightUbo.pointLight[i].quadratic * (distance * distance));
		
		vec3 lightDir = TBN * normalize(lightUbo.pointLight[i].pos.xyz - FragPos);
		
		float diff = max(dot(norm, lightDir), 0.0f);
		diffuse += (diff * lightUbo.pointLight[i].color.xyz) * attenuation;
		
		vec3 reflectDir = normalize(reflect(-lightDir, norm));
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), matUbo.shininess);
		specular += (spec * lightUbo.pointLight[i].color.xyz * (texture(specularTextureSampler, uv.xy).x * matUbo.specularStrength)) * attenuation;
	}
	
	for (int i = 0; i < lightUbo.spotLightCount; ++i)
	{
		vec3 lightDir = normalize(lightUbo.spotLight[i].pos - FragPos);
		
		float angle = dot(lightDir, normalize(-lightUbo.spotLight[i].dir));
    
		if (angle > lightUbo.spotLight[i].radius)
		{
			float epsilon = lightUbo.spotLight[i].radius - lightUbo.spotLight[i].outer;
			float intensity = 1.0f - clamp((angle - lightUbo.spotLight[i].outer) / epsilon, 0.0, 1.0);
			
			lightDir = TBN * normalize(lightUbo.spotLight[i].pos - FragPos);
			
			float distance = length(lightUbo.spotLight[i].pos.xyz - FragPos);
			float attenuation = 1.0f / (lightUbo.spotLight[i].constant + lightUbo.spotLight[i].linear * distance + lightUbo.spotLight[i].quadratic * (distance * distance));

			float diff = max(dot(norm, lightDir), 0.0f);
			diffuse += (diff * lightUbo.spotLight[i].color.xyz) * attenuation * intensity;
			
			vec3 reflectDir = normalize(reflect(-lightDir, norm));
			float spec = pow(max(dot(viewDir, reflectDir), 0.0), matUbo.shininess);
			specular += (spec * lightUbo.spotLight[i].color.xyz * (texture(specularTextureSampler, uv.xy).x * matUbo.specularStrength)) * attenuation * intensity;
		}		
	}
	
	vec3 result = (lightUbo.ambiantColor.xyz + diffuse + specular) * texture(textureSampler, uv.xy).xyz;
	
	frag_color = vec4(result, 1.0f);
}

