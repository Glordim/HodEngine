#include "HodEngine/UIEditor/Pch.hpp"
#include "HodEngine/Editor/AnchorPresetsDrawer.hpp"
#include "HodEngine/Editor/DrawHelper.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/ViewportWindow.hpp"
#include "HodEngine/UIEditor/CustomComponentDrawer/NodeCustomComponentDrawer.hpp"
#include <HodEngine/Game/Entity.hpp>
#include <HodEngine/UI/Node.hpp>

#include <HodEngine/Renderer/MaterialManager.hpp>
#include <HodEngine/Renderer/PickingManager.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>
#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/Renderer/RHI/MaterialInstance.hpp>
#include <HodEngine/Renderer/RHI/RenderTarget.hpp>

#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"
#include "HodEngine/Editor/ViewportWindow.hpp"

#undef max

namespace hod::inline editor
{
	/// @brief
	NodeCustomComponentDrawer::NodeCustomComponentDrawer()
	{
		_freeMoveHandle = Gizmos::GenerateHandle();

		_topLeftCorner = Gizmos::GenerateHandle();
		_topRightCorner = Gizmos::GenerateHandle();
		_bottomLeftCorner = Gizmos::GenerateHandle();
		_bottomRightCorner = Gizmos::GenerateHandle();

		_topEdge = Gizmos::GenerateHandle();
		_bottomEdge = Gizmos::GenerateHandle();
		_leftEdge = Gizmos::GenerateHandle();
		_rightEdge = Gizmos::GenerateHandle();
	}

	/// @brief
	NodeCustomComponentDrawer::~NodeCustomComponentDrawer() {}

	bool NodeCustomComponentDrawer::OnDrawInspector(EditorReflectedObject& reflectedObject)
	{
		bool changed = false;

		EditorReflectedProperty* anchorMin = reflectedObject.FindProperty("AnchorMin");
		EditorReflectedProperty* anchorMax = reflectedObject.FindProperty("AnchorMax");
		EditorReflectedProperty* pivot = reflectedObject.FindProperty("Pivot");

		EditorReflectedProperty* deltaSize = reflectedObject.FindProperty("DeltaSize");

		EditorReflectedProperty* position = reflectedObject.FindProperty("Position");
		EditorReflectedProperty* rotation = reflectedObject.FindProperty("Rotation");
		EditorReflectedProperty* scale = reflectedObject.FindProperty("Scale");

		Vector2 deltaSizeValue = *deltaSize->GetObject<Vector2>();

		ImVec2 cursorPos = ImGui::GetCursorPos();

		Vector2 anchorMinValue = *anchorMin->GetObject<Vector2>();
		Vector2 anchorMaxValue = *anchorMax->GetObject<Vector2>();
		Vector2 pivotValue = *pivot->GetObject<Vector2>();
		if (AnchorPresetsDrawer::Draw(ImVec2(100.0f, 100.0f), anchorMinValue, anchorMaxValue, pivotValue))
		{
			anchorMin->SetObject(anchorMinValue);
			anchorMax->SetObject(anchorMaxValue);
			pivot->SetObject(pivotValue);
			changed = true;
		}

		ImGui::SetCursorPos(cursorPos + ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing()));
		ImGui::Indent(120.0f);

		changed |= PropertyDrawer::DrawProperty(*anchorMin);
		changed |= PropertyDrawer::DrawProperty(*anchorMax);
		changed |= PropertyDrawer::DrawProperty(*pivot);

		ImGui::Unindent(120.0f);

		ImGui::SetCursorPos(cursorPos + ImVec2(0.0f, 120.0f));

		const char* anchoredPosLabelX = "X";
		const char* anchoredPosLabelY = "Y";
		const char* deltaSizeLabelX = "Width";
		const char* deltaSizeLabelY = "Height";

