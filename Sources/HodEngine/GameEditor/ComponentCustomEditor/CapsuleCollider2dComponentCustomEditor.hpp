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
	class HOD_GAME_EDITOR_API CapsuleCollider2dComponentCustomEditor : public ComponentCustomEditor
	{
	public:

												CapsuleCollider2dComponentCustomEditor();
												CapsuleCollider2dComponentCustomEditor(const CapsuleCollider2dComponentCustomEditor&) = delete;
												CapsuleCollider2dComponentCustomEditor(CapsuleCollider2dComponentCustomEditor&&) = delete;
												~CapsuleCollider2dComponentCustomEditor() override;

		CapsuleCollider2dComponentCustomEditor	operator = (const CapsuleCollider2dComponentCustomEditor&) = delete;
		CapsuleCollider2dComponentCustomEditor	operator = (CapsuleCollider2dComponentCustomEditor&&) = delete;

	public:

		bool									OnDrawGizmo(game::Component* component, ViewportWindow& viewport) override;

	private:

		renderer::MaterialInstance*				_materialInstance = nullptr;
	};
}
