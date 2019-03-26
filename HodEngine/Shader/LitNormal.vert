#version 450 core

struct PointLight
{    
    vec4 pos;
	vec4 color;
	float intensity;
	float range;
};  

layout(location=0) in vec3 pos;
layout(location=1) in vec2 uv;
layout(location=2) in vec3 normal;

uniform mat4 mvp;
uniform mat4 model;
uniform float time;
uniform int lightCount;
uniform PointLight pointLight[32];

out vec2 out_uv;
out vec3 out_normal;
out vec3 out_fragPos;

void main()
{
	gl_Position = mvp * vec4(pos.xyz, 1.0f);
	out_fragPos = vec3(model * vec4(pos.xyz, 1.0f));
	out_uv = uv;
	out_normal = normal;
}

