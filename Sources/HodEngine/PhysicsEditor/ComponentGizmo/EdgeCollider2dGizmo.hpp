#pragma once
#include "HodEngine/PhysicsEditor/Export.hpp"

#include <HodEngine/GameEditor/ComponentGizmo/ComponentGizmo.hpp>
#include <HodEngine/PhysicsComponents/Components/EdgeCollider2dComponent.hpp>

namespace hod::inline editor
{
	/// @brief The edge collider's segment, for the active entity
	class HOD_PHYSICS_EDITOR_API EdgeCollider2dGizmo : public ComponentGizmo<EdgeCollider2dComponent>
	{
	protected:
		bool OnDraw(EdgeCollider2dComponent& edgeCollider2d, GizmoViewport& viewport, bool active) override;
	};
}
