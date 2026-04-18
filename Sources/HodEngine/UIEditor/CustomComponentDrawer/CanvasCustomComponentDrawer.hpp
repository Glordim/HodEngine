#pragma once
#include "HodEngine/UIEditor/Export.hpp"

#include "HodEngine/Editor/CustomComponentDrawer/CustomComponentDrawer.hpp"
#include <HodEngine/Math/Vector2.hpp>

namespace hod::renderer
{
	class MaterialInstance;
}

namespace hod::editor
{
	/// @brief 
	class HOD_UI_EDITOR_API CanvasCustomComponentDrawer : public CustomComponentDrawer
	{
	public:

										CanvasCustomComponentDrawer();
										CanvasCustomComponentDrawer(const CanvasCustomComponentDrawer&) = delete;
										CanvasCustomComponentDrawer(CanvasCustomComponentDrawer&&) = delete;
										~CanvasCustomComponentDrawer() override;

		CanvasCustomComponentDrawer				operator = (const CanvasCustomComponentDrawer&) = delete;
		CanvasCustomComponentDrawer				operator = (CanvasCustomComponentDrawer&&) = delete;

	public:

		bool							OnDrawInspector(EditorReflectedObject &reflectedObject) override;
		bool							OnDrawGizmo(Component* component, ViewportWindow& viewport, bool selected) override;
		
	private:

		renderer::MaterialInstance*		_materialInstance = nullptr;
	};
}
