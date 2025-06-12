#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include "HodEngine/Editor/ComponentCustomEditor/ComponentCustomEditor.hpp"

namespace hod::renderer
{
	class MaterialInstance;
}

namespace hod::editor
{
	/// @brief 
	class HOD_GAME_EDITOR_API EdgeCollider2dComponentCustomEditor : public ComponentCustomEditor
	{
	public:

												EdgeCollider2dComponentCustomEditor();
												EdgeCollider2dComponentCustomEditor(const EdgeCollider2dComponentCustomEditor&) = delete;
												EdgeCollider2dComponentCustomEditor(EdgeCollider2dComponentCustomEditor&&) = delete;
												~EdgeCollider2dComponentCustomEditor() override;

		EdgeCollider2dComponentCustomEditor		operator = (const EdgeCollider2dComponentCustomEditor&) = delete;
		EdgeCollider2dComponentCustomEditor		operator = (EdgeCollider2dComponentCustomEditor&&) = delete;

	public:

		bool									OnDrawGizmo(game::Component* component, ViewportWindow& viewport) override;

	private:

		renderer::MaterialInstance*				_materialInstance = nullptr;
	};
}
