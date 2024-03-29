#version 450 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;
layout(location = 4) in vec3 tangent;
layout(location = 5) in vec3 bitangent;

layout(location = 0) out vec3 FragPos;
layout(location = 1) out vec2 TexCoords;
layout(location = 2) out mat3 TBN;
layout(location = 5) out vec3 normals;

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

void main()
{
	normals = mat3(transpose(inverse(modelUbo.model))) * normal;

	vec3 T = normalize(vec3(modelUbo.model * vec4(tangent, 0.0f)));
	vec3 B = normalize(vec3(modelUbo.model * vec4(bitangent, 0.0f)));
	vec3 N = normalize(vec3(modelUbo.model * vec4(normal, 0.0f)));

	TBN = transpose(mat3(T, B, N));
	
	FragPos = vec3(modelUbo.model * vec4(pos, 1.0f));
	TexCoords = uv;
	
	gl_Position = modelUbo.mvp * vec4(pos, 1.0f);
}

