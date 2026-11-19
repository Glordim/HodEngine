#pragma once
#include "HodEngine/PhysicsEditor/Export.hpp"

#include <HodEngine/GameEditor/ComponentGizmo/ComponentGizmo.hpp>
#include <HodEngine/PhysicsComponents/Components/MultiShapeCollider2dComponent.hpp>

namespace hod::inline editor
{
	/// @brief The outlines of the multi shape collider's shapes, for the active entity
	class HOD_PHYSICS_EDITOR_API MultiShapeCollider2dGizmo : public ComponentGizmo<MultiShapeCollider2dComponent>
	{
	protected:
		bool OnDraw(MultiShapeCollider2dComponent& multiShapeCollider2d, GizmoViewport& viewport, bool active) override;
	};
}
