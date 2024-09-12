#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/ComponentCustomEditor/ComponentCustomEditor.hpp"

namespace hod::editor
{
	/// @brief 
	class HOD_EDITOR_API Node2dComponentCustomEditor : public ComponentCustomEditor
	{
	public:

										Node2dComponentCustomEditor() = default;
										Node2dComponentCustomEditor(const Node2dComponentCustomEditor&) = delete;
										Node2dComponentCustomEditor(Node2dComponentCustomEditor&&) = delete;
										~Node2dComponentCustomEditor() override = default;

		Node2dComponentCustomEditor		operator = (const Node2dComponentCustomEditor&) = delete;
		Node2dComponentCustomEditor		operator = (Node2dComponentCustomEditor&&) = delete;

	public:

		bool							OnDrawGizmo(std::shared_ptr<game::Component> component, const Matrix4& projection, const Matrix4& view, ImGuizmo::OPERATION operation, renderer::RenderQueue& renderQueue) override;
	};
}
