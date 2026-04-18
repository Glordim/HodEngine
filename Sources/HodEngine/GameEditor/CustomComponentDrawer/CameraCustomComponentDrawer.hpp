#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include "HodEngine/Editor/CustomComponentDrawer/CustomComponentDrawer.hpp"

namespace hod::inline renderer
{
	class MaterialInstance;
}

namespace hod::inline editor
{
	/// @brief 
	class HOD_GAME_EDITOR_API CameraCustomComponentDrawer : public CustomComponentDrawer
	{
	public:

											CameraCustomComponentDrawer();
											CameraCustomComponentDrawer(const CameraCustomComponentDrawer&) = delete;
											CameraCustomComponentDrawer(CameraCustomComponentDrawer&&) = delete;
											~CameraCustomComponentDrawer() override;

		CameraCustomComponentDrawer			operator = (const CameraCustomComponentDrawer&) = delete;
		CameraCustomComponentDrawer			operator = (CameraCustomComponentDrawer&&) = delete;

	public:

		bool								OnDrawGizmo(Component* component, ViewportWindow& viewport, bool selected) override;

	private:

		renderer::MaterialInstance*			_materialInstance = nullptr;
	};
}
