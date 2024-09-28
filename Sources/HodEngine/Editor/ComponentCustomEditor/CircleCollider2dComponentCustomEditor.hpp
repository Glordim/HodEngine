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
	class HOD_EDITOR_API CircleCollider2dComponentCustomEditor : public ComponentCustomEditor
	{
	public:

												CircleCollider2dComponentCustomEditor();
												CircleCollider2dComponentCustomEditor(const CircleCollider2dComponentCustomEditor&) = delete;
												CircleCollider2dComponentCustomEditor(CircleCollider2dComponentCustomEditor&&) = delete;
												~CircleCollider2dComponentCustomEditor() override;

		CircleCollider2dComponentCustomEditor	operator = (const CircleCollider2dComponentCustomEditor&) = delete;
		CircleCollider2dComponentCustomEditor	operator = (CircleCollider2dComponentCustomEditor&&) = delete;

	public:

		bool								OnDrawGizmo(std::shared_ptr<game::Component> component, ViewportWindow& viewport) override;

	private:

		renderer::MaterialInstance*			_materialInstance = nullptr;
	};
}
