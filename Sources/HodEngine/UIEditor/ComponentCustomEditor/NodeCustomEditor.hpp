#pragma once
#include "HodEngine/UIEditor/Export.hpp"

#include "HodEngine/Editor/ComponentCustomEditor/ComponentCustomEditor.hpp"
#include <HodEngine/Core/Math/Vector2.hpp>

#include "HodEngine/Editor/Gizmos/Gizmos.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

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
		
	private:

		Handle							_freeMoveHandle;

		Handle							_topLeftCorner;
		Handle							_topRightCorner;
		Handle							_bottomLeftCorner;
		Handle							_bottomRightCorner;

		Handle							_topEdge;
		Handle							_bottomEdge;
		Handle							_leftEdge;
		Handle							_rightEdge;

		Vector2							_pickingOffset;
		Vector2							_pickingPosition;
		Vector2							_pickingSize;
	};
}
