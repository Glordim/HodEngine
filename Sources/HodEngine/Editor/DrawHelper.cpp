#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/DrawHelper.hpp"

#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"
#include "HodEngine/Core/String.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitIcon.hpp"
#include "HodEngine/Math/Vector2.hpp"
#include "HodEngine/Math/Vector4.hpp"

#include "HodEngine/ImGui/DearImGui/imgui.h"
#include "HodEngine/ImGui/DearImGui/imgui_internal.h"
#include "HodEngine/ImGui/Widgets.hpp"
#include "HodEngine/ImGui/Helper.hpp"
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <algorithm>
#include <cmath>

namespace hod::inline editor
{
	bool DrawHelper::DrawVector2(Vector2& value, const char* labelX, const char* labelY, bool enabledX, bool enabledY)
	{
		bool changed = false;
		float x = value.GetX();
		float y = value.GetY();

		ImGui::PushID(&value);

		float availableWidth = ImGui::GetColumnWidth();
		availableWidth -= CalculateButtonSize(labelX).x;
		availableWidth -= CalculateButtonSize(labelY).x;
		availableWidth -= ImGui::GetStyle().ItemSpacing.x - 2;
		
		ImGui::BeginDisabled(enabledX == false);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.5f, 0.0f, 0.0f, 1.0f));
		FramedText(labelX, ImDrawFlags_RoundLeft);
		ImGui::PopStyleColor();
		ImGui::SameLine(0.0f, 0.0f);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 1);
		ImGui::SetNextItemWidth(availableWidth * 0.5f);
		changed |= DragScalar("##x", ImGuiDataType_Float, &x, 1.0f, nullptr, nullptr, nullptr, 0, ImDrawFlags_RoundRight);
		ImGui::EndDisabled();

		ImGui::SameLine();

		ImGui::BeginDisabled(enabledY == false);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.5f, 0.0f, 1.0f));
		FramedText(labelY, ImDrawFlags_RoundLeft);
		ImGui::PopStyleColor();
		ImGui::SameLine(0.0f, 0.0f);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 1);
		ImGui::SetNextItemWidth(availableWidth * 0.5f);
		changed |= DragScalar("##y", ImGuiDataType_Float, &y, 1.0f, nullptr, nullptr, nullptr, 0, ImDrawFlags_RoundRight);
		ImGui::EndDisabled();

		ImGui::PopID();

		if (changed)
		{
			value.SetX(x);
			value.SetY(y);
		}

		return changed;
	}

	bool DrawHelper::DrawVector4(Vector4& value, const char* labelX, const char* labelY, const char* labelZ, const char* labelW)
	{
		bool changed = false;
		float x = value.GetX();
		float y = value.GetY();
		float z = value.GetZ();
		float w = value.GetW();

		ImGui::PushID(&value);

		float availableWidth = ImGui::GetColumnWidth();
		availableWidth -= CalculateButtonSize(labelX).x;
		availableWidth -= CalculateButtonSize(labelY).x;
		availableWidth -= CalculateButtonSize(labelZ).x;
		availableWidth -= CalculateButtonSize("W").x;
		availableWidth -= ImGui::GetStyle().ItemSpacing.x - 2;

		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.5f, 0.0f, 0.0f, 1.0f));
		FramedText(labelX, ImDrawFlags_RoundLeft);
		ImGui::PopStyleColor();
		ImGui::SameLine(0.0f, 0.0f);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 1);
		ImGui::SetNextItemWidth(availableWidth * 0.24f);
		changed |= DragScalar("##x", ImGuiDataType_Float, &x, 1.0f, nullptr, nullptr, nullptr, 0, ImDrawFlags_RoundRight);

		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.5f, 0.0f, 1.0f));
		FramedText(labelY, ImDrawFlags_RoundLeft);
		ImGui::PopStyleColor();
		ImGui::SameLine(0.0f, 0.0f);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 1);
		ImGui::SetNextItemWidth(availableWidth * 0.24f);
		changed |= DragScalar("##y", ImGuiDataType_Float, &y, 1.0f, nullptr, nullptr, nullptr, 0, ImDrawFlags_RoundRight);

		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
		FramedText(labelZ, ImDrawFlags_RoundLeft);
		ImGui::PopStyleColor();
		ImGui::SameLine(0.0f, 0.0f);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 1);
		ImGui::SetNextItemWidth(availableWidth * 0.24f);
		changed |= DragScalar("##z", ImGuiDataType_Float, &z, 1.0f, nullptr, nullptr, nullptr, 0, ImDrawFlags_RoundRight);

		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
		FramedText(labelW, ImDrawFlags_RoundLeft);
		ImGui::PopStyleColor();
		ImGui::SameLine(0.0f, 0.0f);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 1);
		ImGui::SetNextItemWidth(availableWidth * 0.24f);
		changed |= DragScalar("##w", ImGuiDataType_Float, &w, 1.0f, nullptr, nullptr, nullptr, 0, ImDrawFlags_RoundRight);

		ImGui::PopID();

		if (changed)
		{
			value.SetX(x);
			value.SetY(y);
			value.SetZ(z);
			value.SetW(w);
		}

		return changed;
	}

	bool DrawHelper::DrawToggleWithSettings(const char* label, const char* itemName, const char* settingsPopupId, bool& toggled)
	{
		bool changed = false;

		// Queried/opened outside of the PushID scope below so the popup ID matches the one the caller's BeginPopup(settingsPopupId) computes.
		bool settingsOpened = ImGui::IsPopupOpen(settingsPopupId);

		ImGui::PushID(settingsPopupId);

		// Remembered for BeginSettingsPopup, refreshed every frame so the popup follows the widget (scroll, docking, resize...).
		ImVec2 anchor = ImGui::GetCursorScreenPos();
		anchor.y += ImGui::GetFrameHeight();
		ImGui::GetStateStorage()->SetFloat(ImGui::GetID("AnchorX"), anchor.x);
		ImGui::GetStateStorage()->SetFloat(ImGui::GetID("AnchorY"), anchor.y);

		if (FramedButton(label, toggled, ImDrawFlags_RoundLeft))
		{
			toggled = !toggled;
			changed = true;
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
		{
			ImGui::SetTooltip("%s %s", toggled ? "Hide" : "Show", itemName);
		}

		// Glued to the toggle, at its height: SameLine alone would go back to the height of the line, not of the toggle if it was moved (SetCursorScreenPos)
		const ImVec2 settingsPosition(ImGui::GetItemRectMax().x - 1.0f, ImGui::GetItemRectMin().y);
		ImGui::SameLine(0.0f, 0.0f);
		ImGui::SetCursorScreenPos(settingsPosition);
		bool openSettings = FramedButton(ICON_MDI_MENU_DOWN "##Settings", settingsOpened, ImDrawFlags_RoundRight);
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip) && settingsOpened == false)
		{
			ImGui::SetTooltip("Configure %s", itemName);
		}

		ImGui::PopID();

		if (openSettings)
		{
			ImGui::OpenPopup(settingsPopupId);
		}

		return changed;
	}

	bool DrawHelper::DrawToggle(const char* label, const char* itemName, bool& toggled)
	{
		bool changed = false;
		if (FramedButton(label, toggled))
		{
			toggled = !toggled;
			changed = true;
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
		{
			ImGui::SetTooltip("%s %s", toggled ? "Hide" : "Show", itemName);
		}
		return changed;
	}

	void DrawHelper::DrawDropDownButton(const char* label, const char* tooltip, const char* popupId)
	{
		bool popupOpened = ImGui::IsPopupOpen(popupId);

		// Same anchor bookkeeping as DrawToggleWithSettings, for BeginSettingsPopup.
		ImGui::PushID(popupId);
		ImVec2 anchor = ImGui::GetCursorScreenPos();
		anchor.y += ImGui::GetFrameHeight();
		ImGui::GetStateStorage()->SetFloat(ImGui::GetID("AnchorX"), anchor.x);
		ImGui::GetStateStorage()->SetFloat(ImGui::GetID("AnchorY"), anchor.y);

		String buttonLabel = String(label) + " " ICON_MDI_MENU_DOWN "##DropDown";
		bool   clicked = FramedButton(buttonLabel.CStr(), popupOpened);
		if (tooltip != nullptr && popupOpened == false && ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
		{
			ImGui::SetTooltip("%s", tooltip);
		}
		ImGui::PopID();

		if (clicked)
		{
			ImGui::OpenPopup(popupId);
		}
	}

	bool DrawHelper::BeginSettingsPopup(const char* settingsPopupId)
	{
		// Same ID scope as DrawToggleWithSettings' write; BeginPopup itself stays outside of it so its ID matches the one OpenPopup used.
		ImGui::PushID(settingsPopupId);
		ImVec2 anchor(ImGui::GetStateStorage()->GetFloat(ImGui::GetID("AnchorX")), ImGui::GetStateStorage()->GetFloat(ImGui::GetID("AnchorY")));
		ImGui::PopID();

		ImGui::SetNextWindowPos(anchor, ImGuiCond_Always, ImVec2(0.0f, 0.0f));
		return ImGui::BeginPopup(settingsPopupId);
	}

	float DrawHelper::GetHiddenTabBarButtonWidth()
	{
		// Same condition and size as the "#UNHIDE" triangle in ImGui::Begin.
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->DockIsActive && window->DockNode->IsHiddenTabBar() && window->DockNode->IsNoTabBar() == false)
		{
			return ImTrunc(ImGui::GetFontSize() * 0.70f);
		}
		return 0.0f;
	}

	void DrawHelper::DrawHiddenTabBarButton()
	{
		float size = GetHiddenTabBarButtonWidth();
		if (size == 0.0f)
		{
			return;
		}

		// Same ID, hover/held state and color as the "#UNHIDE" button in ImGui::Begin (the window ID is back on top of the ID stack here).
		ImGuiContext& g = *ImGui::GetCurrentContext();
		ImGuiWindow*  window = ImGui::GetCurrentWindow();
		ImGuiDockNode* node = window->DockNode;
		ImGuiID       id = window->GetID("#UNHIDE");
		bool          hovered = g.HoveredId == id;
		bool          held = g.ActiveId == id;
		ImU32         color = ImGui::GetColorU32(((held && hovered) || (node->IsFocused && !hovered)) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);

		ImVec2 p = node->Pos;
		window->DrawList->PushClipRect(window->Pos, window->Pos + window->Size, false);
		window->DrawList->AddTriangleFilled(p, p + ImVec2(size, 0.0f), p + ImVec2(0.0f, size), color);
		window->DrawList->PopClipRect();
	}

	namespace
	{
		/// @brief Inspector block header button, at the current cursor position, fading in as the mouse gets close to it and red when hovered.
		/// @return true if clicked
		bool FadingHeaderButton(const char* icon)
		{
			ImVec2 buttonPos = ImGui::GetCursorScreenPos();
			ImVec2 mousePos = ImGui::GetMousePos();

			float distance = (float)std::sqrt(std::pow(buttonPos.x - mousePos.x, 2) + std::pow(buttonPos.y - mousePos.y, 2));
			distance = std::clamp(distance, 15.0f, 40.0f);
			float alpha = 1.0f - ((distance - 15.0f) / (40.0f - 15.0f));

			ImVec4 textColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
			textColor.w = alpha;

			ImVec4 buttonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
			buttonColor.w = alpha;

			ImVec4 borderColor = ImGui::GetStyleColorVec4(ImGuiCol_Border);
			borderColor.w = alpha;

			ImGui::PushStyleColor(ImGuiCol_Border, borderColor);
			ImGui::PushStyleColor(ImGuiCol_Text, textColor);
			ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
			bool clicked = ImGui::Button(icon);
			ImGui::PopStyleColor(4);
			return clicked;
		}
	}

	bool DrawHelper::BeginInspectorBlock(const void* id, const char* icon, const char* title, bool* enabled, bool* removeRequested, const char* actionIcon, const char* actionTooltip, bool* actionRequested)
	{
		ImGui::PushID(id);
		if (ImGui::BeginChild("InspectorBlock", ImVec2(0.0f, 0.0f), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_Borders) == false)
		{
			return false;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::SetCursorPosX(0.0f);
		ImGui::SetCursorPosY(0.0f);
		ImGui::PopStyleVar(1);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
		ImGui::SetCursorPosX(0.0f);
		ImGui::Separator();
		ImGui::SetCursorPosX(0.0f);
		ImGui::PopStyleVar(2);

		ImVec2 min = ImGui::GetCursorScreenPos();
		ImVec2 max = min;
		max.x += ImGui::GetWindowWidth();
		max.y += ImGui::GetTextLineHeight() + ImGui::GetStyle().FramePadding.y * 4;
		ImGui::GetWindowDrawList()->AddRectFilled(min, max, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_FrameBg)));

		min.y += ImGui::GetStyle().FramePadding.y;
		ImGui::SetCursorScreenPos(min);

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

		ImGuiStorage* storage = ImGui::GetCurrentWindow()->DC.StateStorage;
		ImGuiID       collapsedId = ImGui::GetID("Collapsed");
		bool          collapsed = storage->GetInt(collapsedId, false);

		if (ImGui::ArrowButton("CollapseBlock", collapsed == false ? ImGuiDir_Down : ImGuiDir_Right))
		{
			collapsed = !collapsed;
			storage->SetInt(collapsedId, collapsed);
		}
		ImGui::PopStyleColor(4);

		bool disabled = false;
		if (enabled != nullptr)
		{
			ImGui::SameLine();
			ImGui::Checkbox("##Enabled", enabled);
			disabled = (*enabled == false);
		}

		ImGui::BeginDisabled(disabled);
		ImGui::SameLine();
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(icon);
		ImGui::SameLine();
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(title);
		ImGui::EndDisabled();

		// Right-aligned header buttons: the close button last, the action button just before it.
		float rightEdge = ImGui::GetContentRegionAvail().x + 10.0f;
		float closeWidth = (removeRequested != nullptr) ? CalculateButtonSize(ICON_MDI_CLOSE).x : 0.0f;
		if (actionRequested != nullptr && actionIcon != nullptr)
		{
			float spacing = (removeRequested != nullptr) ? ImGui::GetStyle().ItemSpacing.x : 0.0f;
			ImGui::SameLine(rightEdge - closeWidth - spacing - CalculateButtonSize(actionIcon).x, 0.0f);
			if (FadingHeaderButton(actionIcon))
			{
				*actionRequested = true;
			}
			if (actionTooltip != nullptr)
			{
				ImGui::SetItemTooltip("%s", actionTooltip);
			}
		}
		if (removeRequested != nullptr)
		{
			ImGui::SameLine(rightEdge - closeWidth, 0.0f);
			if (FadingHeaderButton(ICON_MDI_CLOSE))
			{
				*removeRequested = true;
			}
		}

		min.y = max.y;
		max.x += ImGui::GetWindowWidth();
		ImGui::GetWindowDrawList()->AddLine(min, max, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Separator)));

		if (collapsed == false)
		{
			ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2(0.0f, 2.0f));
			return true;
		}

		ImVec2 pos = ImGui::GetCursorScreenPos();
		pos.y = max.y;
		ImGui::SetCursorScreenPos(pos);
		return false;
	}

	void DrawHelper::EndInspectorBlock()
	{
		ImGui::Dummy(ImVec2(0, 0)); // todo
		ImGui::EndChild();
		ImGui::PopID();
	}

	/// @brief
	/// @param descriptor
	/// @param fallback
	/// @return
	const char* DrawHelper::GetTypeIcon(const ReflectionDescriptor& descriptor, const char* fallback)
	{
		ReflectionTraitIcon* iconTrait = descriptor.FindTrait<ReflectionTraitIcon>();
		return iconTrait != nullptr ? iconTrait->GetValue().CStr() : fallback;
	}
}
