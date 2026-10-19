import Attributes;

// The texture of the texture editor viewport, over a checker, with only some of its channels

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

struct UBO
{
	float4 channelMask;  // 1 or 0 for R, G, B, A
	float2 checkerScale; // checker tiles in the texture, on each axis
};

struct Material
{
	ConstantBuffer<UBO> ubo;
	Texture2D           image;
	SamplerState        imageSampler;
	Texture2D           checker;
	SamplerState        checkerSampler;
};
ParameterBlock<Material> material;

float4 FragmentMain(VS_OUT input) : SV_Target
{
	float4 color = material.image.Sample(material.imageSampler, input.uv);

	float4 channelMask = material.ubo.channelMask;
	if (dot(channelMask, float4(1.0, 1.0, 1.0, 1.0)) == 1.0)
	{
		// A single channel: in grayscale
		float value = dot(color, channelMask);
		color = float4(value, value, value, 1.0);
	}
	else
	{
		// Hidden color channels are black, a hidden alpha is opaque
		color = float4(color.rgb * channelMask.rgb, lerp(1.0, color.a, channelMask.a));
	}

	float3 checker = material.checker.Sample(material.checkerSampler, input.uv * material.ubo.checkerScale).rgb;
	return float4(lerp(checker, color.rgb, color.a), 1.0);
}
