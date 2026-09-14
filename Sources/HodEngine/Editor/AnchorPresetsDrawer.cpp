#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/AnchorPresetsDrawer.hpp"

#include <HodEngine/Math/Vector2.hpp>

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>

namespace hod::inline editor
{
	namespace
	{
		void AddTextVertical(ImDrawList* DrawList, const char* text, ImVec2 pos, ImU32 text_color)
		{
			pos.x = std::round(pos.x);
			pos.y = std::round(pos.y);
			ImFont*            font = ImGui::GetFont();
			const ImFontGlyph* glyph;
			char               c;
			while ((c = *text++))
			{
				glyph = font->GetFontBaked(ImGui::GetStyle().FontSizeBase)->FindGlyph(c);
				if (!glyph)
				{
					continue;
				}

				DrawList->PrimReserve(6, 4);
				DrawList->PrimQuadUV(pos + ImVec2(glyph->Y0, -glyph->X0), pos + ImVec2(glyph->Y0, -glyph->X1), pos + ImVec2(glyph->Y1, -glyph->X1), pos + ImVec2(glyph->Y1, -glyph->X0),

				                     ImVec2(glyph->U0, glyph->V0), ImVec2(glyph->U1, glyph->V0), ImVec2(glyph->U1, glyph->V1), ImVec2(glyph->U0, glyph->V1),

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

		void DrawAchorPresetParentBounds(ImDrawList* drawList, ImVec2 pos, ImVec2 Size, uint8_t coloredEdges)
		{
			ImU32 normalColor = IM_COL32(100, 100, 100, 255);
			ImU32 highlightColor = IM_COL32(255, 100, 100, 255);

			float thickness = 1.0f;

			ImVec2 topLeft = pos;
			ImVec2 topRight = pos + ImVec2(Size.x, 0.0f);
			ImVec2 bottomLeft = pos + ImVec2(0, Size.y);
			ImVec2 bottomRight = pos + ImVec2(Size.x, Size.y);

			drawList->AddLine(topLeft, topRight, coloredEdges & Edge::Top ? highlightColor : normalColor, thickness);
			drawList->AddLine(topLeft, bottomLeft, coloredEdges & Edge::Left ? highlightColor : normalColor, thickness);
			drawList->AddLine(bottomLeft, bottomRight, coloredEdges & Edge::Bottom ? highlightColor : normalColor, thickness);
			drawList->AddLine(bottomRight, topRight, coloredEdges & Edge::Right ? highlightColor : normalColor, thickness);

			if (coloredEdges & Edge::CenterH)
			{
				drawList->AddLine(pos + ImVec2(0, Size.y * 0.5f), pos + ImVec2(Size.x, Size.y * 0.5f), highlightColor, thickness);
			}

			if (coloredEdges & Edge::CenterV)
			{
				drawList->AddLine(pos + ImVec2(Size.x * 0.5f, 0.0f), pos + ImVec2(Size.x * 0.5f, Size.y), highlightColor, thickness);
			}
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

		void DrawAchorPresetParentAnchor(ImDrawList* drawList, ImVec2 pos, ImVec2 Size, uint16_t coloredAnchors)
		{
			ImU32 color = IM_COL32(200, 200, 0, 255);

			float radius = 3.0f;

			if (coloredAnchors & Anchor::TopLeft)
			{
				drawList->AddCircleFilled(pos + ImVec2(Size.x * 0.0f, 0.0f), radius, color);
			}
			if (coloredAnchors & Anchor::TopCenter)
			{
				drawList->AddCircleFilled(pos + ImVec2(Size.x * 0.5f, 0.0f), radius, color);
			}
			if (coloredAnchors & Anchor::TopRight)
			{
				drawList->AddCircleFilled(pos + ImVec2(Size.x * 1.0f, 0.0f), radius, color);
			}

			if (coloredAnchors & Anchor::MiddleLeft)
			{
				drawList->AddCircleFilled(pos + ImVec2(Size.x * 0.0f, Size.y * 0.5f), radius, color);
			}
			if (coloredAnchors & Anchor::MiddleCenter)
			{
				drawList->AddCircleFilled(pos + ImVec2(Size.x * 0.5f, Size.y * 0.5f), radius, color);
			}
			if (coloredAnchors & Anchor::MiddleRight)
			{
				drawList->AddCircleFilled(pos + ImVec2(Size.x * 1.0f, Size.y * 0.5f), radius, color);
			}

			if (coloredAnchors & Anchor::BottomLeft)
			{
				drawList->AddCircleFilled(pos + ImVec2(Size.x * 0.0f, Size.y), radius, color);
			}
			if (coloredAnchors & Anchor::BottomCenter)
			{
				drawList->AddCircleFilled(pos + ImVec2(Size.x * 0.5f, Size.y), radius, color);
			}
			if (coloredAnchors & Anchor::BottomRight)
			{
				drawList->AddCircleFilled(pos + ImVec2(Size.x * 1.0f, Size.y), radius, color);
			}
		}

		enum ExpandArrow : uint8_t
		{
			Horizontal = (1 << 0),
			Vertical = (1 << 1),
		};

		void DrawAchorPresetChildBounds(ImDrawList* drawList, ImVec2 pos, ImVec2 Size, uint8_t expandArrows)
		{
			ImU32 boundsColor = IM_COL32(200, 200, 200, 255);
			ImU32 expandArrowColor = IM_COL32(100, 200, 255, 255);

			float thickness = 1.0f;

			drawList->AddRect(pos, pos + Size, boundsColor, 0.0f, thickness, 0);

			float arrowWidth = 8.0f;
			float arrowHeight = 4.0f;

			if (expandArrows & Horizontal)
			{
				ImVec2 leftArrow = pos + ImVec2(0.0f, Size.y * 0.5f);
				ImVec2 rightArrow = pos + ImVec2(Size.x, Size.y * 0.5f);
				drawList->AddLine(leftArrow, rightArrow, expandArrowColor, thickness);
				drawList->AddTriangleFilled(ImFloor(leftArrow), ImFloor(leftArrow + ImVec2(arrowHeight, -arrowWidth * 0.5f)),
				                            ImFloor(leftArrow + ImVec2(arrowHeight, arrowWidth * 0.5f)), expandArrowColor);
				drawList->AddTriangleFilled(rightArrow, rightArrow + ImVec2(-arrowHeight, -arrowWidth * 0.5f), rightArrow + ImVec2(-arrowHeight, arrowWidth * 0.5f), expandArrowColor);
			}

			if (expandArrows & Vertical)
			{
				ImVec2 topArrow = pos + ImVec2(Size.x * 0.5f, 0.0f);
				ImVec2 bottomArrow = pos + ImVec2(Size.x * 0.5f, Size.y);
				drawList->AddLine(topArrow, bottomArrow, expandArrowColor, thickness);
				drawList->AddTriangleFilled(topArrow, topArrow + ImVec2(-arrowWidth * 0.5f, arrowHeight), topArrow + ImVec2(arrowWidth * 0.5f, arrowHeight), expandArrowColor);
				drawList->AddTriangleFilled(ImFloor(bottomArrow), ImFloor(bottomArrow + ImVec2(-arrowWidth * 0.5f, -arrowHeight)),
				                            ImFloor(bottomArrow + ImVec2(arrowWidth * 0.5f, -arrowHeight)), expandArrowColor);
			}
		}

		bool DrawPresetButton(const char* id, const ImVec2& Size, uint8_t edge, uint16_t anchors, uint8_t expandArrows, Vector2& anchorMin, Vector2& anchorMax, Vector2& pivot,
		                      Vector2 presetAnchorMin, Vector2 presetAnchorMax)
		{
			(void)pivot; // TODO

			ImVec2 childSize = Size * 0.5f;
			ImVec2 pos = ImGui::GetCursorScreenPos();

			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
			if (anchorMin != presetAnchorMin || anchorMax != presetAnchorMax)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			}
			bool clicked = ImGui::Button(id, Size);
			if (anchorMin != presetAnchorMin || anchorMax != presetAnchorMax)
			{
				ImGui::PopStyleColor();
			}
			ImGui::PopStyleVar(2);

			DrawAchorPresetParentBounds(ImGui::GetWindowDrawList(), pos, Size, edge);
			DrawAchorPresetParentAnchor(ImGui::GetWindowDrawList(), pos, Size, anchors);
			DrawAchorPresetChildBounds(ImGui::GetWindowDrawList(), pos + childSize * 0.5f, childSize, expandArrows);

			if (clicked)
			{
				anchorMin = presetAnchorMin;
				anchorMax = presetAnchorMax;
				// pivot = Vector2(0.5f, 0.5f);
			}
			return clicked;
		}
	}

	/// @brief
	/// @param Size
	/// @param anchorMin
	/// @param anchorMax
	/// @param pivot
	/// @return
	bool AnchorPresetsDrawer::Draw(const ImVec2& Size, Vector2& anchorMin, Vector2& anchorMax, Vector2& pivot)
	{
		bool changed = false;

		uint8_t     edges = 0;
		uint16_t    anchors = 0;
		uint8_t     expandArrows = 0;
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
		{
			anchors = Anchor::TopLeft;
		}
		else if ((edges & (Edge::Top | Edge::CenterV)) == (Edge::Top | Edge::CenterV))
		{
			anchors = Anchor::TopCenter;
		}
		else if ((edges & (Edge::Top | Edge::Right)) == (Edge::Top | Edge::Right))
		{
			anchors = Anchor::TopRight;
		}
		else if ((edges & (Edge::CenterH | Edge::Left)) == (Edge::CenterH | Edge::Left))
		{
			anchors = Anchor::MiddleLeft;
		}
		else if ((edges & (Edge::CenterH | Edge::CenterV)) == (Edge::CenterH | Edge::CenterV))
		{
			anchors = Anchor::MiddleCenter;
		}
		else if ((edges & (Edge::CenterH | Edge::Right)) == (Edge::CenterH | Edge::Right))
		{
			anchors = Anchor::MiddleRight;
		}
		else if ((edges & (Edge::Bottom | Edge::Left)) == (Edge::Bottom | Edge::Left))
		{
			anchors = Anchor::BottomLeft;
		}
		else if ((edges & (Edge::Bottom | Edge::CenterV)) == (Edge::Bottom | Edge::CenterV))
		{
			anchors = Anchor::BottomCenter;
		}
		else if ((edges & (Edge::Bottom | Edge::Right)) == (Edge::Bottom | Edge::Right))
		{
			anchors = Anchor::BottomRight;
		}
		else if ((edges & Edge::Top) && expandArrows & ExpandArrow::Horizontal)
		{
			anchors = Anchor::TopLeft | Anchor::TopRight;
		}
		else if ((edges & Edge::CenterH) && expandArrows & ExpandArrow::Horizontal)
		{
			anchors = Anchor::MiddleLeft | Anchor::MiddleRight;
		}
		else if ((edges & Edge::Bottom) && expandArrows & ExpandArrow::Horizontal)
		{
			anchors = Anchor::BottomLeft | Anchor::BottomRight;
		}
		else if ((edges & Edge::Left) && expandArrows & ExpandArrow::Vertical)
		{
			anchors = Anchor::TopLeft | Anchor::BottomLeft;
		}
		else if ((edges & Edge::CenterV) && expandArrows & ExpandArrow::Vertical)
		{
			anchors = Anchor::TopCenter | Anchor::BottomCenter;
		}
		else if ((edges & Edge::Right) && expandArrows & ExpandArrow::Vertical)
		{
			anchors = Anchor::TopRight | Anchor::BottomRight;
		}
		else if ((expandArrows & (ExpandArrow::Horizontal | ExpandArrow::Vertical)) == (ExpandArrow::Horizontal | ExpandArrow::Vertical))
		{
			anchors = Anchor::TopLeft | Anchor::TopRight | Anchor::BottomLeft | Anchor::BottomRight;
		}

		ImVec2 buttonSize(Size.x - ImGui::GetTextLineHeightWithSpacing(), Size.y - ImGui::GetTextLineHeightWithSpacing());

		ImVec2 cursorPos = ImGui::GetCursorPos();
		ImGui::SetCursorPosX(cursorPos.x + ImGui::GetTextLineHeightWithSpacing() + buttonSize.x * 0.5f - ImGui::CalcTextSize(topLabel).x * 0.5f);
		ImGui::TextUnformatted(topLabel);
		ImGui::SetCursorPos(cursorPos);
		AddTextVertical(ImGui::GetWindowDrawList(), leftLabel,
		                ImGui::GetCursorScreenPos() +
		                    ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() + buttonSize.y * 0.5f - ImGui::CalcTextSize(leftLabel).x * 0.5f + ImGui::CalcTextSize(leftLabel).x),
		                ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Text)));
		ImGui::SetCursorPos(cursorPos + ImVec2(ImGui::GetTextLineHeightWithSpacing(), ImGui::GetTextLineHeightWithSpacing()));

