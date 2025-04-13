struct OUT
{
	float4 color : SV_TARGET;
};

void FragMain()
{
	OUT.color = float4(1, 0, 0, 1);
}
