struct PixelInput
{
	float2 texCoord : TEXCOORD;
	float4 color : COLOR;
};

Texture2D<float4> texture : register(t0);
SamplerState textureSampler : register(s0);

float4 main(PixelInput input) : SV_Target
{
	float4 textureColor = texture.Sample(textureSampler, input.texCoord);
	return textureColor * input.color;
}
