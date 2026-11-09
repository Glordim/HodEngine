#pragma once
#include "HodEngine/PhysicsEditor/Export.hpp"

#include "HodEngine/Editor/CustomComponentDrawer/CustomComponentDrawer.hpp"

namespace hod::inline editor
{
	/// @brief 
	class HOD_PHYSICS_EDITOR_API BoxCollider2dCustomComponentDrawer : public CustomComponentDrawer
	{
	public:

											BoxCollider2dCustomComponentDrawer() = default;
											BoxCollider2dCustomComponentDrawer(const BoxCollider2dCustomComponentDrawer&) = delete;
											BoxCollider2dCustomComponentDrawer(BoxCollider2dCustomComponentDrawer&&) = delete;
											~BoxCollider2dCustomComponentDrawer() override = default;

		BoxCollider2dCustomComponentDrawer	operator = (const BoxCollider2dCustomComponentDrawer&) = delete;
		BoxCollider2dCustomComponentDrawer	operator = (BoxCollider2dCustomComponentDrawer&&) = delete;

	public:

		bool								OnDrawGizmo(Component* component, GizmoViewport& viewport, bool selected) override;
	};
}
