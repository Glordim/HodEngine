struct IN
{
	float4 color : COLOR;
};

struct OUT
{
	float4 color : SV_TARGET;
};

void main()
{
	OUT.color = IN.color;
}