		ImVec2 presetSize = ImVec2(buttonSize.x - 24, buttonSize.y - 24);
		ImVec2 childSize = presetSize * 0.5f;

		ImVec2 presetButtonScreenPosition = ImGui::GetCursorScreenPos();
		bool   clicked = ImGui::Button("##AnchorPresets", buttonSize);
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

			changed |= DrawPresetButton("##AnchorPresetTopLeft", presetSize, Edge::Top | Edge::Left, Anchor::TopLeft, 0, anchorMin, anchorMax, pivot, Vector2(0.0f, 1.0f),
			                            Vector2(0.0f, 1.0f));
			ImGui::SameLine();
			changed |= DrawPresetButton("##AnchorPresetTopCenter", presetSize, Edge::Top | Edge::CenterV, Anchor::TopCenter, 0, anchorMin, anchorMax, pivot, Vector2(0.5f, 1.0f),
			                            Vector2(0.5f, 1.0f));
			ImGui::SameLine();
			changed |= DrawPresetButton("##AnchorPresetTopRight", presetSize, Edge::Top | Edge::Right, Anchor::TopRight, 0, anchorMin, anchorMax, pivot, Vector2(1.0f, 1.0f),
			                            Vector2(1.0f, 1.0f));
			ImGui::SameLine(0.0f, 25.0f);
			changed |= DrawPresetButton("##AnchorPresetTopStretch", presetSize, Edge::Top, Anchor::TopLeft | Anchor::TopRight, ExpandArrow::Horizontal, anchorMin, anchorMax, pivot,
			                            Vector2(0.0f, 1.0f), Vector2(1.0f, 1.0f));

