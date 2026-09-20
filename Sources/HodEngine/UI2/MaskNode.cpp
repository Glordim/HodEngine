#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/UI2/MaskNode.hpp"
#include "HodEngine/UI2/DrawContext.hpp"

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>

namespace hod::inline ui2
{
	DESCRIBE_REFLECTED_CLASS(MaskNode, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief
	/// @param drawContext
	void MaskNode::DrawChildren(DrawContext& drawContext)
	{
		drawContext.PushClipRect(ComputeCanvasMatrix(), GetSize());
		Node::DrawChildren(drawContext);
		drawContext.PopClipRect();
	}
}
