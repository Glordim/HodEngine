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
	class HOD_UI_EDITOR_API NodeCustomEditor : public ComponentCustomEditor
	{
	public:

										NodeCustomEditor();
										NodeCustomEditor(const NodeCustomEditor&) = delete;
										NodeCustomEditor(NodeCustomEditor&&) = delete;
										~NodeCustomEditor() override;

		NodeCustomEditor				operator = (const NodeCustomEditor&) = delete;
		NodeCustomEditor				operator = (NodeCustomEditor&&) = delete;

	public:

		bool							OnDrawInspector(EditorReflectedObject &reflectedObject) override;
		bool							OnDrawGizmo(game::Component* component, ViewportWindow& viewport, bool selected) override;

	private:

		static bool						DrawAnchorPresets(const ImVec2& size, Vector2& anchorMin, Vector2& anchorMax, Vector2& pivot);
		static Vector2					GetMouseWorldPos(const Vector2& mousePosition, const ViewportWindow& viewport);
		
	private:

		renderer::MaterialInstance*		_materialInstanceHitbox = nullptr;

		uint32_t						_pickingIdTopLeftCorner;
		uint32_t						_pickingIdTopRightCorner;
		uint32_t						_pickingIdBottomLeftCorner;
		uint32_t						_pickingIdBottomRightCorner;

		renderer::MaterialInstance*		_materialInstanceCorner = nullptr;
		renderer::MaterialInstance*		_materialInstanceCornerHighlight = nullptr;

		uint32_t						_pickingIdTopEdge;
		uint32_t						_pickingIdLeftEdge;
		uint32_t						_pickingIdBottomEdge;
		uint32_t						_pickingIdRightEdge;

		renderer::MaterialInstance*		_materialInstanceCenterNormal = nullptr;
		renderer::MaterialInstance*		_materialInstanceCenterHightlight = nullptr;

		renderer::MaterialInstance*		_materialInstanceAxisXNormal = nullptr;
		renderer::MaterialInstance*		_materialInstanceAxisXHightlight = nullptr;
		uint32_t						_pickingIdAxisX;

		renderer::MaterialInstance*		_materialInstanceAxisYNormal = nullptr;
		renderer::MaterialInstance*		_materialInstanceAxisYHightlight = nullptr;
		uint32_t						_pickingIdAxisY;

		renderer::MaterialInstance*		_materialInstanceAxisZNormal = nullptr;
		renderer::MaterialInstance*		_materialInstanceAxisZHightlight = nullptr;
		uint32_t						_pickingIdAxisZ;

		renderer::MaterialInstance*		_materialInstance = nullptr;
		renderer::MaterialInstance*		_materialInstanceHighlight = nullptr;

		uint32_t						_movingAxis = 0;
		Vector2							_pickingOffset;
		Vector2							_pickingSize;
	};
}
