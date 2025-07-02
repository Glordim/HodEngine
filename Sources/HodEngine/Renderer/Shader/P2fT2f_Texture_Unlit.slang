struct PushMatrices
{
	float4x4 mvp;
};
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

[[binding(0, 0)]]
struct UBO
{
	float4 color;
};
ConstantBuffer<UBO> ubo;

[[binding(1, 0)]]
Texture2D image;

[[binding(2, 0)]]
SamplerState imageSampler;

float4 FragmentMain(VS_OUT input) : SV_Target
{
	return image.Sample(imageSampler, input.uv) * ubo.color;
}
