#pragma once
#include "HodEngine/PhysicsEditor/Export.hpp"

#include <HodEngine/GameEditor/ComponentGizmo/ComponentGizmo.hpp>
#include <HodEngine/PhysicsComponents/Components/CapsuleCollider2dComponent.hpp>

namespace hod::inline editor
{
	/// @brief The capsule collider's outline, for the active entity
	class HOD_PHYSICS_EDITOR_API CapsuleCollider2dGizmo : public ComponentGizmo<CapsuleCollider2dComponent>
	{
	protected:
		bool OnDraw(CapsuleCollider2dComponent& capsuleCollider2d, GizmoViewport& viewport, bool active) override;
	};
}
