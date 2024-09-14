#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/ComponentCustomEditor/ComponentCustomEditor.hpp"

namespace hod::renderer
{
	class MaterialInstance;
}

namespace hod::editor
{
	/// @brief 
	class HOD_EDITOR_API BoxCollider2dComponentCustomEditor : public ComponentCustomEditor
	{
	public:

											BoxCollider2dComponentCustomEditor();
											BoxCollider2dComponentCustomEditor(const BoxCollider2dComponentCustomEditor&) = delete;
											BoxCollider2dComponentCustomEditor(BoxCollider2dComponentCustomEditor&&) = delete;
											~BoxCollider2dComponentCustomEditor() override = default;

		BoxCollider2dComponentCustomEditor	operator = (const BoxCollider2dComponentCustomEditor&) = delete;
		BoxCollider2dComponentCustomEditor	operator = (BoxCollider2dComponentCustomEditor&&) = delete;

	public:

		bool								OnDrawGizmo(std::shared_ptr<game::Component> component, const Matrix4& projection, const Matrix4& view, ImGuizmo::OPERATION operation, renderer::RenderQueue& renderQueue) override;

	private:

		renderer::MaterialInstance*			_materialInstance = nullptr;
	};
}
