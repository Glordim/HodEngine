#version 450 core

layout(location = 0) out vec4 frag_color;

layout(location = 0) in vec3 color_out;

void main()
{
	frag_color = vec4(color_out, 1.0f);
}

