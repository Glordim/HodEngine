#version 450 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;
layout(location = 4) in vec3 tangent;
layout(location = 5) in vec3 bitangent;

layout(location = 0) out vec3 FragPos;

layout(set = 0, binding = 0) uniform ViewUniformBufferObject {
	mat4 view;
	mat4 proj;
	mat4 vp;
} viewUbo;

struct PointLight
{    
    vec3 pos;
	vec4 color;
	float intensity;
	
	float constant;
    float linear;
    float quadratic;
};

layout(set = 0, binding = 1) uniform LightUniformBufferObject {
	int lightCount;
	PointLight pointLight[32];

	vec4 ambiantColor;
	vec3 eyePos;
} lightUbo;

layout(set = 1, binding = 0) uniform ModelUniformBufferObject {
	mat4 mvp;
	mat4 model;
} modelUbo;

void main()
{
	FragPos = pos;
	
	gl_Position = viewUbo.proj * viewUbo.view * vec4(pos, 1.0f);
}

