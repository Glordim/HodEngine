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

		// The preset drawings are laid out on whole pixels: an odd extent, so a 1 pixel wide line (AddLineH/AddLineV and AddRect strokes
		// stay inside the pixels they are given, since ImGui 1.93) has as many pixels on each side of the middle one.
		float SnapToOddPixels(float extent)
		{
			float pixels = std::floor(extent);
			return std::fmod(pixels, 2.0f) == 0.0f ? pixels - 1.0f : pixels;
		}

		/// @param pos top-left pixel, whole
		/// @param size odd number of pixels on each axis: the edges are the first and last pixel rows/columns, the center ones the middle
		void DrawAchorPresetParentBounds(ImDrawList* drawList, ImVec2 pos, ImVec2 size, uint8_t coloredEdges)
		{
			ImU32 normalColor = IM_COL32(100, 100, 100, 255);
			ImU32 highlightColor = IM_COL32(255, 100, 100, 255);

			ImVec2 end = pos + size;
			ImVec2 last = end - ImVec2(1.0f, 1.0f);
			ImVec2 middle = pos + (size - ImVec2(1.0f, 1.0f)) * 0.5f;

			drawList->AddRect(pos, end, normalColor);

			// Highlighted ones on top of the normal outline
			if (coloredEdges & Edge::Top)
			{
				drawList->AddLineH(pos.x, end.x, pos.y, highlightColor);
			}
			if (coloredEdges & Edge::Bottom)
			{
				drawList->AddLineH(pos.x, end.x, last.y, highlightColor);
			}
			if (coloredEdges & Edge::Left)
			{
				drawList->AddLineV(pos.x, pos.y, end.y, highlightColor);
			}
			if (coloredEdges & Edge::Right)
			{
				drawList->AddLineV(last.x, pos.y, end.y, highlightColor);
			}
			if (coloredEdges & Edge::CenterH)
			{
				drawList->AddLineH(pos.x, end.x, middle.y, highlightColor);
			}
			if (coloredEdges & Edge::CenterV)
			{
				drawList->AddLineV(middle.x, pos.y, end.y, highlightColor);
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

		/// @brief The anchor dots, centered on the pixels of the matching edges (see DrawAchorPresetParentBounds)
		void DrawAchorPresetParentAnchor(ImDrawList* drawList, ImVec2 pos, ImVec2 size, uint16_t coloredAnchors)
		{
			ImU32 color = IM_COL32(200, 200, 0, 255);
			float radius = 3.0f;

			// Centers of the first, middle and last pixel columns/rows
			ImVec2 first = pos + ImVec2(0.5f, 0.5f);
			ImVec2 middle = pos + size * 0.5f;
			ImVec2 last = pos + size - ImVec2(0.5f, 0.5f);

			const float xs[3] = {first.x, middle.x, last.x};
			const float ys[3] = {first.y, middle.y, last.y};
			// Anchor bits go row by row from the top-left: TopLeft, TopCenter, TopRight, MiddleLeft...
			for (uint32_t index = 0; index < 9; ++index)
			{
				if (coloredAnchors & (1 << index))
				{
					drawList->AddCircleFilled(ImVec2(xs[index % 3], ys[index / 3]), radius, color);
				}
			}
		}

		enum ExpandArrow : uint8_t
		{
			Horizontal = (1 << 0),
			Vertical = (1 << 1),
		};

		/// @brief The child's rect and its expand arrows, on whole pixels as the parent's (see DrawAchorPresetParentBounds): each shaft is the
		/// middle pixel row/column, each arrow head is centered on it and points at the rect's outer edge. Heads in clockwise order (on screen,
		/// Y down) as ImGui expects for a filled shape: otherwise its anti-aliasing fringe goes inward and the head looks thinner.
		void DrawAchorPresetChildBounds(ImDrawList* drawList, ImVec2 pos, ImVec2 size, uint8_t expandArrows)
		{
			ImU32 boundsColor = IM_COL32(200, 200, 200, 255);
			ImU32 expandArrowColor = IM_COL32(100, 200, 255, 255);

			drawList->AddRect(pos, pos + size, boundsColor);

			ImVec2 end = pos + size;
			ImVec2 middle = pos + (size - ImVec2(1.0f, 1.0f)) * 0.5f; // Middle pixel row/column
			ImVec2 center = middle + ImVec2(0.5f, 0.5f);               // Its center
			float  arrowHalfWidth = 4.0f;
			float  arrowLength = 4.0f;

			if (expandArrows & Horizontal)
			{
				drawList->AddLineH(pos.x, end.x, middle.y, expandArrowColor);
				drawList->AddTriangleFilled(ImVec2(pos.x, center.y), ImVec2(pos.x + arrowLength, center.y - arrowHalfWidth), ImVec2(pos.x + arrowLength, center.y + arrowHalfWidth),
				                            expandArrowColor);
				drawList->AddTriangleFilled(ImVec2(end.x, center.y), ImVec2(end.x - arrowLength, center.y + arrowHalfWidth), ImVec2(end.x - arrowLength, center.y - arrowHalfWidth),
				                            expandArrowColor);
			}

			if (expandArrows & Vertical)
			{
				drawList->AddLineV(middle.x, pos.y, end.y, expandArrowColor);
				drawList->AddTriangleFilled(ImVec2(center.x, pos.y), ImVec2(center.x + arrowHalfWidth, pos.y + arrowLength), ImVec2(center.x - arrowHalfWidth, pos.y + arrowLength),
				                            expandArrowColor);
				drawList->AddTriangleFilled(ImVec2(center.x, end.y), ImVec2(center.x - arrowHalfWidth, end.y - arrowLength), ImVec2(center.x + arrowHalfWidth, end.y - arrowLength),
				                            expandArrowColor);
			}
		}

		/// @brief A whole preset drawing in the given area: the parent's bounds and anchors, the child's bounds and arrows in its middle.
		/// Snapped to whole pixels (see SnapToOddPixels), so every preset is drawn the same wherever it lands.
		void DrawAnchorPreset(ImDrawList* drawList, ImVec2 pos, ImVec2 size, uint8_t edges, uint16_t anchors, uint8_t expandArrows)
		{
			pos = ImFloor(pos);
			ImVec2 parentSize(SnapToOddPixels(size.x), SnapToOddPixels(size.y));
			ImVec2 childSize(SnapToOddPixels(parentSize.x * 0.5f), SnapToOddPixels(parentSize.y * 0.5f));
			ImVec2 childPos = pos + (parentSize - childSize) * 0.5f; // Whole: both sizes are odd

			DrawAchorPresetParentBounds(drawList, pos, parentSize, edges);
			DrawAchorPresetParentAnchor(drawList, pos, parentSize, anchors);
			DrawAchorPresetChildBounds(drawList, childPos, childSize, expandArrows);
		}

		bool DrawPresetButton(const char* id, const ImVec2& Size, uint8_t edge, uint16_t anchors, uint8_t expandArrows, Vector2& anchorMin, Vector2& anchorMax, Vector2& pivot,
		                      Vector2 presetAnchorMin, Vector2 presetAnchorMax)
		{
			(void)pivot; // TODO

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

			DrawAnchorPreset(ImGui::GetWindowDrawList(), pos, Size, edge, anchors, expandArrows);

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

		ImVec2 presetButtonScreenPosition = ImGui::GetCursorScreenPos();
		bool   clicked = ImGui::Button("##AnchorPresets", buttonSize);
		ImVec2 pos = presetButtonScreenPosition + ImVec2(12, 12);
		DrawAnchorPreset(ImGui::GetWindowDrawList(), pos, presetSize, edges, anchors, expandArrows);

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
