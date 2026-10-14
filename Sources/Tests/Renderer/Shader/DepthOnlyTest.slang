import Attributes;

// Draws only depth, like what a shadow map is drawn with: its fragment stage writes no color

struct PushMatrices
{
	float4x4 mvp;
};
[PushConstant]
[[vk::push_constant]]
ConstantBuffer<PushMatrices> pc;

struct VS_IN
{
	float2 position : POSITION;
};

struct VS_OUT
{
	float4 position : SV_Position;
};

VS_OUT VertexMain(VS_IN input)
{
	VS_OUT output;
	output.position = mul(pc.mvp, float4(input.position, 0.0, 1.0));
	return output;
}

void FragmentMain(VS_OUT input)
{
}
