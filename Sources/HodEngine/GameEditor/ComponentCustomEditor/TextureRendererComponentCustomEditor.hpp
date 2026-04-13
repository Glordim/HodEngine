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
	class HOD_GAME_EDITOR_API TextureRendererComponentCustomEditor : public CustomComponentDrawer
	{
	public:

											TextureRendererComponentCustomEditor();
											TextureRendererComponentCustomEditor(const TextureRendererComponentCustomEditor&) = delete;
											TextureRendererComponentCustomEditor(TextureRendererComponentCustomEditor&&) = delete;
											~TextureRendererComponentCustomEditor() override;

		TextureRendererComponentCustomEditor	operator = (const TextureRendererComponentCustomEditor&) = delete;
		TextureRendererComponentCustomEditor	operator = (TextureRendererComponentCustomEditor&&) = delete;

	public:

		bool								OnDrawGizmo(game::Component* component, ViewportWindow& viewport, bool selected) override;

	private:

		renderer::MaterialInstance*			_materialInstance = nullptr;
	};
}
