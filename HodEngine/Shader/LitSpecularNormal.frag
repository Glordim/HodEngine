#version 450 core

struct PointLight
{    
    vec4 pos;
	vec4 color;
	float intensity;
	float range;
};  

uniform float time;
uniform sampler2D textureSampler;
uniform sampler2D specularTextureSampler;
uniform sampler2D normalTextureSampler;

uniform int lightCount;
uniform PointLight pointLight[32];

uniform vec4 ambiantColor;
uniform vec4 eyePos;

uniform float shininess;
uniform float specularStrength;

layout(location = 0) out vec4 frag_color;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
	mat3 TBN;
} fs_in;

void main()
{
	vec3 viewDir = normalize(eyePos.xyz - fs_in.FragPos);
	
	vec3 norm = normalize(texture(normalTextureSampler, fs_in.TexCoords.st).rgb);
	norm = normalize(norm * 2.0f - 1.0f);
	norm = normalize(fs_in.TBN * norm);
	
	vec3 diffuse = vec3(0.0f, 0.0f, 0.0f);
	vec3 specular = vec3(0.0f, 0.0f, 0.0f);
	
	for (int i = 0; i < lightCount; ++i)
	{		
		vec3 lightDir = normalize(pointLight[i].pos.xyz - fs_in.FragPos);
		
		float diff = max(dot(norm, lightDir), 0.0f);
		diffuse += diff * pointLight[i].color.xyz;
		
		vec3 reflectDir = normalize(reflect(-lightDir, norm));
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
		specular += spec * pointLight[i].color.xyz * (texture(specularTextureSampler, fs_in.TexCoords.xy).x * specularStrength);
	}
	
	vec3 result = (ambiantColor.xyz + diffuse + specular) * texture(textureSampler, fs_in.TexCoords.xy).xyz;
	
	frag_color = vec4(result, 1.0f);
}

