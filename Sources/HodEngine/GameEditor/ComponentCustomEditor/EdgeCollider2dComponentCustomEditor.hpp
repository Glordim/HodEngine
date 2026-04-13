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
	class HOD_GAME_EDITOR_API EdgeCollider2dComponentCustomEditor : public CustomComponentDrawer
	{
	public:

												EdgeCollider2dComponentCustomEditor();
												EdgeCollider2dComponentCustomEditor(const EdgeCollider2dComponentCustomEditor&) = delete;
												EdgeCollider2dComponentCustomEditor(EdgeCollider2dComponentCustomEditor&&) = delete;
												~EdgeCollider2dComponentCustomEditor() override;

		EdgeCollider2dComponentCustomEditor		operator = (const EdgeCollider2dComponentCustomEditor&) = delete;
		EdgeCollider2dComponentCustomEditor		operator = (EdgeCollider2dComponentCustomEditor&&) = delete;

	public:

		bool									OnDrawGizmo(game::Component* component, ViewportWindow& viewport, bool selected) override;

	private:

		renderer::MaterialInstance*				_materialInstance = nullptr;
	};
}
