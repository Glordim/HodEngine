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
	class HOD_EDITOR_API TextureRendererComponentCustomEditor : public ComponentCustomEditor
	{
	public:

											TextureRendererComponentCustomEditor();
											TextureRendererComponentCustomEditor(const TextureRendererComponentCustomEditor&) = delete;
											TextureRendererComponentCustomEditor(TextureRendererComponentCustomEditor&&) = delete;
											~TextureRendererComponentCustomEditor() override;

		TextureRendererComponentCustomEditor	operator = (const TextureRendererComponentCustomEditor&) = delete;
		TextureRendererComponentCustomEditor	operator = (TextureRendererComponentCustomEditor&&) = delete;

	public:

		bool								OnDrawGizmo(std::shared_ptr<game::Component> component, ViewportWindow& viewport) override;

	private:

		renderer::MaterialInstance*			_materialInstance = nullptr;
	};
}
