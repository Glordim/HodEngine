struct OUT
{
	float4 color : SV_TARGET;
};

void main()
{
	OUT.color = float4(1, 0, 0, 1);
}
