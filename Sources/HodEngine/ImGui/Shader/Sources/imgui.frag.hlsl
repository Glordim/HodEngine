struct IN
{
	float2 texCoord : TEXCOORD;
	float4 color : COLOR;
};

struct OUT
{
	float4 color : SV_TARGET;
}

Texture2D<float4> texture : register(t0);
SamplerState textureSampler : register(s0);

void main()
{
	float4 textureColor = texture.Sample(textureSampler, IN.texCoord);
	OUT.color = textureColor * IN.color;
}
