cbuffer Matrices
{
	float4x4 mvp;
} PushMatrices;

struct IN
{
	float2 position : POSITION;
	float4 color : COLOR;
};

struct OUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

void VertexMain()
{
	OUT.position = float4(IN.position, 0, 1) * PushMatrices.mvp;
	OUT.color = IN.color;
}
