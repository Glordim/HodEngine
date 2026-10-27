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

struct UBO
{
	float4 color;
};

struct Material
{
	ConstantBuffer<UBO> ubo;
	Texture2D           image;
	SamplerState        imageSampler;
};
ParameterBlock<Material> material;

// GlyphCache::SdfSpread: the texels the distance goes to on each side of the outline, from 0 (outside) to 1 (inside)
static const float SdfSpread = 6.0;
// 128 is the outline
static const float SdfEdge = 128.0 / 255.0;

float4 FragmentMain(VS_OUT input) : SV_Target
{
	// The distance to the outline, in the red channel of an R8 atlas page
	float distance = material.image.Sample(material.imageSampler, input.uv).r;

	// How many pixels of the screen the whole range of the distance spans: the edge is smoothed over one pixel, whatever the size the glyph is drawn at
	uint width;
	uint height;
	material.image.GetDimensions(width, height);
	float2 unitRange = (2.0 * SdfSpread) / float2(width, height);
	float2 screenTexSize = 1.0 / fwidth(input.uv);
	float  screenPxRange = max(0.5 * dot(unitRange, screenTexSize), 1.0);

	float coverage = saturate(screenPxRange * (distance - SdfEdge) + 0.5);
	return float4(material.ubo.color.rgb, material.ubo.color.a * coverage);
}
