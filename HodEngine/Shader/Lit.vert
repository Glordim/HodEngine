#version 450 core

struct PointLight
{    
    vec4 pos;
	vec4 color;
	float intensity;
	float range;
};

layout(location=0) in vec3 pos;
layout(location=1) in vec3 color;
layout(location=2) in vec3 normal;
layout(location=3) in vec2 uv;
layout(location=4) in vec3 tangent;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
	
	uniform float time;
	uniform int lightCount;
	uniform PointLight pointLight[32];
} ubo;

layout(location = 0) out vec2 out_uv;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec3 out_fragPos;

void main()
{
	mat4 mvp = ubo.proj * ubo.view * ubo.model;

	gl_Position = mvp * vec4(pos.xyz, 1.0f);
	out_fragPos = vec3(ubo.model * vec4(pos.xyz, 1.0f));
	out_uv = uv;
	out_normal = mat3(transpose(inverse(ubo.model))) * normal;
}

