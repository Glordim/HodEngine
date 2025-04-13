struct IN
{
	float2 texCoord : TEXCOORD;
	float4 color : COLOR;
};

struct OUT
{
	float4 color : SV_TARGET;
};

Texture2D image : register(t0);
SamplerState imageSampler : register(s0);

void FragMain()
{
	float4 textureColor = image.Sample(imageSampler, IN.texCoord);
	OUT.color = textureColor * IN.color;
}
