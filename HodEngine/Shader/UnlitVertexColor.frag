#version 450 core

layout(location = 0) out vec4 frag_color;

layout(location = 0) in vec3 color_out;

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
	frag_color = vec4(color_out, 1.0f);
}

