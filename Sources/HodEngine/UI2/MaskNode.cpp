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

	/// @brief
	/// @return
	bool MaskNode::ClipsChildren() const
	{
		// Same simplification as the drawing: the hit-test clips to the mask's rect itself (rotated with it), the scissor
		// to its bounding box on screen, so a rotated mask's corners show children the pointer doesn't reach.
		return true;
	}
}
