#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include "HodEngine/Editor/CustomComponentDrawer/CustomComponentDrawer.hpp"

namespace hod::renderer
{
	class MaterialInstance;
}

namespace hod::editor
{
	/// @brief 
	class HOD_GAME_EDITOR_API CircleCollider2dCustomComponentDrawer : public CustomComponentDrawer
	{
	public:

												CircleCollider2dCustomComponentDrawer();
												CircleCollider2dCustomComponentDrawer(const CircleCollider2dCustomComponentDrawer&) = delete;
												CircleCollider2dCustomComponentDrawer(CircleCollider2dCustomComponentDrawer&&) = delete;
												~CircleCollider2dCustomComponentDrawer() override;

		CircleCollider2dCustomComponentDrawer	operator = (const CircleCollider2dCustomComponentDrawer&) = delete;
		CircleCollider2dCustomComponentDrawer	operator = (CircleCollider2dCustomComponentDrawer&&) = delete;

	public:

		bool								OnDrawGizmo(game::Component* component, ViewportWindow& viewport, bool selected) override;

	private:

		renderer::MaterialInstance*			_materialInstance = nullptr;
	};
}
