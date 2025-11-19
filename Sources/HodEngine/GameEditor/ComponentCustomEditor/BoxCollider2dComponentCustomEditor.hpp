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
	class HOD_GAME_EDITOR_API BoxCollider2dComponentCustomEditor : public ComponentCustomEditor
	{
	public:

											BoxCollider2dComponentCustomEditor();
											BoxCollider2dComponentCustomEditor(const BoxCollider2dComponentCustomEditor&) = delete;
											BoxCollider2dComponentCustomEditor(BoxCollider2dComponentCustomEditor&&) = delete;
											~BoxCollider2dComponentCustomEditor() override;

		BoxCollider2dComponentCustomEditor	operator = (const BoxCollider2dComponentCustomEditor&) = delete;
		BoxCollider2dComponentCustomEditor	operator = (BoxCollider2dComponentCustomEditor&&) = delete;

	public:

		bool								OnDrawGizmo(game::Component* component, ViewportWindow& viewport, bool selected) override;

	private:

		renderer::MaterialInstance*			_materialInstance = nullptr;
	};
}
