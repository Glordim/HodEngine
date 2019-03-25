#version 330

layout(location=0) in vec3 pos;
layout(location=1) in vec2 uv;

uniform mat4 mvp;
uniform float time;

out vec2 out_uv;

void main()
{
	gl_Position = mvp * vec4(pos.xyz, 1.0f);
	out_uv = uv;
}

