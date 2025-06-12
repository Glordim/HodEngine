#pragma once
#include "HodEngine/UIEditor/Export.hpp"

#include "HodEngine/Editor/ComponentCustomEditor/ComponentCustomEditor.hpp"
#include <HodEngine/Core/Math/Vector2.hpp>

namespace hod::renderer
{
	class MaterialInstance;
}

namespace hod::editor
{
	/// @brief 
	class HOD_UI_EDITOR_API CanvasCustomEditor : public ComponentCustomEditor
	{
	public:

										CanvasCustomEditor();
										CanvasCustomEditor(const CanvasCustomEditor&) = delete;
										CanvasCustomEditor(CanvasCustomEditor&&) = delete;
										~CanvasCustomEditor() override;

		CanvasCustomEditor				operator = (const CanvasCustomEditor&) = delete;
		CanvasCustomEditor				operator = (CanvasCustomEditor&&) = delete;

	public:

		bool							OnDrawGizmo(game::Component* component, ViewportWindow& viewport, bool selected) override;
		
	private:

		renderer::MaterialInstance*		_materialInstance = nullptr;
	};
}
