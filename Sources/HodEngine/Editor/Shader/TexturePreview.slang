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
	float2 textureSize;  // in texels
	float  pixelsPerTexel;
	float  gridOpacity; // of the lines between the texels, 0 for none
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
	float3 result = lerp(checker, color.rgb, color.a);

	// Texel grid: a two-tone line on each edge between texels, a dark pixel on the first pixel of the texel and a light one on the last pixel of the previous texel.
	// One of the two always stands out, whatever the color of the texel (one color can't: it blends into the texels of its own luminance).
	// As the image starts on a whole pixel, each is 1 pixel wide at a whole zoom
	float  pixelsPerTexel = material.ubo.pixelsPerTexel;
	float2 pixelsInTexel = frac(input.uv * material.ubo.textureSize) * pixelsPerTexel;
	if (material.ubo.gridOpacity > 0.0)
	{
		float gridStrength = 0.35 * material.ubo.gridOpacity;
		if (min(pixelsInTexel.x, pixelsInTexel.y) < 1.0)
		{
			result = lerp(result, float3(0.0, 0.0, 0.0), gridStrength);
		}
		else if (max(pixelsInTexel.x, pixelsInTexel.y) >= pixelsPerTexel - 1.0)
		{
			result = lerp(result, float3(1.0, 1.0, 1.0), gridStrength);
		}
	}

	return float4(result, 1.0);
}
