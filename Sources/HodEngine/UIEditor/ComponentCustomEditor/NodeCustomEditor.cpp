#include "HodEngine/UIEditor/Pch.hpp"
#include "HodEngine/UIEditor/ComponentCustomEditor/NodeCustomEditor.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/ViewportWindow.hpp"
#include "HodEngine/Editor/DrawHelper.hpp"
#include <HodEngine/UI/Node.hpp>

#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/PickingManager.hpp>
#include <HodEngine/Renderer/MaterialManager.hpp>
#include <HodEngine/Renderer/RHI/MaterialInstance.hpp>
#include <HodEngine/Renderer/RenderQueue.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>
#include <HodEngine/Renderer/RHI/RenderTarget.hpp>

#include "HodEngine/Editor/ViewportWindow.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"

#include "HodEngine/ImGui/DearImGui/imgui_internal.h"

#undef max

namespace hod::editor
{
	/// @brief 
	NodeCustomEditor::NodeCustomEditor()
	{
		_materialInstanceCenterNormal = renderer::Renderer::GetInstance()->CreateMaterialInstance(renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Triangle));
		_materialInstanceCenterNormal->SetVec4("UBO.color", Vector4(0.5f, 0.5f, 0.5f, 1.0f));

		_materialInstanceCenterHightlight = renderer::Renderer::GetInstance()->CreateMaterialInstance(renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Triangle));
		_materialInstanceCenterHightlight->SetVec4("UBO.color", Vector4(1.0f, 0.5f, 0.5f, 1.0f));

		_materialInstanceAxisXNormal = renderer::Renderer::GetInstance()->CreateMaterialInstance(renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Triangle));
		_materialInstanceAxisXNormal->SetVec4("UBO.color", Vector4(1.0f, 0.0f, 0.0f, 1.0f));

		_materialInstanceAxisXHightlight = renderer::Renderer::GetInstance()->CreateMaterialInstance(renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Triangle));
		_materialInstanceAxisXHightlight->SetVec4("UBO.color", Vector4(1.0f, 0.5f, 0.5f, 1.0f));

		_pickingIdAxisX = renderer::PickingManager::GetInstance()->GenerateId();

		_materialInstanceAxisYNormal = renderer::Renderer::GetInstance()->CreateMaterialInstance(renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Triangle));
		_materialInstanceAxisYNormal->SetVec4("UBO.color", Vector4(0.0f, 1.0f, 0.0f, 1.0f));

		_materialInstanceAxisYHightlight = renderer::Renderer::GetInstance()->CreateMaterialInstance(renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Triangle));
		_materialInstanceAxisYHightlight->SetVec4("UBO.color", Vector4(0.5f, 1.0f, 0.5f, 1.0f));

		_pickingIdAxisY = renderer::PickingManager::GetInstance()->GenerateId();

		_materialInstanceAxisZNormal = renderer::Renderer::GetInstance()->CreateMaterialInstance(renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Triangle));
		_materialInstanceAxisZNormal->SetVec4("UBO.color", Vector4(0.0f, 0.0f, 1.0f, 1.0f));

		_materialInstanceAxisZHightlight = renderer::Renderer::GetInstance()->CreateMaterialInstance(renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Triangle));
		_materialInstanceAxisZHightlight->SetVec4("UBO.color", Vector4(0.5f, 0.5f, 1.0f, 1.0f));

		_pickingIdAxisZ = renderer::PickingManager::GetInstance()->GenerateId();

		_materialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Line_LineStrip));
		_materialInstance->SetVec4("UBO.color", Vector4(0.75f, 0.75f, 0.75f, 1.0f));
	}

	/// @brief 
	NodeCustomEditor::~NodeCustomEditor()
	{
		DefaultAllocator::GetInstance().Delete(_materialInstanceCenterNormal);
		DefaultAllocator::GetInstance().Delete(_materialInstanceCenterHightlight);
		DefaultAllocator::GetInstance().Delete(_materialInstanceAxisXNormal);
		DefaultAllocator::GetInstance().Delete(_materialInstanceAxisXHightlight);
		DefaultAllocator::GetInstance().Delete(_materialInstanceAxisYNormal);
		DefaultAllocator::GetInstance().Delete(_materialInstanceAxisYHightlight);
		DefaultAllocator::GetInstance().Delete(_materialInstanceAxisZNormal);
		DefaultAllocator::GetInstance().Delete(_materialInstanceAxisZHightlight);

		DefaultAllocator::GetInstance().Delete(_materialInstance);
	}

	bool NodeCustomEditor::OnDrawInspector(EditorReflectedObject& reflectedObject)
	{
		bool changed = false;

		EditorReflectedProperty* anchorMin = reflectedObject.FindProperty("AnchorMin");
		EditorReflectedProperty* anchorMax = reflectedObject.FindProperty("AnchorMax");
		EditorReflectedProperty* pivot = reflectedObject.FindProperty("Pivot");

		EditorReflectedProperty* deltaSize = reflectedObject.FindProperty("DeltaSize");

		EditorReflectedProperty* position = reflectedObject.FindProperty("Position");
		EditorReflectedProperty* rotation = reflectedObject.FindProperty("Rotation");
		EditorReflectedProperty* scale = reflectedObject.FindProperty("Scale");

		Vector2 anchorDiff = *anchorMax->GetObject<Vector2>() - *anchorMin->GetObject<Vector2>();

		Vector2 deltaSizeValue = *deltaSize->GetObject<Vector2>();

		ImVec2 cursorPos = ImGui::GetCursorPos();

		Vector2 anchorMinValue = *anchorMin->GetObject<Vector2>();
		Vector2 anchorMaxValue = *anchorMax->GetObject<Vector2>();
		Vector2 pivotValue = *pivot->GetObject<Vector2>();
		if (DrawAnchorPresets(ImVec2(100.0f, 100.0f), anchorMinValue, anchorMaxValue, pivotValue))
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
		const char* anchoredPosLabelY = "X";
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

	void AddTextVertical(ImDrawList* DrawList, const char *text, ImVec2 pos, ImU32 text_color)
	{
		pos.x = std::round(pos.x);
		pos.y = std::round(pos.y);
		ImFont* font = ImGui::GetFont();
		const ImFontGlyph *glyph;
		char c;
		ImVec2 text_size = ImGui::CalcTextSize(text);
		while ((c = *text++))
		{
			glyph = font->FindGlyph(c);
			if (!glyph) continue;

			DrawList->PrimReserve(6, 4);
			DrawList->PrimQuadUV(
					pos + ImVec2(glyph->Y0, -glyph->X0),
					pos + ImVec2(glyph->Y0, -glyph->X1),
					pos + ImVec2(glyph->Y1, -glyph->X1),
					pos + ImVec2(glyph->Y1, -glyph->X0),

					ImVec2(glyph->U0, glyph->V0),
					ImVec2(glyph->U1, glyph->V0),
					ImVec2(glyph->U1, glyph->V1),
					ImVec2(glyph->U0, glyph->V1),

					text_color);

			pos.y -= glyph->AdvanceX;
		}
	}

	enum Edge : uint8_t
	{
		None = 0,
		Top = (1 << 0),
		Left = (1 << 1),
		Right = (1 << 2),
		Bottom = (1 << 3),
		CenterH = (1 << 4),
		CenterV = (1 << 5),
	};

	void DrawAchorPresetParentBounds(ImDrawList* drawList, ImVec2 pos, ImVec2 size, uint8_t coloredEdges)
	{
		ImU32 normalColor = IM_COL32(100, 100, 100, 255);
		ImU32 highlightColor = IM_COL32(255, 100, 100, 255);

		float thickness = 1.0f;

		ImVec2 topLeft = pos;
		ImVec2 topRight = pos + ImVec2(size.x, 0.0f);
		ImVec2 bottomLeft = pos + ImVec2(0, size.y);
		ImVec2 bottomRight = pos + ImVec2(size.x, size.y);

		drawList->AddLine(topLeft, topRight, coloredEdges & Edge::Top ? highlightColor : normalColor, thickness);
		drawList->AddLine(topLeft, bottomLeft, coloredEdges & Edge::Left ? highlightColor : normalColor, thickness);
		drawList->AddLine(bottomLeft, bottomRight, coloredEdges & Edge::Bottom ? highlightColor : normalColor, thickness);
		drawList->AddLine(bottomRight, topRight, coloredEdges & Edge::Right ? highlightColor : normalColor, thickness);

		if (coloredEdges & Edge::CenterH)
			drawList->AddLine(pos + ImVec2(0, size.y * 0.5f), pos + ImVec2(size.x, size.y * 0.5f), highlightColor, thickness);

		if (coloredEdges & Edge::CenterV)
			drawList->AddLine(pos + ImVec2(size.x * 0.5f, 0.0f), pos + ImVec2(size.x * 0.5f, size.y), highlightColor, thickness);
	}

	enum Anchor : uint16_t
	{
		TopLeft = (1 << 0),
		TopCenter = (1 << 1),
		TopRight = (1 << 2),
		MiddleLeft = (1 << 3),
		MiddleCenter = (1 << 4),
		MiddleRight = (1 << 5),
		BottomLeft = (1 << 6),
		BottomCenter = (1 << 7),
		BottomRight = (1 << 8),
	};

	void DrawAchorPresetParentAnchor(ImDrawList* drawList, ImVec2 pos, ImVec2 size, uint16_t coloredAnchors)
	{
		ImU32 color = IM_COL32(200, 200, 0, 255);

		float radius = 3.0f;

		if (coloredAnchors & Anchor::TopLeft)
			drawList->AddCircleFilled(pos + ImVec2(size.x * 0.0f, 0.0f), radius, color);
		if (coloredAnchors & Anchor::TopCenter)
			drawList->AddCircleFilled(pos + ImVec2(size.x * 0.5f, 0.0f), radius, color);
		if (coloredAnchors & Anchor::TopRight)
			drawList->AddCircleFilled(pos + ImVec2(size.x * 1.0f, 0.0f), radius, color);

		if (coloredAnchors & Anchor::MiddleLeft)
			drawList->AddCircleFilled(pos + ImVec2(size.x * 0.0f, size.y * 0.5f), radius, color);
		if (coloredAnchors & Anchor::MiddleCenter)
			drawList->AddCircleFilled(pos + ImVec2(size.x * 0.5f, size.y * 0.5f), radius, color);
		if (coloredAnchors & Anchor::MiddleRight)
			drawList->AddCircleFilled(pos + ImVec2(size.x * 1.0f, size.y * 0.5f), radius, color);

		if (coloredAnchors & Anchor::BottomLeft)
			drawList->AddCircleFilled(pos + ImVec2(size.x * 0.0f, size.y), radius, color);
		if (coloredAnchors & Anchor::BottomCenter)
			drawList->AddCircleFilled(pos + ImVec2(size.x * 0.5f, size.y), radius, color);
		if (coloredAnchors & Anchor::BottomRight)
			drawList->AddCircleFilled(pos + ImVec2(size.x * 1.0f, size.y), radius, color);
	}

	enum ExpandArrow : uint8_t
	{
		Horizontal = (1 << 0),
		Vertical = (1 << 1),
	};

	void DrawAchorPresetChildBounds(ImDrawList* drawList, ImVec2 pos, ImVec2 size, uint8_t expandArrows)
	{
		ImU32 boundsColor = IM_COL32(200, 200, 200, 255);
		ImU32 expandArrowColor = IM_COL32(100, 200, 255, 255);

		float thickness = 1.0f;

		drawList->AddRect(pos, pos + size, boundsColor, 0.0f, 0, thickness);

		float arrowWidth = 8.0f;
		float arrowHeight = 4.0f;

		if (expandArrows & Horizontal)
		{
			ImVec2 leftArrow = pos + ImVec2(0.0f, size.y * 0.5f);
			ImVec2 rightArrow = pos + ImVec2(size.x, size.y * 0.5f);
			drawList->AddLine(leftArrow, rightArrow, expandArrowColor, thickness);
			drawList->AddTriangleFilled(ImFloor(leftArrow), ImFloor(leftArrow + ImVec2(arrowHeight, -arrowWidth * 0.5f)), ImFloor(leftArrow + ImVec2(arrowHeight, arrowWidth * 0.5f)), expandArrowColor);
			drawList->AddTriangleFilled(rightArrow, rightArrow + ImVec2(-arrowHeight, -arrowWidth * 0.5f), rightArrow + ImVec2(-arrowHeight, arrowWidth * 0.5f), expandArrowColor);
		}
		
		if (expandArrows & Vertical)
		{
			ImVec2 topArrow = pos + ImVec2(size.x * 0.5f, 0.0f);
			ImVec2 bottomArrow = pos + ImVec2(size.x * 0.5f, size.y);
			drawList->AddLine(topArrow, bottomArrow, expandArrowColor, thickness);
			drawList->AddTriangleFilled(topArrow, topArrow + ImVec2(-arrowWidth * 0.5f, arrowHeight), topArrow + ImVec2(arrowWidth * 0.5f, arrowHeight), expandArrowColor);
			drawList->AddTriangleFilled(ImFloor(bottomArrow), ImFloor(bottomArrow + ImVec2(-arrowWidth * 0.5f, -arrowHeight)), ImFloor(bottomArrow + ImVec2(arrowWidth * 0.5f, -arrowHeight)), expandArrowColor);
		}
	}

	bool DrawPresetButton(const char* id, const ImVec2& size, uint8_t edge, uint16_t anchors, uint8_t expandArrows, Vector2& anchorMin, Vector2& anchorMax, Vector2& pivot, Vector2 presetAnchorMin, Vector2 presetAnchorMax)
	{
		ImVec2 childSize = size * 0.5f;
		ImVec2 pos = ImGui::GetCursorScreenPos();

		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
		if (anchorMin != presetAnchorMin || anchorMax != presetAnchorMax)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		bool clicked = ImGui::Button(id, size);
		if (anchorMin != presetAnchorMin || anchorMax != presetAnchorMax)
			ImGui::PopStyleColor();
		ImGui::PopStyleVar(2);

		DrawAchorPresetParentBounds(ImGui::GetWindowDrawList(), pos, size, edge);
		DrawAchorPresetParentAnchor(ImGui::GetWindowDrawList(), pos, size, anchors);
		DrawAchorPresetChildBounds(ImGui::GetWindowDrawList(), pos + childSize * 0.5f, childSize, expandArrows);

		if (clicked)
		{
			anchorMin = presetAnchorMin;
			anchorMax = presetAnchorMax;
			//pivot = Vector2(0.5f, 0.5f);
		}
		return clicked;
	}

	/// @brief 
	/// @param anchorMin 
	/// @param anchorMax 
	/// @param pivot 
	/// @return 
	bool NodeCustomEditor::DrawAnchorPresets(const ImVec2& size, Vector2& anchorMin, Vector2& anchorMax, Vector2& pivot)
	{
		bool changed = false;

		uint8_t edges = 0;
		uint16_t anchors = 0;
		uint8_t expandArrows = 0;
		const char* topLabel = "";
		const char* leftLabel = "";

		if (anchorMin.GetX() != anchorMax.GetX())
		{
			topLabel = "stretch";
			expandArrows |= ExpandArrow::Horizontal;
		}
		else
		{
			if (anchorMin.GetX() == 0.0f)
			{
				topLabel = "left";
				edges |= Edge::Left;
			}
			else if (anchorMin.GetX() == 0.5f)
			{
				topLabel = "center";
				edges |= Edge::CenterV;
			}
			else if (anchorMin.GetX() == 1.0f)
			{
				topLabel = "right";
				edges |= Edge::Right;
			}
		}
		if (anchorMin.GetY() != anchorMax.GetY())
		{
			leftLabel = "stretch";
			expandArrows |= ExpandArrow::Vertical;
		}
		else
		{
			if (anchorMin.GetY() == 0.0f)
			{
				leftLabel = "bottom";
				edges |= Edge::Bottom;
			}
			else if (anchorMin.GetY() == 0.5f)
			{
				leftLabel = "center";
				edges |= Edge::CenterH;
			}
			else if (anchorMin.GetY() == 1.0f)
			{
				leftLabel = "top";
				edges |= Edge::Top;
			}
		}

		if ((edges & (Edge::Top | Edge::Left)) == (Edge::Top | Edge::Left))
			anchors = Anchor::TopLeft;
		else if ((edges & (Edge::Top | Edge::CenterV)) == (Edge::Top | Edge::CenterV))
			anchors = Anchor::TopCenter;
		else if ((edges & (Edge::Top | Edge::Right)) == (Edge::Top | Edge::Right))
			anchors = Anchor::TopRight;
		else if ((edges & (Edge::CenterH | Edge::Left)) == (Edge::CenterH | Edge::Left))
			anchors = Anchor::MiddleLeft;
		else if ((edges & (Edge::CenterH | Edge::CenterV)) == (Edge::CenterH | Edge::CenterV))
			anchors = Anchor::MiddleCenter;
		else if ((edges & (Edge::CenterH | Edge::Right)) == (Edge::CenterH | Edge::Right))
			anchors = Anchor::MiddleRight;
		else if ((edges & (Edge::Bottom | Edge::Left)) == (Edge::Bottom | Edge::Left))
			anchors = Anchor::BottomLeft;
		else if ((edges & (Edge::Bottom | Edge::CenterV)) == (Edge::Bottom | Edge::CenterV))
			anchors = Anchor::BottomCenter;
		else if ((edges & (Edge::Bottom | Edge::Right)) == (Edge::Bottom | Edge::Right))
			anchors = Anchor::BottomRight;
		else if ((edges & Edge::Top) && expandArrows & ExpandArrow::Horizontal)
			anchors = Anchor::TopLeft | Anchor::TopRight;
		else if ((edges & Edge::CenterH) && expandArrows & ExpandArrow::Horizontal)
			anchors = Anchor::MiddleLeft | Anchor::MiddleRight;
		else if ((edges & Edge::Bottom) && expandArrows & ExpandArrow::Horizontal)
			anchors = Anchor::BottomLeft | Anchor::BottomRight;
		else if ((edges & Edge::Left) && expandArrows & ExpandArrow::Vertical)
			anchors = Anchor::TopLeft | Anchor::BottomLeft;
		else if ((edges & Edge::CenterV) && expandArrows & ExpandArrow::Vertical)
			anchors = Anchor::TopCenter | Anchor::BottomCenter;
		else if ((edges & Edge::Right) && expandArrows & ExpandArrow::Vertical)
			anchors = Anchor::TopRight | Anchor::BottomRight;
		else if ((expandArrows & (ExpandArrow::Horizontal | ExpandArrow::Vertical)) == (ExpandArrow::Horizontal | ExpandArrow::Vertical))
			anchors = Anchor::TopLeft | Anchor::TopRight | Anchor::BottomLeft | Anchor::BottomRight;

		ImVec2 buttonSize(size.x - ImGui::GetTextLineHeightWithSpacing(), size.y - ImGui::GetTextLineHeightWithSpacing());

		ImVec2 cursorPos = ImGui::GetCursorPos();
		ImGui::SetCursorPosX(cursorPos.x + ImGui::GetTextLineHeightWithSpacing() + buttonSize.x * 0.5f - ImGui::CalcTextSize(topLabel).x * 0.5f);
		ImGui::TextUnformatted(topLabel);
		ImGui::SetCursorPos(cursorPos);
		AddTextVertical(ImGui::GetWindowDrawList(), leftLabel, ImGui::GetCursorScreenPos() + ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() + buttonSize.y * 0.5f - ImGui::CalcTextSize(leftLabel).x * 0.5f + ImGui::CalcTextSize(leftLabel).x), ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Text)));
		ImGui::SetCursorPos(cursorPos + ImVec2(ImGui::GetTextLineHeightWithSpacing(), ImGui::GetTextLineHeightWithSpacing()));

		ImVec2 presetSize = ImVec2(buttonSize.x - 24, buttonSize.y - 24);
		ImVec2 childSize = presetSize * 0.5f;

		ImVec2 presetButtonScreenPosition = ImGui::GetCursorScreenPos();
		bool clicked = ImGui::Button("##AnchorPresets", buttonSize);
		ImVec2 pos = presetButtonScreenPosition + ImVec2(12, 12);
		DrawAchorPresetParentBounds(ImGui::GetWindowDrawList(), pos, presetSize, edges);
		DrawAchorPresetParentAnchor(ImGui::GetWindowDrawList(), pos, presetSize, anchors);
		DrawAchorPresetChildBounds(ImGui::GetWindowDrawList(), pos + childSize * 0.5f, childSize, expandArrows);

		if (clicked)
		{
			ImGui::OpenPopup("AnchorPresets");
		}

		ImGui::SetNextWindowPos(presetButtonScreenPosition + ImVec2(0.0f, buttonSize.y), ImGuiCond_Appearing);
		if (ImGui::BeginPopup("AnchorPresets", ImGuiWindowFlags_NoMove))
		{
			ImGui::TextUnformatted("Anchor Presets");

			changed |= DrawPresetButton("##AnchorPresetTopLeft", presetSize, Edge::Top | Edge::Left, Anchor::TopLeft, 0, anchorMin, anchorMax, pivot, Vector2(0.0f, 1.0f), Vector2(0.0f, 1.0f));
			ImGui::SameLine();
			changed |= DrawPresetButton("##AnchorPresetTopCenter", presetSize, Edge::Top | Edge::CenterV, Anchor::TopCenter, 0, anchorMin, anchorMax, pivot, Vector2(0.5f, 1.0f), Vector2(0.5f, 1.0f));
			ImGui::SameLine();
			changed |= DrawPresetButton("##AnchorPresetTopRight", presetSize, Edge::Top | Edge::Right, Anchor::TopRight, 0, anchorMin, anchorMax, pivot, Vector2(1.0f, 1.0f), Vector2(1.0f, 1.0f));
			ImGui::SameLine(0.0f, 25.0f);
			changed |= DrawPresetButton("##AnchorPresetTopStretch", presetSize, Edge::Top | Edge::CenterV, Anchor::TopLeft | Anchor::TopRight, ExpandArrow::Horizontal, anchorMin, anchorMax, pivot, Vector2(0.0f, 1.0f), Vector2(1.0f, 1.0f));


			changed |= DrawPresetButton("##AnchorPresetMiddleLeft", presetSize, Edge::CenterH | Edge::Left, Anchor::MiddleLeft, 0, anchorMin, anchorMax, pivot, Vector2(0.0f, 0.5f), Vector2(0.0f, 0.5f));
			ImGui::SameLine();
			changed |= DrawPresetButton("##AnchorPresetMiddleCenter", presetSize, Edge::CenterH | Edge::CenterV, Anchor::MiddleCenter, 0, anchorMin, anchorMax, pivot, Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f));
			ImGui::SameLine();
			changed |= DrawPresetButton("##AnchorPresetMiddleRight", presetSize, Edge::CenterH | Edge::Right, Anchor::MiddleRight, 0, anchorMin, anchorMax, pivot, Vector2(1.0f, 0.5f), Vector2(1.0f, 0.5f));
			ImGui::SameLine(0.0f, 25.0f);
			changed |= DrawPresetButton("##AnchorPresetMiddleStretch", presetSize, Edge::CenterH | Edge::CenterV, Anchor::MiddleLeft | Anchor::MiddleRight, ExpandArrow::Horizontal, anchorMin, anchorMax, pivot, Vector2(0.0f, 0.5f), Vector2(1.0f, 0.5f));


			changed |= DrawPresetButton("##AnchorPresetBottomLeft", presetSize, Edge::Bottom | Edge::Left, Anchor::BottomLeft, 0, anchorMin, anchorMax, pivot, Vector2(0.0f, 0.0f), Vector2(0.0f, 0.0f));
			ImGui::SameLine();
			changed |= DrawPresetButton("##AnchorPresetBottomCenter", presetSize, Edge::Bottom | Edge::CenterV, Anchor::BottomCenter, 0, anchorMin, anchorMax, pivot, Vector2(0.5f, 0.0f), Vector2(0.5f, 0.0f));
			ImGui::SameLine();
			changed |= DrawPresetButton("##AnchorPresetBottomRight", presetSize, Edge::Bottom | Edge::Right, Anchor::BottomRight, 0, anchorMin, anchorMax, pivot, Vector2(1.0f, 0.0f), Vector2(1.0f, 0.0f));
			ImGui::SameLine(0.0f, 25.0f);
			changed |= DrawPresetButton("##AnchorPresetBottomStretch", presetSize, Edge::Bottom | Edge::CenterV, Anchor::BottomLeft | Anchor::BottomRight, ExpandArrow::Horizontal, anchorMin, anchorMax, pivot, Vector2(0.0f, 0.0f), Vector2(1.0f, 0.0f));

			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Spacing();

			changed |= DrawPresetButton("##AnchorPresetStretchLeft", presetSize, Edge::Left, Anchor::TopLeft | Anchor::BottomLeft, ExpandArrow::Vertical, anchorMin, anchorMax, pivot, Vector2(0.0f, 0.0f), Vector2(0.0f, 1.0f));
			ImGui::SameLine();
			changed |= DrawPresetButton("##AnchorPresetStretchMiddle", presetSize, Edge::CenterV, Anchor::TopCenter | Anchor::BottomCenter, ExpandArrow::Vertical, anchorMin, anchorMax, pivot, Vector2(0.5f, 0.0f), Vector2(0.5f, 1.0f));
			ImGui::SameLine();
			changed |= DrawPresetButton("##AnchorPresetStretchRight", presetSize, Edge::Right, Anchor::TopRight | Anchor::BottomRight, ExpandArrow::Vertical, anchorMin, anchorMax, pivot, Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f));
			ImGui::SameLine(0.0f, 25.0f);
			changed |= DrawPresetButton("##AnchorPresetStretchStretch", presetSize, 0, Anchor::TopLeft | Anchor::TopRight | Anchor::BottomLeft | Anchor::BottomRight, ExpandArrow::Horizontal | ExpandArrow::Vertical, anchorMin, anchorMax, pivot, Vector2(0.0f, 0.0f), Vector2(1.0f, 1.0f));

			if (ImGui::BeginTable("PresetsTable", 5))
			{
				//DrawAchorPresetsParentBounds(ImGui::GetWindowDrawList(), Edge::None, ImVec2(buttonSize.x - 4, buttonSize.y - 4));

				ImGui::EndTable();
			}

			ImGui::EndPopup();
		}

		return changed;
	}

	/// @brief 
	/// @param component 
	/// @param projection 
	/// @param view 
	/// @param operation 
	/// @return 
	bool NodeCustomEditor::OnDrawGizmo(game::Component* component, ViewportWindow& viewport, bool selected)
	{
		if (selected == false)
		{
			return false;
		}

		bool changed = false;

		ui::Node* node = static_cast<ui::Node*>(component);
		if (node != nullptr)
		{
			ImVec2 mousePos = ImGui::GetIO().MousePos - ImGui::GetCursorScreenPos();
			Vector2 mousePosition(mousePos.x, mousePos.y);
			Color pickingColor = viewport.GetPickingRenderTarget()->GetColorTexture()->ReadPixel(mousePosition);
			uint32_t pickingId = renderer::PickingManager::ConvertColorToId(pickingColor);

			if (ImGui::GetIO().MouseDown[ImGuiMouseButton_Left])
			{
				if (ImGui::GetIO().MouseDownDuration[ImGuiMouseButton_Left] == 0.0f && pickingId != 0 && _movingAxis == 0)
				{
					if (pickingId == _pickingIdAxisX || pickingId == _pickingIdAxisY || pickingId == _pickingIdAxisZ)
					{
						_movingAxis = pickingId;

						Vector2 mouseWorldPos = GetMouseWorldPos(mousePosition, viewport);
						_pickingOffset = mouseWorldPos - node->GetPosition();
					}
				}
			}
			else
			{
				_movingAxis = 0;
			}

			Matrix4 finalMatrix = Matrix4::Translation(node->ComputeCanvasMatrix().GetTranslation()); // Backup gizmo matrix before modification to avoid desync/lag
			
			if (_movingAxis != 0 && (ImGui::GetIO().MouseDelta.x != 0.0f || ImGui::GetIO().MouseDelta.y != 0.0f))
			{
				Vector2 mouseWorldPos = GetMouseWorldPos(mousePosition, viewport);
				Vector2 previousPosition = node->GetPosition();
				Vector2 newPosition = mouseWorldPos - _pickingOffset;
				if (_movingAxis == _pickingIdAxisX)
				{
					newPosition.SetY(previousPosition.GetY());
				}
				else if (_movingAxis == _pickingIdAxisY)
				{
					newPosition.SetX(previousPosition.GetX());
				}

				if (newPosition != previousPosition)
				{
					node->SetPosition(newPosition);
					changed = true;
				}
			}

			constexpr float thickness = 0.05f;
			constexpr float length = 1.0f;
			constexpr float headHeight = 0.15f;
			constexpr float headLength = 0.25f;
			constexpr float lineLength = length - headLength;
			constexpr float squareSize = 0.25f;
			constexpr float squareOffset = 0.5f;

			std::array<Vector2, 9> verticesX = {
				Vector2(0.0f, thickness * 0.5f),
				Vector2(0.0f, -thickness * 0.5f),
				Vector2(lineLength, thickness * 0.5f),
				
				Vector2(0.0f, -thickness * 0.5f),
				Vector2(lineLength, -thickness * 0.5f),
				Vector2(lineLength, thickness * 0.5f),

				Vector2(lineLength, headHeight * 0.5f),
				Vector2(lineLength, -headHeight * 0.5f),
				Vector2(length, 0.0f),
			};

			renderer::RenderCommandMesh* renderMeshCommand = DefaultAllocator::GetInstance().New<renderer::RenderCommandMesh>(verticesX.data(), nullptr, nullptr, (uint32_t)verticesX.size(), nullptr, 0, finalMatrix, _movingAxis != _pickingIdAxisX && pickingId != _pickingIdAxisX ? _materialInstanceAxisXNormal : _materialInstanceAxisXHightlight, std::numeric_limits<uint32_t>::max(), _pickingIdAxisX);
			viewport.GetRenderQueue()->PushRenderCommand(renderMeshCommand);

			std::array<Vector2, 9> verticesY = {
				Vector2(thickness * 0.5f, 0.0f),
				Vector2(-thickness * 0.5f, 0.0f),
				Vector2(thickness * 0.5f, lineLength),
				
				Vector2(-thickness * 0.5f, 0.0f),
				Vector2(-thickness * 0.5f, lineLength),
				Vector2(thickness * 0.5f, lineLength),

				Vector2(headHeight * 0.5f, lineLength),
				Vector2(-headHeight * 0.5f, lineLength),
				Vector2(0.0f, length),
			};

			renderMeshCommand = DefaultAllocator::GetInstance().New<renderer::RenderCommandMesh>(verticesY.data(), nullptr, nullptr, (uint32_t)verticesY.size(), nullptr, 0, finalMatrix, _movingAxis != _pickingIdAxisY && pickingId != _pickingIdAxisY ? _materialInstanceAxisYNormal : _materialInstanceAxisYHightlight, std::numeric_limits<uint32_t>::max(), _pickingIdAxisY);
			viewport.GetRenderQueue()->PushRenderCommand(renderMeshCommand);

			std::array<Vector2, 6> verticesZ = {
				Vector2(squareOffset + -squareSize * 0.5f, squareOffset + squareSize * 0.5f),
				Vector2(squareOffset + -squareSize * 0.5f, squareOffset + -squareSize * 0.5f),
				Vector2(squareOffset +squareSize * 0.5f, squareOffset + squareSize * 0.5f),
				
				Vector2(squareOffset + -squareSize * 0.5f, squareOffset + -squareSize * 0.5f),
				Vector2(squareOffset + squareSize * 0.5f, squareOffset + -squareSize * 0.5f),
				Vector2(squareOffset + squareSize * 0.5f, squareOffset + squareSize * 0.5f),
			};

			renderMeshCommand = DefaultAllocator::GetInstance().New<renderer::RenderCommandMesh>(verticesZ.data(), nullptr, nullptr, (uint32_t)verticesZ.size(), nullptr, 0, finalMatrix, _movingAxis != _pickingIdAxisZ && pickingId != _pickingIdAxisZ ? _materialInstanceAxisZNormal : _materialInstanceAxisZHightlight, std::numeric_limits<uint32_t>::max(), _pickingIdAxisZ);
			viewport.GetRenderQueue()->PushRenderCommand(renderMeshCommand);

			std::array<Vector2, 6> verticesCenter = {
				Vector2(-thickness * 0.5f, thickness * 0.5f),
				Vector2(-thickness * 0.5f, -thickness * 0.5f),
				Vector2(thickness * 0.5f, thickness * 0.5f),
				
				Vector2(-thickness * 0.5f, -thickness * 0.5f),
				Vector2(thickness * 0.5f, -thickness * 0.5f),
				Vector2(thickness * 0.5f, thickness * 0.5f),
			};

			renderMeshCommand = DefaultAllocator::GetInstance().New<renderer::RenderCommandMesh>(verticesCenter.data(), nullptr, nullptr, (uint32_t)verticesCenter.size(), nullptr, 0, finalMatrix, _materialInstanceCenterNormal, std::numeric_limits<uint32_t>::max());
			viewport.GetRenderQueue()->PushRenderCommand(renderMeshCommand);
		}

		Vector2 size = node->ComputeSize();

		std::array<Vector2, 5> vertices = {
			Vector2(-size.GetX() * 0.5f, size.GetY() * 0.5f),
			Vector2(size.GetX() * 0.5f, size.GetY() * 0.5f),
			Vector2(size.GetX() * 0.5f, -size.GetY() * 0.5f),
			Vector2(-size.GetX() * 0.5f, -size.GetY() * 0.5f),
			Vector2(-size.GetX() * 0.5f, size.GetY() * 0.5f),
		};

		renderer::RenderCommandMesh* renderMeshCommand = DefaultAllocator::GetInstance().New<renderer::RenderCommandMesh>(vertices.data(), nullptr, nullptr, (uint32_t)vertices.size(), nullptr, 0, node->ComputeCanvasMatrix(), _materialInstance, std::numeric_limits<uint32_t>::max() - 1);
		viewport.GetRenderQueue()->PushRenderCommand(renderMeshCommand);

		return changed;
	}

	/// @brief 
	/// @param mousePosition 
	/// @param viewport 
	/// @return 
	Vector2 NodeCustomEditor::GetMouseWorldPos(const Vector2& mousePosition, const ViewportWindow& viewport)
	{
		float ndcX = (2.0f * mousePosition.GetX()) / viewport.GetPickingRenderTarget()->GetWidth() - 1.0f;
		float ndcY = 1.0f - (2.0f * mousePosition.GetY()) / viewport.GetPickingRenderTarget()->GetHeight();
		
		Vector4 mouseNDC = Vector4(ndcX, ndcY, 0.0f, 1.0f);
		Vector4 mouseWorld = Matrix4::Inverse(viewport.GetProjectionMatrix() * viewport.GetViewMatrix()) * mouseNDC;

		return Vector2(mouseWorld.GetX() / mouseWorld.GetW(), mouseWorld.GetY() / mouseWorld.GetW());
	}
}
