#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include "HodEngine/GameEditor/ComponentGizmo/ComponentGizmo.hpp"

#include <HodEngine/Game/Components/Node2dComponent.hpp>

namespace hod::inline editor
{
	/// @brief The move handles of the active entity's node, at a constant size on screen: free move (the square), along its X and Y axes
	/// (the arrows); Escape cancels
	class HOD_GAME_EDITOR_API Node2dGizmo : public ComponentGizmo<Node2dComponent>
	{
	protected:
		bool OnDraw(Node2dComponent& node2d, GizmoViewport& viewport, bool active) override;
	};
}
