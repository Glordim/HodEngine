#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/FontEditor/FontEditorPropertiesWindow.hpp"

#include "HodEngine/Editor/FontEditor/FontCooker.hpp"
#include "HodEngine/Editor/FontEditor/FontEditorTab.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <HodEngine/Renderer/Resource/FontResource.hpp>
#include <HodEngine/Text/FontFace.hpp>

#include <cinttypes>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(FontEditorPropertiesWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief
	/// @param editorTab
	FontEditorPropertiesWindow::FontEditorPropertiesWindow(EditorTab* editorTab)
	: EditorTabWindow(editorTab)
	{
		SetTitle("Properties");
	}

	/// @brief
	void FontEditorPropertiesWindow::DrawContent()
	{
		if (ImGui::CollapsingHeader("Settings", ImGuiTreeNodeFlags_DefaultOpen))
		{
			DrawSettings();
		}
		if (ImGui::CollapsingHeader("Information", ImGuiTreeNodeFlags_DefaultOpen))
		{
			DrawInformation();
		}
	}

	/// @brief Applied to the font as they are edited
	void FontEditorPropertiesWindow::DrawSettings()
	{
		FontEditorTab* fontEditorTab = GetOwner<FontEditorTab>();
		FontSettings&  fontSettings = fontEditorTab->GetFontSettings();
		bool           changed = false;

		const float valuePos = ImGui::GetContentRegionAvail().x * 0.4f;

		static const char* renderModeLabels[] = {"Smooth", "Pixel Art"};

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Render Mode");
		ImGui::SameLine(valuePos);
		ImGui::SetNextItemWidth(-1);
		if (ImGui::BeginCombo("##RenderMode", renderModeLabels[std::to_underlying(fontSettings._renderMode)]))
		{
			for (uint8_t renderMode = 0; renderMode < std::size(renderModeLabels); ++renderMode)
			{
				if (ImGui::Selectable(renderModeLabels[renderMode], std::to_underlying(fontSettings._renderMode) == renderMode))
				{
					fontSettings._renderMode = static_cast<FontResource::RenderMode>(renderMode);
					changed = true;
				}
			}
			ImGui::EndCombo();
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
		{
			ImGui::SetTooltip("Smooth: anti-aliased, rasterized at the size it is drawn at.\nPixel Art: on its grid of Pixel Size pixels an em, drawn at whole multiples of it, sharp.");
		}

		if (fontSettings._renderMode == FontResource::RenderMode::PixelArt)
		{
			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted("Pixel Size");
			ImGui::SameLine(valuePos);

			const float detectWidth = ImGui::CalcTextSize("Detect").x + ImGui::GetStyle().FramePadding.x * 2.0f;
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - detectWidth - ImGui::GetStyle().ItemSpacing.x);
			const uint32_t step = 1;
			if (ImGui::InputScalar("##PixelSize", ImGuiDataType_U32, &fontSettings._pixelSize, &step))
			{
				changed = true;
			}
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
			{
				ImGui::SetTooltip("The pixels an em of the grid the glyphs are drawn on (Silkscreen: 8).\nThe text is drawn at whole multiples of it.");
			}

			ImGui::SameLine();
			if (ImGui::Button("Detect"))
			{
				std::shared_ptr<FontResource> font = fontEditorTab->GetFont();
				const uint32_t                pixelSize = font != nullptr ? font->GetFontFace().DetectPixelGrid() : 0;
				if (pixelSize != 0)
				{
					fontSettings._pixelSize = pixelSize;
					changed = true;
					_detectMessage = String::Format("A grid of %u pixels an em", pixelSize);
				}
				else
				{
					_detectMessage = "No pixel grid found: not a pixel art font?";
				}
			}
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
			{
				ImGui::SetTooltip("The grid the outlines and advances of the glyphs are on");
			}

			if (_detectMessage.Empty() == false)
			{
				ImGui::SetCursorPosX(valuePos);
				ImGui::BeginDisabled();
				ImGui::TextWrapped("%s", _detectMessage.CStr());
				ImGui::EndDisabled();
			}

			if (fontSettings._pixelSize == 0)
			{
				ImGui::SetCursorPosX(valuePos);
				ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), ICON_MDI_ALERT " Drawn Smooth until it is set");
			}
		}

		if (changed)
		{
			fontEditorTab->ApplySettings();
			fontEditorTab->MarkAsDirty();
		}
	}

	/// @brief Names, counts and metrics, in font units and in ems
	void FontEditorPropertiesWindow::DrawInformation()
	{
		std::shared_ptr<FontResource> font = GetOwner<FontEditorTab>()->GetFont();
		if (font == nullptr || font->GetFontFace().IsLoaded() == false)
		{
			ImGui::TextDisabled("The font is not loaded");
			return;
		}

		const FontFace& face = font->GetFontFace();
		const float     valuePos = ImGui::GetContentRegionAvail().x * 0.4f;

		auto drawValue = [valuePos](const char* label, const char* value)
		{
			ImGui::TextUnformatted(label);
			ImGui::SameLine(valuePos);
			ImGui::TextUnformatted(value);
		};

		const String familyName = face.GetFamilyName();
		const String styleName = face.GetStyleName();
		drawValue("Family", familyName.Empty() ? "-" : familyName.CStr());
		drawValue("Style", styleName.Empty() ? "-" : styleName.CStr());
		drawValue("Glyphs", String::Format("%u", face.GetGlyphCount()).CStr());
		drawValue("Units per em", String::Format("%u", face.GetUnitsPerEm()).CStr());

		ImGui::Spacing();

		// PadOuterX: without the outer borders, ImGui gives the first and last columns no padding on their outer side
		if (ImGui::BeginTable("##Metrics", 3, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_PadOuterX))
		{
			ImGui::TableSetupColumn("Metric", ImGuiTableColumnFlags_WidthStretch, 2.0f);
			ImGui::TableSetupColumn("Units", ImGuiTableColumnFlags_WidthStretch, 1.0f);
			ImGui::TableSetupColumn("Em", ImGuiTableColumnFlags_WidthStretch, 1.0f);
			ImGui::TableHeadersRow();

			const float unitsPerEm = (float)face.GetUnitsPerEm();
			auto        drawMetric = [unitsPerEm](const char* name, int32_t units)
			{
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::TextUnformatted(name);
				ImGui::TableNextColumn();
				ImGui::Text("%" PRId32, units);
				ImGui::TableNextColumn();
				ImGui::Text("%.3f", (float)units / unitsPerEm);
			};
			drawMetric("Ascender", face.GetAscender());
			drawMetric("Cap height", face.GetCapHeight());
			drawMetric("x-height", face.GetXHeight());
			drawMetric("Descender", face.GetDescender());
			drawMetric("Line gap", face.GetLineGap());
			drawMetric("Line height", face.GetAscender() - face.GetDescender() + face.GetLineGap());

			ImGui::EndTable();
		}
	}
}
