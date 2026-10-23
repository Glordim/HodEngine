#include "HodEngine/ImGui/Pch.hpp"
#include "Helper.hpp"
#include "Widgets.hpp"

namespace hod::inline imgui
{
	void RenderFrame(ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, bool border, float rounding, ImDrawFlags drawFlags)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow*  window = g.CurrentWindow;
		window->DrawList->AddRectFilled(p_min, p_max, fill_col, rounding, drawFlags);
		const float border_size = g.Style.FrameBorderSize;
		if (border && border_size > 0.0f)
		{
			window->DrawList->AddRect(p_min + ImVec2(1, 1), p_max + ImVec2(1, 1), ImGui::GetColorU32(ImGuiCol_BorderShadow), rounding, border_size, drawFlags);
			window->DrawList->AddRect(p_min, p_max, ImGui::GetColorU32(ImGuiCol_Border), rounding, border_size, drawFlags);
		}
	}

	void FramedText(const char* label, ImDrawFlags drawFlags)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
		{
			return;
		}

		ImGuiContext&     g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImVec2      label_size = ImGui::CalcTextSize(label, NULL, true);

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 Size = ImGui::CalcItemSize(ImVec2(0, 0), label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

		const ImRect bb(pos, pos + Size);
		ImGui::ItemSize(Size, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, 0))
		{
			return;
		}

		const ImU32 frame_col = ImGui::GetColorU32(ImGuiCol_FrameBg);
		RenderFrame(bb.Min, bb.Max, frame_col, true, style.FrameRounding, drawFlags);

		if (label_size.x > 0.0f)
		{
			ImGui::RenderText(ImVec2(bb.Min.x + style.ItemInnerSpacing.x, bb.Min.y + style.FramePadding.y), label);
		}
	}

	bool FramedButton(const char* label, bool active, ImDrawFlags drawFlags)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
		{
			return false;
		}

		ImGuiContext&     g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID     id = window->GetID(label);
		const ImVec2      label_size = ImGui::CalcTextSize(label, NULL, true);

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 Size = ImGui::CalcItemSize(ImVec2(0, 0), label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

		const ImRect bb(pos, pos + Size);
		ImGui::ItemSize(Size, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id))
		{
			return false;
		}

		bool hovered = false;
		bool held = false;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

		const ImU32 col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : active ? ImGuiCol_ButtonActive : ImGuiCol_Button);
		ImGui::RenderNavCursor(bb, id);
		RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding, drawFlags);
		ImGui::RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);

		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
		return pressed;
	}

	bool LinkText(const char* label)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
		{
			return false;
		}

		ImGuiContext&     g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID     id = window->GetID(label);
		const ImVec2      label_size = ImGui::CalcTextSize(label, NULL, false);

		// Same height as a button so it lines up with the widgets around it, but only a small horizontal margin for the highlight
		const ImVec2 padding(2.0f, style.FramePadding.y);
		ImVec2       pos = window->DC.CursorPos;
		ImVec2       size = label_size + padding * 2.0f;

		const ImRect bb(pos, pos + size);
		ImGui::ItemSize(size, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id))
		{
			return false;
		}

		bool hovered = false;
		bool held = false;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);
		if (hovered)
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		}

		ImGui::RenderNavCursor(bb, id);
		if (held && hovered)
		{
			window->DrawList->AddRectFilled(bb.Min, bb.Max, ImGui::GetColorU32(ImGuiCol_HeaderActive), style.FrameRounding);
		}

		const ImU32  textColor = ImGui::GetColorU32(ImGuiCol_Text);
		const ImVec2 textPos = bb.Min + padding;
		window->DrawList->AddText(textPos, textColor, label);
		if (hovered)
		{
			float lineY = textPos.y + label_size.y;
			window->DrawList->AddLine(ImVec2(textPos.x, lineY), ImVec2(textPos.x + label_size.x, lineY), textColor);
		}

		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
		return pressed;
	}

	// Note: p_data, p_min and p_max are _pointers_ to a memory address holding the data. For a Drag widget, p_min and p_max are optional.
	// Read code of e.g. DragFloat(), DragInt() etc. or examples in 'Demo->Widgets->Data Types' to understand how to use this function directly.
	bool DragScalar(const char* label, ImGuiDataType data_type, void* p_data, float v_speed, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags,
	                ImDrawFlags drawFlags)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
		{
			return false;
		}

		ImGuiContext&     g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID     id = window->GetID(label);
		const float       w = ImGui::CalcItemWidth();

		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));
		const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

		const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;
		ImGui::ItemSize(total_bb, style.FramePadding.y);
		if (!ImGui::ItemAdd(total_bb, id, &frame_bb, temp_input_allowed ? ImGuiItemFlags_Inputable : 0))
		{
			return false;
		}

		// Default format string when passing NULL
		if (format == NULL)
		{
			format = ImGui::DataTypeGetInfo(data_type)->PrintFmt;
		}

		const bool hovered = ImGui::ItemHoverable(frame_bb, id, g.LastItemData.ItemFlags);
		bool       temp_input_is_active = temp_input_allowed && ImGui::TempInputIsActive(id);
		if (!temp_input_is_active)
		{
			// Tabbing or CTRL-clicking on Drag turns it into an InputText
			const bool clicked = hovered && ImGui::IsMouseClicked(0, ImGuiInputFlags_None, id);
			const bool double_clicked = (hovered && g.IO.MouseClickedCount[0] == 2 && ImGui::TestKeyOwner(ImGuiKey_MouseLeft, id));
			const bool make_active = (clicked || double_clicked || g.NavActivateId == id);
			if (make_active && (clicked || double_clicked))
			{
				ImGui::SetKeyOwner(ImGuiKey_MouseLeft, id);
			}
			if (make_active && temp_input_allowed)
			{
				if ((clicked && g.IO.KeyCtrl) || double_clicked || (g.NavActivateId == id && (g.NavActivateFlags & ImGuiActivateFlags_PreferInput)))
				{
					temp_input_is_active = true;
				}
			}

			static const float DRAG_MOUSE_THRESHOLD_FACTOR = 0.50f;

			// (Optional) simple click (without moving) turns Drag into an InputText
			if (g.IO.ConfigDragClickToInputText && temp_input_allowed && !temp_input_is_active)
			{
				if (g.ActiveId == id && hovered && g.IO.MouseReleased[0] && !ImGui::IsMouseDragPastThreshold(0, g.IO.MouseDragThreshold * DRAG_MOUSE_THRESHOLD_FACTOR))
				{
					g.NavActivateId = id;
					g.NavActivateFlags = ImGuiActivateFlags_PreferInput;
					temp_input_is_active = true;
				}
			}

			if (make_active && !temp_input_is_active)
			{
				ImGui::SetActiveID(id, window);
				ImGui::SetFocusID(id, window);
				ImGui::FocusWindow(window);
				g.ActiveIdUsingNavDirMask = (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
			}
		}

		if (temp_input_is_active)
		{
			// Only clamp CTRL+Click input when ImGuiSliderFlags_AlwaysClamp is set
			const bool is_clamp_input = (flags & ImGuiSliderFlags_AlwaysClamp) != 0 && (p_min == NULL || p_max == NULL || ImGui::DataTypeCompare(data_type, p_min, p_max) < 0);
			return ImGui::TempInputScalar(frame_bb, id, label, data_type, p_data, format, is_clamp_input ? p_min : NULL, is_clamp_input ? p_max : NULL);
		}

		// Draw frame
		const ImU32 frame_col = ImGui::GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
		ImGui::RenderNavCursor(frame_bb, id);
		RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, style.FrameRounding, drawFlags);

		// Drag behavior
		const bool value_changed = ImGui::DragBehavior(id, data_type, p_data, v_speed, p_min, p_max, format, flags);
		if (value_changed)
		{
			ImGui::MarkItemEdited(id);
		}

		// Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
		char        value_buf[64];
		const char* value_buf_end = value_buf + ImGui::DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);
		if (g.LogEnabled)
		{
			ImGui::LogSetNextTextDecoration("{", "}");
		}
		ImGui::RenderTextClipped(frame_bb.Min, frame_bb.Max, value_buf, value_buf_end, NULL, ImVec2(0.5f, 0.5f));

		if (label_size.x > 0.0f)
		{
			ImGui::RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);
		}

		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | (temp_input_allowed ? ImGuiItemStatusFlags_Inputable : 0));
		return value_changed;
	}

	namespace
	{
		constexpr int RecentColorCount = 10;

		/// @brief Only one color popup can be open at a time, so its state is shared
		struct ColorFieldState
		{
			ImGuiID popupId = 0;
			float   original[4] = {};
			bool    displayFloat = false;
			ImVec4  recents[RecentColorCount];
			int     recentCount = 0;
		};

		ColorFieldState gColorFieldState;

		void PushRecentColor(const float* col, int componentCount)
		{
			ImVec4 color(col[0], col[1], col[2], componentCount == 4 ? col[3] : 1.0f);

			int index = 0;
			while (index < gColorFieldState.recentCount && memcmp(&gColorFieldState.recents[index], &color, sizeof(ImVec4)) != 0)
			{
				++index;
			}
			if (index == gColorFieldState.recentCount)
			{
				index = ImMin(gColorFieldState.recentCount, RecentColorCount - 1);
				gColorFieldState.recentCount = ImMin(gColorFieldState.recentCount + 1, RecentColorCount);
			}
			memmove(&gColorFieldState.recents[1], &gColorFieldState.recents[0], index * sizeof(ImVec4));
			gColorFieldState.recents[0] = color;
		}

		bool ParseHexColor(const char* text, float* col, int componentCount)
		{
			if (text == nullptr)
			{
				return false;
			}
			while (*text == ' ' || *text == '#')
			{
				++text;
			}

			// "RRGGBB" keeps the current alpha, "RRGGBBAA" overrides it
			unsigned int rgba[4] = {};
			const int    parsedCount = sscanf(text, "%02X%02X%02X%02X", &rgba[0], &rgba[1], &rgba[2], &rgba[3]);
			if (parsedCount < 3)
			{
				return false;
			}
			for (int index = 0; index < ImMin(parsedCount, componentCount); ++index)
			{
				col[index] = rgba[index] / 255.0f;
			}
			return true;
		}

		void FormatHexColor(char* buffer, size_t bufferSize, const float* col, bool hasAlpha)
		{
			if (hasAlpha)
			{
				ImFormatString(buffer, bufferSize, "#%02X%02X%02X%02X", IM_F32_TO_INT8_SAT(col[0]), IM_F32_TO_INT8_SAT(col[1]), IM_F32_TO_INT8_SAT(col[2]), IM_F32_TO_INT8_SAT(col[3]));
			}
			else
			{
				ImFormatString(buffer, bufferSize, "#%02X%02X%02X", IM_F32_TO_INT8_SAT(col[0]), IM_F32_TO_INT8_SAT(col[1]), IM_F32_TO_INT8_SAT(col[2]));
			}
		}

		/// @return true when the clipboard holds a hex color (and col was updated)
		bool PasteHexColor(float* col, int componentCount)
		{
			float pasted[4];
			memcpy(pasted, col, componentCount * sizeof(float));
			if (ParseHexColor(ImGui::GetClipboardText(), pasted, componentCount) == false || memcmp(pasted, col, componentCount * sizeof(float)) == 0)
			{
				return false;
			}
			memcpy(col, pasted, componentCount * sizeof(float));
			return true;
		}
	}

	bool ColorField(const char* label, float* col, ImGuiColorEditFlags flags)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
		{
			return false;
		}

		ImGuiContext&     g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID     id = window->GetID(label);
		const bool        hasAlpha = (flags & ImGuiColorEditFlags_NoAlpha) == 0;
		const int         componentCount = hasAlpha ? 4 : 3;

		char hex[16];
		FormatHexColor(hex, sizeof(hex), col, hasAlpha);

		// Hex input on the right of the swatch, dropped when there is not enough room
		const float totalWidth = ImGui::CalcItemWidth();
		const float hexWidth = ImGui::CalcTextSize(hasAlpha ? "#DDDDDDDD" : "#DDDDDD").x + style.FramePadding.x * 3.0f;
		const bool  showHex = (flags & ImGuiColorEditFlags_NoInputs) == 0 && totalWidth >= hexWidth * 2.0f;
		const float swatchWidth = showHex ? totalWidth - hexWidth - style.ItemInnerSpacing.x : totalWidth;

		const ImVec2 pos = window->DC.CursorPos;
		const ImRect bb(pos, pos + ImVec2(swatchWidth, ImGui::GetFrameHeight()));
		ImGui::ItemSize(bb, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id))
		{
			return false;
		}

		bool       hovered = false;
		bool       held = false;
		const bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);
		bool       changed = false;

		// Swatch: opaque color on top, alpha as a white (alpha) / black (1 - alpha) strip at the bottom
		ImDrawList* drawList = window->DrawList;
		const float rounding = style.FrameRounding;
		const ImU32 opaqueColor = ImGui::ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 1.0f));
		if (hasAlpha)
		{
			const float splitY = bb.Max.y - ImMax(2.0f, IM_TRUNC(bb.GetHeight() * 0.2f));
			const float alphaX = IM_TRUNC(ImLerp(bb.Min.x, bb.Max.x, ImSaturate(col[3])));
			drawList->AddRectFilled(bb.Min, ImVec2(bb.Max.x, splitY), opaqueColor, rounding, ImDrawFlags_RoundTop);
			drawList->AddRectFilled(ImVec2(bb.Min.x, splitY), bb.Max, IM_COL32_BLACK, rounding, ImDrawFlags_RoundBottom);
			drawList->PushClipRect(ImVec2(bb.Min.x, splitY), ImVec2(alphaX, bb.Max.y), true);
			drawList->AddRectFilled(ImVec2(bb.Min.x, splitY), bb.Max, IM_COL32_WHITE, rounding, ImDrawFlags_RoundBottom);
			drawList->PopClipRect();
		}
		else
		{
			drawList->AddRectFilled(bb.Min, bb.Max, opaqueColor, rounding);
		}
		const ImU32 borderColor = ImGui::GetColorU32(held ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Border);
		drawList->AddRect(bb.Min, bb.Max, borderColor, rounding, ImMax(1.0f, style.FrameBorderSize));
		ImGui::RenderNavCursor(bb, id);

		ImGui::PushID(label);
		const ImGuiID popupId = window->GetID("##ColorFieldPopup");

		if ((flags & ImGuiColorEditFlags_NoDragDrop) == 0)
		{
			if (g.ActiveId == id && ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload(hasAlpha ? IMGUI_PAYLOAD_TYPE_COLOR_4F : IMGUI_PAYLOAD_TYPE_COLOR_3F, col, componentCount * sizeof(float), ImGuiCond_Once);
				ImGui::ColorButton("##preview", ImVec4(col[0], col[1], col[2], hasAlpha ? col[3] : 1.0f), flags & ImGuiColorEditFlags_AlphaMask_);
				ImGui::SameLine();
				ImGui::TextUnformatted("Color");
				ImGui::EndDragDropSource();
			}
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F))
				{
					memcpy(col, payload->Data, 3 * sizeof(float));
					changed = true;
				}
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
				{
					memcpy(col, payload->Data, componentCount * sizeof(float));
					changed = true;
				}
				ImGui::EndDragDropTarget();
			}
		}

		if (gColorFieldState.popupId != popupId && hovered && !held && (flags & ImGuiColorEditFlags_NoTooltip) == 0 && ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
		{
			ImGui::ColorTooltip(label, col, flags & (ImGuiColorEditFlags_AlphaMask_ | ImGuiColorEditFlags_HDR));
		}

		if ((flags & ImGuiColorEditFlags_NoOptions) == 0)
		{
			if (hovered && g.ActiveId == 0)
			{
				if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_C))
				{
					ImGui::SetClipboardText(hex);
				}
				if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_V))
				{
					changed |= PasteHexColor(col, componentCount);
				}
			}

			if (ImGui::BeginPopupContextItem("##ColorFieldContext"))
			{
				if (ImGui::MenuItem("Copy hex", "Ctrl+C"))
				{
					ImGui::SetClipboardText(hex);
				}
				float      pasted[4];
				const bool canPaste = ParseHexColor(ImGui::GetClipboardText(), pasted, componentCount);
				if (ImGui::MenuItem("Paste hex", "Ctrl+V", false, canPaste))
				{
					changed |= PasteHexColor(col, componentCount);
				}
				ImGui::EndPopup();
			}
		}

		if (showHex)
		{
			ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
			ImGui::SetNextItemWidth(hexWidth);
			ImGui::InputText("##hex", hex, sizeof(hex), ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_AutoSelectAll);
			// Applied on Enter / focus loss only, so partial input does not make the color jump while typing
			if (ImGui::IsItemDeactivatedAfterEdit())
			{
				float parsed[4];
				memcpy(parsed, col, componentCount * sizeof(float));
				if (ParseHexColor(hex, parsed, componentCount) && memcmp(parsed, col, componentCount * sizeof(float)) != 0)
				{
					memcpy(col, parsed, componentCount * sizeof(float));
					changed = true;
				}
			}
		}

		// Popup closed since last frame (click outside, Escape, ...)
		if (gColorFieldState.popupId == popupId && !ImGui::IsPopupOpen(popupId, ImGuiPopupFlags_None))
		{
			gColorFieldState.popupId = 0;
			if (ImGui::IsKeyPressed(ImGuiKey_Escape, false))
			{
				changed |= memcmp(col, gColorFieldState.original, componentCount * sizeof(float)) != 0;
				memcpy(col, gColorFieldState.original, componentCount * sizeof(float));
			}
			else if (memcmp(col, gColorFieldState.original, componentCount * sizeof(float)) != 0)
			{
				PushRecentColor(col, componentCount);
			}
		}

		if (pressed)
		{
			gColorFieldState.popupId = popupId;
			memcpy(gColorFieldState.original, col, componentCount * sizeof(float));
			ImGui::OpenPopupEx(popupId);
			ImGui::SetNextWindowPos(ImVec2(bb.Min.x, bb.Max.y + style.ItemSpacing.y));
		}

		if (ImGui::BeginPopupEx(popupId, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings))
		{
			// Escape reverts (when not consumed by an active input), whether or not keyboard navigation would close the popup itself
			if (ImGui::IsKeyPressed(ImGuiKey_Escape, false) && g.ActiveId == 0 && g.ActiveIdPreviousFrame == 0)
			{
				changed |= memcmp(col, gColorFieldState.original, componentCount * sizeof(float)) != 0;
				memcpy(col, gColorFieldState.original, componentCount * sizeof(float));
				gColorFieldState.popupId = 0;
				ImGui::CloseCurrentPopup();
			}

			if (FramedButton("0-1", gColorFieldState.displayFloat, ImDrawFlags_RoundLeft))
			{
				gColorFieldState.displayFloat = true;
			}
			ImGui::SameLine(0.0f, 0.0f);
			if (FramedButton("0-255", !gColorFieldState.displayFloat, ImDrawFlags_RoundRight))
			{
				gColorFieldState.displayFloat = false;
			}

			ImGuiColorEditFlags pickerFlags = (flags & (ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_HDR)) | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_DisplayRGB |
			                                  ImGuiColorEditFlags_DisplayHSV | ImGuiColorEditFlags_DisplayHex;
			if (gColorFieldState.displayFloat)
			{
				pickerFlags |= ImGuiColorEditFlags_Float;
			}
			ImGui::SetNextItemWidth(ImGui::GetFrameHeight() * 12.0f);
			changed |= ImGui::ColorPicker4("##picker", col, pickerFlags, gColorFieldState.original);

			if (gColorFieldState.recentCount > 0)
			{
				ImGui::Separator();
				ImGui::TextUnformatted("Recent");
				const float swatchSize = ImGui::GetFrameHeight();
				for (int index = 0; index < gColorFieldState.recentCount; ++index)
				{
					if (index > 0)
					{
						ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
					}
					ImGui::PushID(index);
					if (ImGui::ColorButton("##recent", gColorFieldState.recents[index], flags & ImGuiColorEditFlags_NoAlpha, ImVec2(swatchSize, swatchSize)))
					{
						memcpy(col, &gColorFieldState.recents[index], componentCount * sizeof(float));
						changed = true;
					}
					ImGui::PopID();
				}
			}

			ImGui::EndPopup();
		}

		ImGui::PopID();
		return changed;
	}

	void PreserveAspectImage(ImTextureID image, ImVec2 imageSize, ImVec2 resizedSize)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
		{
			return;
		}

		ImGuiContext&     g = *GImGui;
		const ImGuiStyle& style = g.Style;
		ImVec2            pos = window->DC.CursorPos;
		const ImGuiID     id = window->GetID(image);

		ImVec2 imageFinalSize = resizedSize - style.FramePadding * 2;
		float  widthFactor = resizedSize.x / imageSize.x;
		float  heightFactor = resizedSize.y / imageSize.y;
		if (widthFactor < heightFactor)
		{
			imageFinalSize.y = imageSize.y * widthFactor;
		}
		else
		{
			imageFinalSize.x = imageSize.x * heightFactor;
		}

		const ImRect bb(pos, pos + resizedSize);
		ImGui::ItemSize(resizedSize, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id))
		{
			return;
		}

		ImGui::RenderFrameBorder(bb.Min, bb.Max, style.FrameRounding);
		ImVec2 imageOffset = (resizedSize - imageFinalSize) * 0.5f;
		window->DrawList->AddImage(image, bb.Min + style.FramePadding + imageOffset, bb.Max - style.FramePadding - imageOffset);
	}

	bool ImageTextButton(ImTextureID image, ImVec2 imageSize, ImVec2 resizedSize, const char* label, ImVec2 size_arg, ImGuiButtonFlags flags, ImDrawFlags drawFlags)
	{
		(void)drawFlags; // TODO

		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
		{
			return false;
		}

		ImGuiContext&     g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID     id = window->GetID(label);
		const ImVec2      label_size = ImGui::CalcTextSize(label, NULL, true);
		const float       finalHeight = ImMax(label_size.y, resizedSize.y) + style.FramePadding.y * 2.0f;
		const float       spaceBetweenImageAndText = finalHeight * 0.25f;

		ImVec2 pos = window->DC.CursorPos;
		if ((flags & ImGuiButtonFlags_AlignTextBaseLine) &&
		    style.FramePadding.y < window->DC.CurrLineTextBaseOffset) // Try to vertically align buttons that are smaller/have no padding so that text baseline matches (bit hacky,
		                                                              // since it shouldn't be a flag)
		{
			pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
		}
		ImVec2 Size = ImGui::CalcItemSize(size_arg, resizedSize.x + spaceBetweenImageAndText + label_size.x + style.FramePadding.x * 2.0f, finalHeight);

		const ImRect bb(pos, pos + Size);
		ImGui::ItemSize(Size, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id))
		{
			return false;
		}

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, flags);

		// Render
		const ImU32 col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
		ImGui::RenderNavCursor(bb, id);
		ImGui::RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);

		ImVec2 imageFinalSize = resizedSize;
		float  widthFactor = resizedSize.x / imageSize.x;
		float  heightFactor = resizedSize.y / imageSize.y;
		if (widthFactor < heightFactor)
		{
			imageFinalSize.y = imageSize.y * widthFactor;
		}
		else
		{
			imageFinalSize.x = imageSize.x * heightFactor;
		}

		ImVec2 imageOffset = (resizedSize - imageFinalSize) * 0.5f;
		window->DrawList->AddImage(image, bb.Min + style.FramePadding + imageOffset, bb.Min + style.FramePadding + imageOffset + imageFinalSize);

		if (g.LogEnabled)
		{
			ImGui::LogSetNextTextDecoration("[", "]");
		}
		ImGui::RenderTextClipped(bb.Min + style.FramePadding + ImVec2(resizedSize.x, 0) + ImVec2(spaceBetweenImageAndText, 0), bb.Max - style.FramePadding, label, NULL,
		                         &label_size, style.ButtonTextAlign, &bb);

		// Automatically close popups
		// if (pressed && !(flags & ImGuiButtonFlags_DontClosePopups) && (window->Flags & ImGuiWindowFlags_Popup))
		//    CloseCurrentPopup();

		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
		return pressed;
	}
}
