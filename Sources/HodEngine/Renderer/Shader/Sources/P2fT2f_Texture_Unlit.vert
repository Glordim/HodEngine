cbuffer Matrices
{
	float4x4 mvp;
} PushMatrices;

struct IN
{
	float2 position : POSITION;
	float2 uv : TEXTCOORD;
};

struct OUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXTCOORD;
};

void main()
{
	OUT.position = float4(IN.position, 0, 1) * PushMatrices.mvp;
	OUT.uv = IN.uv;
}
