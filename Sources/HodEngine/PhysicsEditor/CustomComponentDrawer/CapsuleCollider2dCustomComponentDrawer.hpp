#pragma once
#include "HodEngine/PhysicsEditor/Export.hpp"

#include "HodEngine/Editor/CustomComponentDrawer/CustomComponentDrawer.hpp"

namespace hod::inline editor
{
	/// @brief 
	class HOD_PHYSICS_EDITOR_API CapsuleCollider2dCustomComponentDrawer : public CustomComponentDrawer
	{
	public:

												CapsuleCollider2dCustomComponentDrawer() = default;
												CapsuleCollider2dCustomComponentDrawer(const CapsuleCollider2dCustomComponentDrawer&) = delete;
												CapsuleCollider2dCustomComponentDrawer(CapsuleCollider2dCustomComponentDrawer&&) = delete;
												~CapsuleCollider2dCustomComponentDrawer() override = default;

		CapsuleCollider2dCustomComponentDrawer	operator = (const CapsuleCollider2dCustomComponentDrawer&) = delete;
		CapsuleCollider2dCustomComponentDrawer	operator = (CapsuleCollider2dCustomComponentDrawer&&) = delete;

	public:

		bool									OnDrawGizmo(Component* component, GizmoViewport& viewport, bool selected) override;
	};
}
