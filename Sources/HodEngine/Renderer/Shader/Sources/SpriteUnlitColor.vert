struct VertexInput
{
	float2 pos : POSITION;
	float2 uv : TEXCOORD;
}

struct VertexOutput
{
	float2 pos : POSITION;
	float2 uv : TEXCOORD;
}

struct ViewUniformBufferObject
{
	matrix4x4 view;
	matrix4x4 proj;
	matrix4x4 vp;
}

struct ModelUniformBufferObject
{
	matrix4x4 mvp;
	matrix4x4 model;
}

ViewUniformBufferObject viewUbo;
ModelUniformBufferObject modelUbo;

VertexOutput main(VertexInput input)
{
	VertexOutput output;
	output.position = input.position * modelUbo.mvp;
	output.uv = input.uv;
	return output;
}
