#pragma once
#include "HodEngine/PhysicsEditor/Export.hpp"

#include <HodEngine/GameEditor/ComponentGizmo/ComponentGizmo.hpp>
#include <HodEngine/PhysicsComponents/Components/CircleCollider2dComponent.hpp>

namespace hod::inline editor
{
	/// @brief The circle collider's outline, for the active entity
	class HOD_PHYSICS_EDITOR_API CircleCollider2dGizmo : public ComponentGizmo<CircleCollider2dComponent>
	{
	protected:
		bool OnDraw(CircleCollider2dComponent& circleCollider2d, GizmoViewport& viewport, bool active) override;
	};
}
