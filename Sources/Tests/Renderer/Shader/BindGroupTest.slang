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
	float2 uv : TEXTCOORD;
};

struct VS_OUT
{
	float4 position : SV_Position;
	float2 uv : TEXTCOORD;
};

VS_OUT VertexMain(VS_IN input)
{
	VS_OUT output;
	output.position = mul(pc.mvp, float4(input.position, 0.0, 1.0));
	output.uv = input.uv;
	return output;
}

struct Globals
{
	float4 tint;
};
// A set is given twice: to Vulkan by [[vk::binding(binding, set)]], to D3D12 by the space of the register.
// The b registers are numbered across the sets, after the one the push constant takes, because Metal has no space and reads them as buffer indices.
[[vk::binding(0, 0)]]
ConstantBuffer<Globals> global : register(b1, space0);

struct UBO
{
	float4 color;
};
[[vk::binding(0, 1)]]
ConstantBuffer<UBO> ubo : register(b2, space1);

[[vk::binding(1, 1)]]
Texture2D image : register(t0, space1);
[[vk::binding(2, 1)]]
SamplerState imageSampler : register(s0, space1);

float4 FragmentMain(VS_OUT input) : SV_Target
{
	return image.Sample(imageSampler, input.uv) * ubo.color * global.tint;
}
