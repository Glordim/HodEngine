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
	float4 channelMask;    // 1 or 0 for R, G, B, A
	float4 gridFirstColor; // line on the first pixel of each texel, alpha: how much it covers the texel, 0 for none
	float4 gridLastColor;  // line on the last pixel of each texel, same
	float2 textureSize;    // in texels, of the mip level shown
	float2 pixelsPerTexel; // of the mip level shown
	float  checkerTexels;  // size of a square of the checker, in texels: a power of two, 1/4 at most (4 x 4 squares per texel)
	float  mipLevel;       // the one shown, whatever the zoom
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
	float4 color = material.image.SampleLevel(material.imageSampler, input.uv, material.ubo.mipLevel);

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

	// In texels: aligned with the grid, it zooms with the image. A square is a power of two of texels, or of a texel divided
	// (down to 4 x 4 per texel), so that a translucent texel still shows the checker through it (2 x 2 texels checker texture, repeated)
	float3 checker = material.checker.Sample(material.checkerSampler, input.uv * material.ubo.textureSize / (2.0 * material.ubo.checkerTexels)).rgb;
	float3 result = lerp(checker, color.rgb, color.a);

	// Texel grid: a line on each edge between texels, on the first pixel of the texel and/or on the last pixel of the previous texel.
	// With a dark one and a light one, one of the two always stands out, whatever the color of the texel.
	// As the image starts on a whole pixel, each is 1 pixel wide at a whole zoom
	float2 pixelsPerTexel = material.ubo.pixelsPerTexel;
	float2 pixelsInTexel = frac(input.uv * material.ubo.textureSize) * pixelsPerTexel;
	if (min(pixelsInTexel.x, pixelsInTexel.y) < 1.0)
	{
		result = lerp(result, material.ubo.gridFirstColor.rgb, material.ubo.gridFirstColor.a);
	}
	else if (pixelsInTexel.x >= pixelsPerTexel.x - 1.0 || pixelsInTexel.y >= pixelsPerTexel.y - 1.0)
	{
		result = lerp(result, material.ubo.gridLastColor.rgb, material.ubo.gridLastColor.a);
	}

	return float4(result, 1.0);
}
