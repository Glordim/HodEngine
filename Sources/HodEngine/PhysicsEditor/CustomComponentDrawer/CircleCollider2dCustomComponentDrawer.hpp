#pragma once
#include "HodEngine/PhysicsEditor/Export.hpp"

#include "HodEngine/Editor/CustomComponentDrawer/CustomComponentDrawer.hpp"

namespace hod::inline editor
{
	/// @brief 
	class HOD_PHYSICS_EDITOR_API CircleCollider2dCustomComponentDrawer : public CustomComponentDrawer
	{
	public:

												CircleCollider2dCustomComponentDrawer() = default;
												CircleCollider2dCustomComponentDrawer(const CircleCollider2dCustomComponentDrawer&) = delete;
												CircleCollider2dCustomComponentDrawer(CircleCollider2dCustomComponentDrawer&&) = delete;
												~CircleCollider2dCustomComponentDrawer() override = default;

		CircleCollider2dCustomComponentDrawer	operator = (const CircleCollider2dCustomComponentDrawer&) = delete;
		CircleCollider2dCustomComponentDrawer	operator = (CircleCollider2dCustomComponentDrawer&&) = delete;

	public:

		bool								OnDrawGizmo(Component* component, GizmoViewport& viewport, bool selected) override;
	};
}
