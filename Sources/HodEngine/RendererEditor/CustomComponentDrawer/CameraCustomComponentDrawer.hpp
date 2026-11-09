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
	class HOD_RENDERER_EDITOR_API CameraCustomComponentDrawer : public CustomComponentDrawer
	{
	public:

											CameraCustomComponentDrawer();
											CameraCustomComponentDrawer(const CameraCustomComponentDrawer&) = delete;
											CameraCustomComponentDrawer(CameraCustomComponentDrawer&&) = delete;
											~CameraCustomComponentDrawer() override;

		CameraCustomComponentDrawer			operator = (const CameraCustomComponentDrawer&) = delete;
		CameraCustomComponentDrawer			operator = (CameraCustomComponentDrawer&&) = delete;

	public:

		bool								OnDrawGizmo(Component* component, GizmoViewport& viewport, bool selected) override;

	private:

		MaterialInstance*			_materialInstance = nullptr;
	};
}
