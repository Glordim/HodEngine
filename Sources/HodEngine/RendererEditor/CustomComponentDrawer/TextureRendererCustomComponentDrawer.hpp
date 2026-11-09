#pragma once
#include "HodEngine/RendererEditor/Export.hpp"

#include "HodEngine/Editor/CustomComponentDrawer/CustomComponentDrawer.hpp"

namespace hod::inline renderer
{
	class MaterialInstance;
}

namespace hod::inline editor
{
	/// @brief 
	class HOD_RENDERER_EDITOR_API TextureRendererCustomComponentDrawer : public CustomComponentDrawer
	{
	public:

											TextureRendererCustomComponentDrawer();
											TextureRendererCustomComponentDrawer(const TextureRendererCustomComponentDrawer&) = delete;
											TextureRendererCustomComponentDrawer(TextureRendererCustomComponentDrawer&&) = delete;
											~TextureRendererCustomComponentDrawer() override;

		TextureRendererCustomComponentDrawer	operator = (const TextureRendererCustomComponentDrawer&) = delete;
		TextureRendererCustomComponentDrawer	operator = (TextureRendererCustomComponentDrawer&&) = delete;

	public:

		bool								OnDrawGizmo(Component* component, GizmoViewport& viewport, bool selected) override;

	private:

		MaterialInstance*			_materialInstance = nullptr;
	};
}
