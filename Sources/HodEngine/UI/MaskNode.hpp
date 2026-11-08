#pragma once
#include "HodEngine/UI/Export.hpp"
#include "HodEngine/UI/Node.hpp"

namespace hod::inline ui
{
	/// @brief Clips its children to its own rect: whatever they draw outside of it is not shown.
	/// Nested masks intersect. The clip is a scissor, so it is always the axis-aligned bounding box
	/// of the rect on screen (a rotated mask clips to that box, and corners can't be rounded).
	class HOD_UI_API MaskNode : public Node
	{
		REFLECTED_CLASS(MaskNode, Node)

	protected:

		void	DrawChildren(DrawContext& drawContext) override;
		bool	ClipsChildren() const override;
	};
}
