#version 450 core

layout(location = 0) out vec4 frag_color;

//layout(location = 0) in vec3 color_out;

layout(set = 0, binding = 0) uniform ViewUniformBufferObject {
	mat4 view;
	mat4 proj;
	mat4 vp;
} viewUbo;
/*
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
*/

layout(set = 1, binding = 0) uniform ModelUniformBufferObject {
	mat4 mvp;
	mat4 model;
} modelUbo;

layout(set = 2, binding = 0) uniform MatUniformBufferObject {
	vec4 color;
} matUbo;

void main()
{
	frag_color = matUbo.color;
}

