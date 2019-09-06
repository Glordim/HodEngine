#version 450 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;
layout(location = 4) in vec3 tangent;

layout(location = 0) out vec2 out_uv;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec3 out_fragPos;

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

void main()
{
	out_fragPos = vec3(modelUbo.model * vec4(pos.xyz, 1.0f));
	out_uv = uv;
	out_normal = mat3(transpose(inverse(modelUbo.model))) * normal;

	gl_Position = modelUbo.mvp * vec4(pos.xyz, 1.0f);
}
