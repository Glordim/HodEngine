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

uniform mat4 mvp;
uniform mat4 model;
uniform float time;
uniform int lightCount;
uniform PointLight pointLight[32];

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
	mat3 TBN;
} vs_out;

void main()
{
	vec3 T = normalize(vec3(model * vec4(tangent, 0.0f)));
	vec3 N = normalize(vec3(model * vec4(normal, 0.0f)));
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);
	
	gl_Position = mvp * vec4(pos, 1.0f);
	vs_out.FragPos = vec3(model * vec4(pos, 0.0f));
	vs_out.TexCoords = uv;
	vs_out.TBN = transpose(mat3(T, B, N));
}

