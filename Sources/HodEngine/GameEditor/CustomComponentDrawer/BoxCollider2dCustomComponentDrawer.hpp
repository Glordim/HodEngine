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
	class HOD_GAME_EDITOR_API BoxCollider2dCustomComponentDrawer : public CustomComponentDrawer
	{
	public:

											BoxCollider2dCustomComponentDrawer();
											BoxCollider2dCustomComponentDrawer(const BoxCollider2dCustomComponentDrawer&) = delete;
											BoxCollider2dCustomComponentDrawer(BoxCollider2dCustomComponentDrawer&&) = delete;
											~BoxCollider2dCustomComponentDrawer() override;

		BoxCollider2dCustomComponentDrawer	operator = (const BoxCollider2dCustomComponentDrawer&) = delete;
		BoxCollider2dCustomComponentDrawer	operator = (BoxCollider2dCustomComponentDrawer&&) = delete;

	public:

		bool								OnDrawGizmo(game::Component* component, ViewportWindow& viewport, bool selected) override;

	private:

		renderer::MaterialInstance*			_materialInstance = nullptr;
	};
}
