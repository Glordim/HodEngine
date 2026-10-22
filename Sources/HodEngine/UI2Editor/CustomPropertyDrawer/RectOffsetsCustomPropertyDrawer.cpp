#include "HodEngine/UI2Editor/Pch.hpp"
#include "HodEngine/UI2Editor/CustomPropertyDrawer/RectOffsetsCustomPropertyDrawer.hpp"
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

		auto drawField = [&](const char* id, const ImVec2& position, float& field)
		{
			ImGui::SetCursorScreenPos(position);
			ImGui::SetNextItemWidth(fieldWidth);
			changed |= ImGui::DragFloat(id, &field, 1.0f, 0.0f, 0.0f, "%.3f");
		};

		drawField("##Top", ImVec2(centerX - fieldWidth * 0.5f, topY), value._top);
		drawField("##Left", ImVec2(origin.x, middleY), value._left);
		drawField("##Right", ImVec2(nodeMax.x + connectorLength, middleY), value._right);
		drawField("##Bottom", ImVec2(centerX - fieldWidth * 0.5f, bottomY), value._bottom);

		// The node (or the content it pads) itself, and an arrow linking it to each value
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImU32       lineColor = ImGui::GetColorU32(ImGuiCol_TextDisabled);
		float       middleLineY = middleY + rowHeight * 0.5f;

		drawList->AddRectFilled(nodeMin, nodeMax, ImGui::GetColorU32(ImGuiCol_Button), ImGui::GetStyle().FrameRounding);
		drawList->AddRect(nodeMin, nodeMax, lineColor, ImGui::GetStyle().FrameRounding);
		const char* nodeLabel = isPadding ? "Child" : "Node";
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

		if (changed == true)
		{
			editorReflectedProperty.SetObject(value);
		}
		return changed;
	}
}
