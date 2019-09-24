#version 450 core

layout(location = 0) in vec3 FragPos;

layout(location = 0) out vec4 frag_color;

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

layout(set = 1, binding = 0) uniform ModelUniformBufferObject {
	mat4 mvp;
	mat4 model;
} modelUbo;


layout(set = 2, binding = 0) uniform MatUniformBufferObject {
	float exposure;
} matUbo;

layout(set = 2, binding = 1) uniform sampler2D textureSampler;

#define M_PI 3.1415926535897932384626433832795

vec2 toLatLong(vec3 v) // incoming vertex after MVP mult. 
{
    float val;
    vec2 o = vec2(1.0);

    // radius, theta, phi
    float r = sqrt(pow(v.x, 2.0) + pow(v.y, 2.0) + pow(v.z, 2.0));
    float theta = acos(v.z / r);
    float phi   = atan(v.y / v.x);

    o.x = clamp(0.5 + phi / (2.0 * M_PI), 0.0, 1.0);
    o.y = clamp(theta / M_PI, 0.0, 1.0);
    //o.z = (r * cos(theta)) / v.z;
    return o; // final vertex pos. gl_position.
}

void main()
{
	vec3 dir = normalize(FragPos);
	vec2 longlat = vec2(atan(dir.x, dir.z) + M_PI, acos(-dir.y));
	vec2 uv = longlat / vec2(2.0 * M_PI, M_PI);
	
	frag_color = vec4(texture(textureSampler, uv).rgb, 1.0f);
}

