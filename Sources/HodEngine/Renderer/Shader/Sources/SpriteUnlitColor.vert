cbuffer Matrices
{
	float4x4 mvp;
} PushMatrices;

struct IN
{
	float2 position : POSITION;
};

struct OUT
{
	float4 position : SV_POSITION;
};

void main()
{
	OUT.position = float4(IN.position, 0, 1) * PushMatrices.mvp;
}
