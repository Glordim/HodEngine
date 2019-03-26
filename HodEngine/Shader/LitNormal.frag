#version 450 core

struct PointLight
{    
    vec4 pos;
	vec4 color;
	float intensity;
	float range;
};  

uniform float time;
uniform sampler2D textureSampler_Albedo;
uniform sampler2D textureSampler_Normal;

uniform int lightCount;
uniform PointLight pointLight[32];

layout(location = 0) out vec4 frag_color;

in vec2 out_uv;
in vec3 out_normal;
in vec3 out_fragPos;

void main()
{
	vec3 normal = texture(textureSampler_Normal, out_uv.xy).rgb;
	normal = -normalize(normal * 2.0f - 1.0f);
	//normal = normalize(out_normal);
	
	vec4 diffuse = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	
	for (int i = 0; i < lightCount; ++i)
	{
		vec3 norm = normal;
		vec3 lightDir = normalize(pointLight[i].pos.xyz - out_fragPos);
	
		float dist = distance(out_fragPos.xyz, pointLight[i].pos.xyz);
	
		float diff = max(dot(norm, lightDir), 0.0f);
		diffuse += vec4(diff * pointLight[i].color.xyz, 1.0f) * 4.0f / dist;
	}
	
	frag_color = texture(textureSampler_Albedo, out_uv.xy) * diffuse;
	//frag_color = vec4(out_normal, 1.0f);
}

