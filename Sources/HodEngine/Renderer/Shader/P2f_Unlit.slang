import Attributes;

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

struct UBO
{
	float4 color;
};

struct Material
{
	ConstantBuffer<UBO> ubo;
};
ParameterBlock<Material> material;

float4 FragmentMain(VS_OUT input) : SV_Target
{
	return material.ubo.color;
}
