struct PushMatrices
{
	float4x4 mvp;
};

struct VS_IN
{
	float2 position : POSITION;
	float4 color : COLOR0;
};

struct VS_OUT
{
	float4 position : SV_Position;
	float4 color : COLOR0;
};

VS_OUT VertexMain(VS_IN input, uniform PushMatrices pc)
{
	VS_OUT output;
	output.position = mul(pc.mvp, float4(input.position, 0.0, 1.0));
	output.color = input.color;
	return output;
}

float4 FragmentMain(VS_OUT input) : SV_Target
{
	return input.color;
}
