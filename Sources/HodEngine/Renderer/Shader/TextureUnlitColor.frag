struct IN
{
	float2 uv : TEXTCOORD;
};

struct OUT
{
	float4 color : SV_TARGET;
};

Texture2D image : register(t1);
SamplerState imageSampler : register(s1);

struct UBO
{
	float4 color;
};

void FragMain()
{
	OUT.color = image.Sample(imageSampler, IN.uv) * UBO.color;
}