		Vector2 anchoredPositionValue = *position->GetObject<Vector2>();
		ImGui::TextUnformatted("Anchored Position");
		ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);
		if (DrawHelper::DrawVector2(anchoredPositionValue, anchoredPosLabelX, anchoredPosLabelY))
		{
			position->SetObject(anchoredPositionValue);
			changed = true;
		}

		ImGui::TextUnformatted("Delta Size");
		ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);
		if (DrawHelper::DrawVector2(deltaSizeValue, deltaSizeLabelX, deltaSizeLabelY))
		{
			deltaSize->SetObject(deltaSizeValue);
			changed = true;
		}

		ImGui::Spacing();

		changed |= PropertyDrawer::DrawProperty(*rotation);
		changed |= PropertyDrawer::DrawProperty(*scale);

		return changed;
	}

	/// @brief
	/// @param component
	/// @param projection
	/// @param view
	/// @param operation
	/// @return
	bool NodeCustomComponentDrawer::OnDrawGizmo(Component* component, ViewportWindow& viewport, bool selected)
	{
		Node* node = static_cast<Node*>(component);
		if (node == nullptr)
		{
			return false;
		}

		Vector2 Size = node->ComputeSize();
		Matrix4 worldMatrix = node->ComputeWorldMatrix();

		if (selected == false)
		{
			Gizmos::Rect(worldMatrix, Size, Color(0.75f, 0.75f, 0.75f, 1.0f), *viewport.GetRenderView());

			Vector2 position;
			Handle  selectionHandle;
			selectionHandle._pickingId = (uint32_t)node->GetOwner()->GetInstanceId();
			selectionHandle._sortingOrder = node->GetZOrder();
			Gizmos::FreeMoveRect(selectionHandle, worldMatrix, position, Size, Color(0.0f, 0.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 0.0f, 0.0f), viewport);

			return false;
		}

		bool changed = false;

		static Color hitboxHandleColor(0.0f, 0.0f, 0.0f, 0.0f);

		_freeMoveHandle._pickingId = (uint32_t)node->GetOwner()->GetInstanceId();
		_freeMoveHandle._sortingOrder = node->GetZOrder();
		changed |= Gizmos::FreeMoveRect(_freeMoveHandle, worldMatrix, Vector2::Zero, Size, hitboxHandleColor, hitboxHandleColor, viewport);
		if (_freeMoveHandle._justPressed)
		{
			_pickingPosition = node->GetPosition();
		}
		if (_freeMoveHandle._pressed)
		{
			node->SetPosition(_pickingPosition + _freeMoveHandle._delta * 100.0f); // TODO NOP ! * 100.0f
		}

		float handleSize = Gizmos::GetHandleSize(0.01f, viewport);

		changed |= Gizmos::FreeMoveRect(_topEdge, worldMatrix, Vector2(0.0f, Size.GetY() * 0.5f), Vector2(Size.GetX(), handleSize), hitboxHandleColor, hitboxHandleColor, viewport);
		changed |=
			Gizmos::FreeMoveRect(_bottomEdge, worldMatrix, Vector2(0.0f, -Size.GetY() * 0.5f), Vector2(Size.GetX(), handleSize), hitboxHandleColor, hitboxHandleColor, viewport);
		changed |=
			Gizmos::FreeMoveRect(_leftEdge, worldMatrix, Vector2(-Size.GetX() * 0.5f, 0.0f), Vector2(handleSize, Size.GetY()), hitboxHandleColor, hitboxHandleColor, viewport);
		changed |=
			Gizmos::FreeMoveRect(_rightEdge, worldMatrix, Vector2(Size.GetX() * 0.5f, 0.0f), Vector2(handleSize, Size.GetY()), hitboxHandleColor, hitboxHandleColor, viewport);

		static Color lineColor(0.75f, 0.75f, 0.75f, 1.0f);
		static Color lineHighlightColor(0.75f, 0.75f, 0.75f, 1.0f);

		Gizmos::Line(worldMatrix, Vector2(-Size.GetX() * 0.5f, Size.GetY() * 0.5f), Vector2(Size.GetX() * 0.5f, Size.GetY() * 0.5f),
		             _topEdge._hovered ? lineHighlightColor : lineColor, *viewport.GetRenderView());
		Gizmos::Line(worldMatrix, Vector2(-Size.GetX() * 0.5f, -Size.GetY() * 0.5f), Vector2(Size.GetX() * 0.5f, -Size.GetY() * 0.5f),
		             _bottomEdge._hovered ? lineHighlightColor : lineColor, *viewport.GetRenderView());
		Gizmos::Line(worldMatrix, Vector2(-Size.GetX() * 0.5f, Size.GetY() * 0.5f), Vector2(-Size.GetX() * 0.5f, -Size.GetY() * 0.5f),
		             _leftEdge._hovered ? lineHighlightColor : lineColor, *viewport.GetRenderView());
		Gizmos::Line(worldMatrix, Vector2(Size.GetX() * 0.5f, Size.GetY() * 0.5f), Vector2(Size.GetX() * 0.5f, -Size.GetY() * 0.5f),
		             _rightEdge._hovered ? lineHighlightColor : lineColor, *viewport.GetRenderView());

		static Color cornerColor(0.25f, 0.25f, 1.0f, 1.0f);
		static Color cornerHighlightColor(0.5f, 0.5f, 1.0f, 1.0f);

		changed |= Gizmos::FreeMoveCircle(_topLeftCorner, worldMatrix, Vector2(-Size.GetX() * 0.5f, Size.GetY() * 0.5f), handleSize, cornerColor, cornerHighlightColor, viewport);
		changed |= Gizmos::FreeMoveCircle(_topRightCorner, worldMatrix, Vector2(Size.GetX() * 0.5f, Size.GetY() * 0.5f), handleSize, cornerColor, cornerHighlightColor, viewport);
		changed |=
			Gizmos::FreeMoveCircle(_bottomLeftCorner, worldMatrix, Vector2(-Size.GetX() * 0.5f, -Size.GetY() * 0.5f), handleSize, cornerColor, cornerHighlightColor, viewport);
		changed |=
			Gizmos::FreeMoveCircle(_bottomRightCorner, worldMatrix, Vector2(Size.GetX() * 0.5f, -Size.GetY() * 0.5f), handleSize, cornerColor, cornerHighlightColor, viewport);

		if (_topLeftCorner._hovered || _bottomRightCorner._hovered)
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNWSE);
		}
		else if (_topRightCorner._hovered || _bottomLeftCorner._hovered)
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNESW);
		}
		else if (_topEdge._hovered || _bottomEdge._hovered)
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
		}
		else if (_leftEdge._hovered || _rightEdge._hovered)
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
		}

		if (_topEdge._justPressed || _bottomEdge._justPressed || _leftEdge._justPressed || _rightEdge._justPressed || _topLeftCorner._justPressed || _topRightCorner._justPressed ||
		    _bottomLeftCorner._justPressed || _bottomRightCorner._justPressed)
		{
			_pickingSize = node->GetDeltaSize();
			_pickingPosition = node->GetPosition();
		}

		Vector2 pivot = node->GetPivot();

		Vector2 deltaSize;
		Vector2 reverse;
		if (_topEdge._pressed)
		{
			deltaSize = Vector2(0.0f, _topEdge._delta.GetY());
			reverse = Vector2(0.0f, 0.0f);
		}
		else if (_bottomEdge._pressed)
		{
			deltaSize = Vector2(0.0f, -_bottomEdge._delta.GetY());
			reverse = Vector2(0.0f, 1.0f);
		}
		else if (_leftEdge._pressed)
		{
			deltaSize = Vector2(-_leftEdge._delta.GetX(), 0.0f);
			reverse = Vector2(1.0f, 0.0f);
		}
		else if (_rightEdge._pressed)
		{
			deltaSize = Vector2(_rightEdge._delta.GetX(), 0.0f);
			reverse = Vector2(0.0f, 0.0f);
		}
		else if (_topLeftCorner._pressed)
		{
			deltaSize = Vector2(-_topLeftCorner._delta.GetX(), _topLeftCorner._delta.GetY());
			reverse = Vector2(1.0f, 0.0f);
		}
		else if (_topRightCorner._pressed)
		{
			deltaSize = Vector2(_topRightCorner._delta.GetX(), _topRightCorner._delta.GetY());
			reverse = Vector2(0.0f, 0.0f);
		}
		else if (_bottomLeftCorner._pressed)
		{
			deltaSize = Vector2(-_bottomLeftCorner._delta.GetX(), -_bottomLeftCorner._delta.GetY());
			reverse = Vector2(1.0f, 1.0f);
		}
		else if (_bottomRightCorner._pressed)
		{
			deltaSize = Vector2(_bottomRightCorner._delta.GetX(), -_bottomRightCorner._delta.GetY());
			reverse = Vector2(0.0f, 1.0f);
		}

		if (deltaSize != Vector2::Zero)
		{
			deltaSize *= 100.0f; // TODO NOP !
			Vector2 newSize = _pickingSize + deltaSize;
			Vector2 newPosition = _pickingPosition - deltaSize * (reverse - pivot);

			if (newSize != _pickingSize)
			{
				node->SetDeltaSize(newSize);
				node->SetPosition(newPosition);
			}
		}

		return changed;
	}
}
