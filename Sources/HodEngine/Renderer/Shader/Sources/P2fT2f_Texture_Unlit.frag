struct IN
{
	float2 uv : TEXTCOORD;	
};

struct OUT
{
	float4 color : SV_TARGET;
};

Texture2D image : register(t0);
SamplerState imageSampler : register(s0);

void FragMain()
{
	OUT.color = image.Sample(imageSampler, IN.uv);
}
