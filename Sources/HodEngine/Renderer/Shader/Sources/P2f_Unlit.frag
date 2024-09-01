struct IN
{
};

struct OUT
{
	float4 color : SV_TARGET;
};

struct UBO
{
	float4 color;
};

void FragMain()
{
	OUT.color = UBO.color;
}
