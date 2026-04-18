#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include "HodEngine/Editor/CustomComponentDrawer/CustomComponentDrawer.hpp"

namespace hod::inline renderer
{
	class MaterialInstance;
}

namespace hod::inline editor
{
	/// @brief 
	class HOD_GAME_EDITOR_API EdgeCollider2dCustomComponentDrawer : public CustomComponentDrawer
	{
	public:

												EdgeCollider2dCustomComponentDrawer();
												EdgeCollider2dCustomComponentDrawer(const EdgeCollider2dCustomComponentDrawer&) = delete;
												EdgeCollider2dCustomComponentDrawer(EdgeCollider2dCustomComponentDrawer&&) = delete;
												~EdgeCollider2dCustomComponentDrawer() override;

		EdgeCollider2dCustomComponentDrawer		operator = (const EdgeCollider2dCustomComponentDrawer&) = delete;
		EdgeCollider2dCustomComponentDrawer		operator = (EdgeCollider2dCustomComponentDrawer&&) = delete;

	public:

		bool									OnDrawGizmo(Component* component, ViewportWindow& viewport, bool selected) override;

	private:

		MaterialInstance*				_materialInstance = nullptr;
	};
}
