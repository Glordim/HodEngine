#version 450 core

layout(location=0) in vec2 pos;
layout(location=1) in vec2 textcoord;

layout(location=0) out vec4 outColor;

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
	outColor = vec4(255, 0, 0, 255);
	gl_Position = modelUbo.mvp * vec4(pos.xy, 0.0f, 1.0f);
}

