#pragma once
#include "HodEngine/PhysicsEditor/Export.hpp"

#include "HodEngine/Editor/CustomComponentDrawer/CustomComponentDrawer.hpp"

namespace hod::inline renderer
{
	class MaterialInstance;
}

namespace hod::inline editor
{
	/// @brief 
	class HOD_PHYSICS_EDITOR_API EdgeCollider2dCustomComponentDrawer : public CustomComponentDrawer
	{
	public:

												EdgeCollider2dCustomComponentDrawer();
												EdgeCollider2dCustomComponentDrawer(const EdgeCollider2dCustomComponentDrawer&) = delete;
												EdgeCollider2dCustomComponentDrawer(EdgeCollider2dCustomComponentDrawer&&) = delete;
												~EdgeCollider2dCustomComponentDrawer() override;

		EdgeCollider2dCustomComponentDrawer		operator = (const EdgeCollider2dCustomComponentDrawer&) = delete;
		EdgeCollider2dCustomComponentDrawer		operator = (EdgeCollider2dCustomComponentDrawer&&) = delete;

	public:

		bool									OnDrawGizmo(Component* component, GizmoViewport& viewport, bool selected) override;

	private:

		MaterialInstance*				_materialInstance = nullptr;
	};
}
