#include "HodEngine/UIEditor/Pch.hpp"
#include "HodEngine/UIEditor/CustomPropertyDrawer/RectOffsetsCustomPropertyDrawer.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"

#include "HodEngine/Math/RectOffsets.hpp"
#include "HodEngine/ImGui/DearImGui/imgui.h"

#include <cmath>

namespace hod::inline editor
{
	/// @brief
	/// @param direction
	RectOffsetsCustomPropertyDrawer::RectOffsetsCustomPropertyDrawer(Direction direction)
	: _direction(direction)
	{
	}

	/// @brief
	/// @param editorReflectedProperty
	/// @return
	bool RectOffsetsCustomPropertyDrawer::Draw(EditorReflectedProperty& editorReflectedProperty, bool onlyValue)
	{
		bool changed = false;
		if (onlyValue == false)
		{
			changed |= PropertyDrawer::BeginProperty(editorReflectedProperty);
			ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);
		}

		// Edited through the RectOffsets base: the types deriving from it only add a meaning, no data
		RectOffsets value = *editorReflectedProperty.GetObject<RectOffsets>();

		// Each side on its own: the instances edited together may differ on one only
		bool mixedTop = false;
		bool mixedLeft = false;
		bool mixedRight = false;
		bool mixedBottom = false;
		for (uint32_t index = 1; index < editorReflectedProperty.GetInstances().Size(); ++index)
		{
			const RectOffsets& other = *editorReflectedProperty.GetObjectOf<RectOffsets>(index);
			mixedTop |= (other._top != value._top);
			mixedLeft |= (other._left != value._left);
			mixedRight |= (other._right != value._right);
			mixedBottom |= (other._bottom != value._bottom);
		}
		bool editedTop = false;
		bool editedLeft = false;
		bool editedRight = false;
		bool editedBottom = false;

		// Outward (margin): arrows towards the values. Inward (padding): arrows towards the content.
		bool isPadding = (_direction == Direction::Inward);

		ImGui::PushID(editorReflectedProperty.GetReflectionProperty());

		// Three rows: Top / Left - Node - Right / Bottom, the arrows in the gaps between the fields and the node.
		// Fixed size, whatever the width available: the cross keeps its shape.
		constexpr float connectorLength = 18.0f;
		constexpr float rowGap = connectorLength; // the vertical arrows as long as the horizontal ones
		constexpr float fieldWidth = 50.0f;
		constexpr float nodeWidth = 50.0f;
		constexpr float width = 2.0f * fieldWidth + 2.0f * connectorLength + nodeWidth;

		ImVec2 origin = ImGui::GetCursorScreenPos();
		float  rowHeight = ImGui::GetFrameHeight();
		float  centerX = origin.x + fieldWidth + connectorLength + nodeWidth * 0.5f;

		float topY = origin.y;
		float middleY = topY + rowHeight + rowGap;
		float bottomY = middleY + rowHeight + rowGap;

		ImVec2 nodeMin(origin.x + fieldWidth + connectorLength, middleY);
		ImVec2 nodeMax(nodeMin.x + nodeWidth, middleY + rowHeight);

		auto drawField = [&](const char* id, const ImVec2& position, float& field, bool mixed, bool& edited)
		{
			ImGui::SetCursorScreenPos(position);
			ImGui::SetNextItemWidth(fieldWidth);
			ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, mixed);
			edited = ImGui::DragFloat(id, &field, 1.0f, 0.0f, 0.0f, "%.3f");
			ImGui::PopItemFlag();
		};

		drawField("##Top", ImVec2(centerX - fieldWidth * 0.5f, topY), value._top, mixedTop, editedTop);
		drawField("##Left", ImVec2(origin.x, middleY), value._left, mixedLeft, editedLeft);
		drawField("##Right", ImVec2(nodeMax.x + connectorLength, middleY), value._right, mixedRight, editedRight);
		drawField("##Bottom", ImVec2(centerX - fieldWidth * 0.5f, bottomY), value._bottom, mixedBottom, editedBottom);

		// The node (or the content it pads) itself, and an arrow linking it to each value
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImU32       lineColor = ImGui::GetColorU32(ImGuiCol_TextDisabled);
		float       middleLineY = middleY + rowHeight * 0.5f;

		drawList->AddRectFilled(nodeMin, nodeMax, ImGui::GetColorU32(ImGuiCol_Button), ImGui::GetStyle().FrameRounding);
		drawList->AddRect(nodeMin, nodeMax, lineColor, ImGui::GetStyle().FrameRounding);
		const char* nodeLabel = isPadding ? "Child" : "Self";
		ImVec2      nodeLabelSize = ImGui::CalcTextSize(nodeLabel);
		drawList->AddText(ImVec2(centerX - nodeLabelSize.x * 0.5f, middleLineY - nodeLabelSize.y * 0.5f), lineColor, nodeLabel);

		// From the node side to the value side for a margin, the other way round for a padding. 2 px off both ends: not glued to the frames.
		auto drawArrow = [&](ImVec2 nodeSide, ImVec2 valueSide)
		{
			ImVec2 from = isPadding ? valueSide : nodeSide;
			ImVec2 to = isPadding ? nodeSide : valueSide;
			ImVec2 direction(to.x - from.x, to.y - from.y);
			float  length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
			direction = ImVec2(direction.x / length, direction.y / length);
			ImVec2 normal(-direction.y, direction.x);

			constexpr float endGap = 2.0f;
			constexpr float headLength = 6.0f;
			constexpr float headHalfWidth = 4.0f;
			from = ImVec2(from.x + direction.x * endGap, from.y + direction.y * endGap);
			to = ImVec2(to.x - direction.x * endGap, to.y - direction.y * endGap);
			ImVec2 headBase(to.x - direction.x * headLength, to.y - direction.y * headLength);

			drawList->AddLine(from, headBase, lineColor);
			drawList->AddTriangleFilled(to, ImVec2(headBase.x + normal.x * headHalfWidth, headBase.y + normal.y * headHalfWidth),
			                            ImVec2(headBase.x - normal.x * headHalfWidth, headBase.y - normal.y * headHalfWidth), lineColor);
		};

		drawArrow(ImVec2(centerX, nodeMin.y), ImVec2(centerX, topY + rowHeight));
		drawArrow(ImVec2(centerX, nodeMax.y), ImVec2(centerX, bottomY));
		drawArrow(ImVec2(nodeMin.x, middleLineY), ImVec2(origin.x + fieldWidth, middleLineY));
		drawArrow(ImVec2(nodeMax.x, middleLineY), ImVec2(nodeMax.x + connectorLength, middleLineY));

		// The fields were placed by hand: reserve the whole cross for the layout of what follows
		ImGui::SetCursorScreenPos(origin);
		ImGui::Dummy(ImVec2(width, bottomY + rowHeight - topY));

		ImGui::PopID();

		if (editedTop || editedLeft || editedRight || editedBottom)
		{
			editorReflectedProperty.EditObjects<RectOffsets>(
				[&](RectOffsets& object)
				{
					object._top = editedTop ? value._top : object._top;
					object._left = editedLeft ? value._left : object._left;
					object._right = editedRight ? value._right : object._right;
					object._bottom = editedBottom ? value._bottom : object._bottom;
				});
			changed = true;
		}
		return changed;
	}
}
