#version 450 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;
layout(location = 4) in vec3 tangent;

layout(location = 0) out vec3 FragPos;
layout(location = 1) out vec2 TexCoords;
layout(location = 2) out mat3 TBN;

layout(set = 0, binding = 0) uniform ViewUniformBufferObject {
	mat4 view;
	mat4 proj;
	mat4 vp;
} viewUbo;

layout(set = 1, binding = 0) uniform ModelUniformBufferObject {
	mat4 mvp;
	mat4 model;
} modelUbo;

void main()
{
	vec3 T = normalize(vec3(modelUbo.model * vec4(tangent, 0.0f)));
	vec3 N = normalize(vec3(modelUbo.model * vec4(normal, 0.0f)));
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);
	
	FragPos = vec3(modelUbo.model * vec4(pos, 0.0f));
	TexCoords = uv;
	TBN = mat3(T, B, N);
	
	gl_Position = modelUbo.mvp * vec4(pos, 1.0f);
}
