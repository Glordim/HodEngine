#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/FontEditor/FontPreviewTab.hpp"

#include "HodEngine/Editor/DrawHelper.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <HodEngine/Core/Math.hpp>

#include <cstring>

namespace hod::inline editor
{
	namespace
	{
		// What a font is often checked on
		struct PreviewPreset
		{
			const char* _name;
			const char* _text;
		};

		constexpr PreviewPreset PreviewPresets[] = {
			{"Pangram", "The quick brown fox jumps over the lazy dog"},
			{"Capitals", "THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG"},
			{"Digits and punctuation", "0123456789 .,;:!? ()[]{} +-*/=%& @#$"},
			{"Accents", "\xC3\x89t\xC3\xA9 \xC3\xA0 l'\xC3\xAE" "le, \xC3\xA7" "a co\xC3\xBBte \xC5\x93uvre \xC3\x80 \xC3\x87"}, // Été à l'île, ça coûte œuvre À Ç
			{"Kerning", "AVATAR Tokyo WAVE Yo LT"},
		};
	}

	/// @brief
	/// @param asset
	/// @param icon
	FontPreviewTab::FontPreviewTab(std::shared_ptr<Asset> asset, const char* icon)
	: EditorTab(asset, icon)
	{
	}

	/// @brief
	/// @return
	uint32_t FontPreviewTab::GetPreviewVersion() const
	{
		return _previewVersion;
	}

	/// @brief
	/// @return
	const char* FontPreviewTab::GetPreviewText() const
	{
		return _previewText;
	}

	/// @brief
	/// @return
	bool FontPreviewTab::IsShowingMetrics() const
	{
		return _showMetrics;
	}

	/// @brief
	/// @return
	float FontPreviewTab::GetZoomFactor() const
	{
		return _zoomFactor;
	}

	/// @brief
	/// @param zoomFactor
	void FontPreviewTab::SetZoomFactor(float zoomFactor)
	{
		_zoomFactor = Clamp(zoomFactor, MinZoomFactor, MaxZoomFactor);
	}

	/// @brief
	/// @return
	const Vector2& FontPreviewTab::GetPanOffset() const
	{
		return _panOffset;
	}

	/// @brief
	/// @param panOffset
	void FontPreviewTab::SetPanOffset(const Vector2& panOffset)
	{
		_panOffset = panOffset;
	}

	/// @brief
	void FontPreviewTab::InvalidatePreview()
	{
		++_previewVersion;
	}

	/// @brief
	void FontPreviewTab::DrawMenuBarPopups()
	{
		if (DrawHelper::BeginSettingsPopup("FontPreviewPresets"))
		{
			for (const PreviewPreset& preset : PreviewPresets)
			{
				if (ImGui::Selectable(preset._name))
				{
					std::strncpy(_previewText, preset._text, sizeof(_previewText) - 1);
					++_previewVersion;
				}
			}
			ImGui::EndPopup();
		}
	}

	/// @brief What the tab adds, the text of the preview, the metrics shown or not, the zoom
	void FontPreviewTab::DrawMenuBar()
	{
		ImGui::SameLine(0.0f, 8.0f);

		// SameLine goes back to the top of the line: each item is centered on the 28 pixels high buttons of the bar (below its 2 pixels of margin)
		const float lineTop = ImGui::GetCursorScreenPos().y;
		auto        centerOnBar = [lineTop]()
		{
			ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x, lineTop + 2.0f + (28.0f - ImGui::GetFrameHeight()) * 0.5f));
		};

		centerOnBar();
		DrawMenuBarOptions();

		centerOnBar();
		ImGui::SetNextItemWidth(360.0f);
		if (ImGui::InputTextWithHint("##PreviewText", "Text of the preview", _previewText, sizeof(_previewText)))
		{
			++_previewVersion;
		}

		// The frame padding of the editor style (ImGuiManager), as the toggles of the other viewports
		ImGui::PushStyleVarX(ImGuiStyleVar_FramePadding, 5.0f);
		ImGui::SameLine(0.0f, 4.0f);
		centerOnBar();
		DrawHelper::DrawDropDownButton(ICON_MDI_TEXT_BOX_OUTLINE " Presets", "Texts to preview the font with", "FontPreviewPresets"); // Its popup: DrawMenuBarPopups

		ImGui::SameLine(0.0f, 8.0f);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, 2.0f);
		ImGui::SameLine(0.0f, 8.0f);
		centerOnBar();
		DrawHelper::DrawToggle(ICON_MDI_FORMAT_LINE_SPACING " Metrics", "metrics (ascender, cap height, x-height, baseline, descender)", _showMetrics);
		ImGui::PopStyleVar();

		ImGui::SameLine(0.0f, 8.0f);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, 2.0f);
		ImGui::SameLine(0.0f, 8.0f);

		centerOnBar();
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Zoom");
		ImGui::SameLine(0.0f, 8.0f);
		centerOnBar();
		ImGui::SetNextItemWidth(160);
		ImGui::SliderFloat("##Zoom", &_zoomFactor, MinZoomFactor, MaxZoomFactor, "", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_AlwaysClamp);
		ImGui::SameLine(0.0f, 8.0f);
		centerOnBar();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("%u%%", static_cast<uint32_t>(_zoomFactor * 100.0f));

		ImGui::SameLine(0.0f, 8.0f);
		centerOnBar();
		// Resets the whole view: zoom and pan
		ImGui::BeginDisabled(_zoomFactor == 1.0f && _panOffset == Vector2(DefaultMargin, -DefaultMargin));
		if (ImGui::Button(ICON_MDI_UNDO))
		{
			_zoomFactor = 1.0f;
			_panOffset = Vector2(DefaultMargin, -DefaultMargin);
		}
		ImGui::EndDisabled();
	}
}
