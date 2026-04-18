#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include "HodEngine/Editor/CustomComponentDrawer/CustomComponentDrawer.hpp"

namespace hod::renderer
{
	class MaterialInstance;
}

namespace hod::inline editor
{
	/// @brief 
	class HOD_GAME_EDITOR_API CapsuleCollider2dCustomComponentDrawer : public CustomComponentDrawer
	{
	public:

												CapsuleCollider2dCustomComponentDrawer();
												CapsuleCollider2dCustomComponentDrawer(const CapsuleCollider2dCustomComponentDrawer&) = delete;
												CapsuleCollider2dCustomComponentDrawer(CapsuleCollider2dCustomComponentDrawer&&) = delete;
												~CapsuleCollider2dCustomComponentDrawer() override;

		CapsuleCollider2dCustomComponentDrawer	operator = (const CapsuleCollider2dCustomComponentDrawer&) = delete;
		CapsuleCollider2dCustomComponentDrawer	operator = (CapsuleCollider2dCustomComponentDrawer&&) = delete;

	public:

		bool									OnDrawGizmo(Component* component, ViewportWindow& viewport, bool selected) override;

	private:

		renderer::MaterialInstance*				_materialInstance = nullptr;
	};
}
