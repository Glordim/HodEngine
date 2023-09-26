struct IN
{
	float2 pos : POSITION;
	float2 uv : TEXCOORD;
}

struct OUT
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

void main()
{
	OUT.position = IN.position * modelUbo.mvp;
	OUT.uv = IN.uv;
}
