cbuffer Constant
{
	float2 scale;
	float2 translate;
};

struct IN
{
	float2 position : POSITION;
	float2 texCoord : TEXCOORD;
	float4 color : COLOR;
};

struct OUT
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
	float4 color : COLOR;
};

void main()
{
	OUT.position = float4(IN.position * scale + translate, 0, 1);
	OUT.texCoord = IN.texCoord;
	OUT.color = IN.color;
}
