#version 450 core

uniform float time;
uniform sampler2D textureSampler;

layout(location = 0) out vec4 frag_color;

in vec2 out_uv;

void main()
{
	frag_color = texture(textureSampler, out_uv.xy);
}

