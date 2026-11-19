#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include "HodEngine/GameEditor/ComponentGizmo/ComponentGizmo.hpp"

#include <HodEngine/Editor/Gizmos/Handles.hpp>
#include <HodEngine/Game/Components/Node2dComponent.hpp>
#include <HodEngine/Math/Vector2.hpp>

namespace hod::inline editor
{
	/// @brief The move handles of the active entity's node: free move, along X, along Y (Escape cancels)
	class HOD_GAME_EDITOR_API Node2dGizmo : public ComponentGizmo<Node2dComponent>
	{
	public:
		Node2dGizmo();

	protected:
		bool OnDraw(Node2dComponent& node2d, GizmoViewport& viewport, bool active) override;

	private:
		Handle _freeMoveHandle;

		Handle _XAxisHandle;
		Handle _YAxisHandle;

		Vector2 _initialPosition;
	};
}