			changed |= DrawPresetButton("##AnchorPresetMiddleLeft", presetSize, Edge::CenterH | Edge::Left, Anchor::MiddleLeft, 0, anchorMin, anchorMax, pivot, Vector2(0.0f, 0.5f),
			                            Vector2(0.0f, 0.5f));
			ImGui::SameLine();
			changed |= DrawPresetButton("##AnchorPresetMiddleCenter", presetSize, Edge::CenterH | Edge::CenterV, Anchor::MiddleCenter, 0, anchorMin, anchorMax, pivot,
			                            Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f));
			ImGui::SameLine();
			changed |= DrawPresetButton("##AnchorPresetMiddleRight", presetSize, Edge::CenterH | Edge::Right, Anchor::MiddleRight, 0, anchorMin, anchorMax, pivot,
			                            Vector2(1.0f, 0.5f), Vector2(1.0f, 0.5f));
			ImGui::SameLine(0.0f, 25.0f);
			changed |= DrawPresetButton("##AnchorPresetMiddleStretch", presetSize, Edge::CenterH, Anchor::MiddleLeft | Anchor::MiddleRight, ExpandArrow::Horizontal, anchorMin,
			                            anchorMax, pivot, Vector2(0.0f, 0.5f), Vector2(1.0f, 0.5f));

