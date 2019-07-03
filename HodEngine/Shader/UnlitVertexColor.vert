#version 450 core

layout(location=0) in vec3 pos;
layout(location=1) in vec3 color;
layout(location=2) in vec3 normal;
layout(location=3) in vec2 uv;
layout(location=4) in vec3 tangent;

layout(binding = 0) uniform UniformBufferObject {
	mat4 model;
	mat4 view;
	mat4 proj;
	mat4 mvp;
} ubo;

layout(location = 0) out vec3 color_out;

void main()
{
	gl_Position = ubo.mvp * vec4(pos.xyz, 1.0f);
	color_out = color;
}

