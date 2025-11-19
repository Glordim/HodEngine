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
	class HOD_GAME_EDITOR_API CircleCollider2dComponentCustomEditor : public ComponentCustomEditor
	{
	public:

												CircleCollider2dComponentCustomEditor();
												CircleCollider2dComponentCustomEditor(const CircleCollider2dComponentCustomEditor&) = delete;
												CircleCollider2dComponentCustomEditor(CircleCollider2dComponentCustomEditor&&) = delete;
												~CircleCollider2dComponentCustomEditor() override;

		CircleCollider2dComponentCustomEditor	operator = (const CircleCollider2dComponentCustomEditor&) = delete;
		CircleCollider2dComponentCustomEditor	operator = (CircleCollider2dComponentCustomEditor&&) = delete;

	public:

		bool								OnDrawGizmo(game::Component* component, ViewportWindow& viewport, bool selected) override;

	private:

		renderer::MaterialInstance*			_materialInstance = nullptr;
	};
}
