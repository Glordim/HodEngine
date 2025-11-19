struct PushMatrices
{
	float4x4 mvp;
};
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
ConstantBuffer<UBO> ubo;

float4 FragmentMain(VS_OUT input) : SV_Target
{
	return ubo.color;
}
