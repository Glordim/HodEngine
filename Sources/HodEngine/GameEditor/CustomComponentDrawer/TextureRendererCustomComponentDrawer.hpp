#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include "HodEngine/Editor/CustomComponentDrawer/CustomComponentDrawer.hpp"

namespace hod::renderer
{
	class MaterialInstance;
}

namespace hod::inline editor
{
	/// @brief 
	class HOD_GAME_EDITOR_API TextureRendererCustomComponentDrawer : public CustomComponentDrawer
	{
	public:

											TextureRendererCustomComponentDrawer();
											TextureRendererCustomComponentDrawer(const TextureRendererCustomComponentDrawer&) = delete;
											TextureRendererCustomComponentDrawer(TextureRendererCustomComponentDrawer&&) = delete;
											~TextureRendererCustomComponentDrawer() override;

		TextureRendererCustomComponentDrawer	operator = (const TextureRendererCustomComponentDrawer&) = delete;
		TextureRendererCustomComponentDrawer	operator = (TextureRendererCustomComponentDrawer&&) = delete;

	public:

		bool								OnDrawGizmo(Component* component, ViewportWindow& viewport, bool selected) override;

	private:

		renderer::MaterialInstance*			_materialInstance = nullptr;
	};
}
