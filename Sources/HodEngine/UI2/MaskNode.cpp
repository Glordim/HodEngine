#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/UI2/MaskNode.hpp"
#include "HodEngine/UI2/DrawContext.hpp"

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitIcon.hpp>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

namespace hod::inline ui2
{
	DESCRIBE_REFLECTED_CLASS(MaskNode, reflectionDescriptor)
	{
		reflectionDescriptor.AddTrait<ReflectionTraitIcon>(ICON_MDI_CROP_FREE);
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
