#version 330

layout(location=0) in vec3 pos;

uniform mat4 mvp;

void main()
{
	gl_Position = mvp * vec4(pos.xyz, 1.0f);
}

