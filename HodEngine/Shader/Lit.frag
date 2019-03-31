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

uniform int lightCount;
uniform PointLight pointLight[32];

uniform vec4 ambiantColor;
uniform vec4 eyePos;

uniform float shininess;
uniform float specularStrength;

layout(location = 0) out vec4 frag_color;

in vec2 out_uv;
in vec3 out_normal;
in vec3 out_fragPos;

void main()
{
	vec3 viewDir = normalize(eyePos.xyz - out_fragPos);
	
	vec3 diffuse = vec3(0.0f, 0.0f, 0.0f);
	vec3 specular = vec3(0.0f, 0.0f, 0.0f);
	
	for (int i = 0; i < lightCount; ++i)
	{
		vec3 norm = normalize(out_normal);
		vec3 lightDir = normalize(pointLight[i].pos.xyz - out_fragPos);
		
		float diff = max(dot(norm, lightDir), 0.0f);
		diffuse += diff * pointLight[i].color.xyz;
		
		vec3 reflectDir = reflect(-lightDir, norm);		
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
		specular += specularStrength * spec * pointLight[i].color.xyz;
	}
	
	vec3 result = (ambiantColor.xyz + diffuse + specular) * texture(textureSampler, out_uv.xy).xyz;
	
	frag_color = vec4(result, 1.0f);
}

