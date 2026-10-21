import Attributes;

// The same as ConstantFirstTest.slang, but for the order it declares its push constant and its set in: the set first.
// On Metal, where the compiler numbers the buffers in the order they are declared, the set is at buffer 0 and the constant at buffer 1.

struct UBO
{
	float4 color;
};

struct Material
{
	ConstantBuffer<UBO> ubo;
};
ParameterBlock<Material> material;

struct PushMatrices
{
	float4x4 mvp;
};
[PushConstant]
[[vk::push_constant]]
ConstantBuffer<PushMatrices> pc;

struct VS_IN
{
	float2 position : POSITION;
	float2 uv : TEXTCOORD;
};

struct VS_OUT
{
	float4 position : SV_Position;
	float4 color : COLOR;
};

// Both stages read the set, the vertex one the constant too
VS_OUT VertexMain(VS_IN input)
{
	VS_OUT output;
	output.position = mul(pc.mvp, float4(input.position, 0.0, 1.0));
	output.color = material.ubo.color;
	return output;
}

float4 FragmentMain(VS_OUT input) : SV_Target
{
	return input.color * material.ubo.color;
}
