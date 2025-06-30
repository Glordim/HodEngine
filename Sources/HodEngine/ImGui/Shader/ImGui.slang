struct Constant
{
	float2 scale;
	float2 translate;
};

struct VS_IN
{
	float2 position : POSITION;
	float2 texCoord : TEXCOORD0;
	float4 color : COLOR0;
};

struct VS_OUT
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
	float4 color : COLOR0;
};

VS_OUT VertexMain(VS_IN input, uniform Constant constant)
{
	VS_OUT output;
	output.position = float4(input.position * constant.scale + constant.translate, 0, 1);
	output.texCoord = input.texCoord;
	output.color = input.color;
	return output;
}

Texture2D image;
SamplerState imageSampler;

float4 FragmentMain(VS_OUT input) : SV_TARGET
{
	float4 textureColor = image.Sample(imageSampler, input.texCoord);
	return textureColor * input.color;
}
