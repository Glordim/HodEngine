struct IN
{
	float2 uv : TEXCOORD;
}

struct OUT
{
	float4 color : SV_TARGET;
}

void main()
{
	OUT.color = float4(1.0f, 0.0, 0.0, 1.0f);
}
