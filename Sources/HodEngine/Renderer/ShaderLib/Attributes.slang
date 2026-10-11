// Marks the constant buffer SetConstant writes to.
// Goes with [[vk::push_constant]], which only Vulkan knows: on the other targets the buffer is an ordinary one, told apart by this attribute.
[__AttributeUsage(_AttributeTargets.Var)]
struct PushConstantAttribute
{
};

[__AttributeUsage(_AttributeTargets.Var)]
struct ColorAttribute
{
};