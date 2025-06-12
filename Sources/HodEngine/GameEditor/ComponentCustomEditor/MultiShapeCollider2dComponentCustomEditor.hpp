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
	class HOD_GAME_EDITOR_API MultiShapeCollider2dComponentCustomEditor : public ComponentCustomEditor
	{
	public:

													MultiShapeCollider2dComponentCustomEditor();
													MultiShapeCollider2dComponentCustomEditor(const MultiShapeCollider2dComponentCustomEditor&) = delete;
													MultiShapeCollider2dComponentCustomEditor(MultiShapeCollider2dComponentCustomEditor&&) = delete;
													~MultiShapeCollider2dComponentCustomEditor() override;

		MultiShapeCollider2dComponentCustomEditor	operator = (const MultiShapeCollider2dComponentCustomEditor&) = delete;
		MultiShapeCollider2dComponentCustomEditor	operator = (MultiShapeCollider2dComponentCustomEditor&&) = delete;

	public:

		bool										OnDrawGizmo(game::Component* component, ViewportWindow& viewport, bool selected) override;

	private:

		renderer::MaterialInstance*					_materialInstance = nullptr;
	};
}
