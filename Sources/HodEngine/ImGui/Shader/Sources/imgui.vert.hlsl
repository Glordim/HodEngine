cbuffer Constant
{
	float2 scale;
	float2 translate;
};

struct VertexInput
{
	float2 position : POSITION;
	float2 texCoord : TEXCOORD;
	float4 color : COLOR;
};

struct VertexOutput
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
	float4 color : COLOR;
};

VertexOutput main(VertexInput input)
{
	VertexOutput output;
	output.position = float4(input.position * scale + translate, 0, 1);
	output.texCoord = input.texCoord;
	output.color = input.color;
	return output;
}
