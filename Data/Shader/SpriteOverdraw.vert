#version 450 core

layout(location=0) in vec2 pos;
layout(location=1) in vec2 uv;

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
	gl_Position = modelUbo.model * vec4(pos.xy, 0.0f, 1.0f);
}