			changed |= DrawPresetButton("##AnchorPresetBottomLeft", presetSize, Edge::Bottom | Edge::Left, Anchor::BottomLeft, 0, anchorMin, anchorMax, pivot, Vector2(0.0f, 0.0f),
			                            Vector2(0.0f, 0.0f));
			ImGui::SameLine();
			changed |= DrawPresetButton("##AnchorPresetBottomCenter", presetSize, Edge::Bottom | Edge::CenterV, Anchor::BottomCenter, 0, anchorMin, anchorMax, pivot,
			                            Vector2(0.5f, 0.0f), Vector2(0.5f, 0.0f));
			ImGui::SameLine();
			changed |= DrawPresetButton("##AnchorPresetBottomRight", presetSize, Edge::Bottom | Edge::Right, Anchor::BottomRight, 0, anchorMin, anchorMax, pivot,
			                            Vector2(1.0f, 0.0f), Vector2(1.0f, 0.0f));
			ImGui::SameLine(0.0f, 25.0f);
			changed |= DrawPresetButton("##AnchorPresetBottomStretch", presetSize, Edge::Bottom, Anchor::BottomLeft | Anchor::BottomRight, ExpandArrow::Horizontal, anchorMin,
			                            anchorMax, pivot, Vector2(0.0f, 0.0f), Vector2(1.0f, 0.0f));

			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Spacing();

			changed |= DrawPresetButton("##AnchorPresetStretchLeft", presetSize, Edge::Left, Anchor::TopLeft | Anchor::BottomLeft, ExpandArrow::Vertical, anchorMin, anchorMax,
			                            pivot, Vector2(0.0f, 0.0f), Vector2(0.0f, 1.0f));
			ImGui::SameLine();
			changed |= DrawPresetButton("##AnchorPresetStretchMiddle", presetSize, Edge::CenterV, Anchor::TopCenter | Anchor::BottomCenter, ExpandArrow::Vertical, anchorMin,
			                            anchorMax, pivot, Vector2(0.5f, 0.0f), Vector2(0.5f, 1.0f));
			ImGui::SameLine();
			changed |= DrawPresetButton("##AnchorPresetStretchRight", presetSize, Edge::Right, Anchor::TopRight | Anchor::BottomRight, ExpandArrow::Vertical, anchorMin, anchorMax,
			                            pivot, Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f));
			ImGui::SameLine(0.0f, 25.0f);
			changed |= DrawPresetButton("##AnchorPresetStretchStretch", presetSize, 0, Anchor::TopLeft | Anchor::TopRight | Anchor::BottomLeft | Anchor::BottomRight,
			                            ExpandArrow::Horizontal | ExpandArrow::Vertical, anchorMin, anchorMax, pivot, Vector2(0.0f, 0.0f), Vector2(1.0f, 1.0f));

			ImGui::EndPopup();
		}

		return changed;
	}
}
