#version 450 core

layout(location=0) in vec3 pos;
layout(location=1) in vec3 color;
layout(location=2) in vec3 normal;
layout(location=3) in vec2 uv;
layout(location=4) in vec3 tangent;

uniform mat4 mvp;
uniform float time;

out vec2 out_uv;

void main()
{
	gl_Position = mvp * vec4(pos.xyz, 1.0f);
	out_uv = uv;
}

