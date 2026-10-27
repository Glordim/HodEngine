#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/FontFaceEditor/FontFaceEditorPropertiesWindow.hpp"

#include "HodEngine/Editor/FontFaceEditor/FontFaceCooker.hpp"
#include "HodEngine/Editor/FontFaceEditor/FontFaceEditorTab.hpp"

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/FontEditor/FontHelper.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <HodEngine/Renderer/Resource/FontFaceResource.hpp>
#include <HodEngine/Text/FontFace.hpp>

#include <cinttypes>
#include <utility>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(FontFaceEditorPropertiesWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief
	/// @param editorTab
	FontFaceEditorPropertiesWindow::FontFaceEditorPropertiesWindow(EditorTab* editorTab)
	: EditorTabWindow(editorTab)
	{
		SetTitle("Properties");
	}


	/// @brief
	void FontFaceEditorPropertiesWindow::DrawContent()
	{
		DrawFonts();
		if (ImGui::CollapsingHeader("Settings", ImGuiTreeNodeFlags_DefaultOpen))
		{
			DrawSettings();
		}
		if (ImGui::CollapsingHeader("Information", ImGuiTreeNodeFlags_DefaultOpen))
		{
			DrawInformation();
		}
	}

	/// @brief A text is drawn with a font, not a face: the fonts the face is a style of, opened from here, or one made from it
	void FontFaceEditorPropertiesWindow::DrawFonts()
	{
		FontFaceEditorTab*                    fontFaceEditorTab = GetOwner<FontFaceEditorTab>();
		const Vector<std::shared_ptr<Asset>>& fonts = fontFaceEditorTab->GetFonts();

		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_FrameBg));
		ImGui::BeginChild("##Fonts", ImVec2(0.0f, 0.0f), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysUseWindowPadding);
		if (fonts.Empty())
		{
			ImGui::TextWrapped(ICON_MDI_INFORMATION_OUTLINE " A text is drawn with a font, the family this face is one of the styles of: none uses it yet.");
			if (ImGui::Button(ICON_MDI_PLUS " Create Font"))
			{
				std::shared_ptr<Asset> asset = fontFaceEditorTab->GetAsset();
				const Path             fontPath = FontHelper::CreateFont({asset}, asset->GetPath().ParentPath());
				_createMessage = fontPath.Empty() ? String("The font could not be made") : String::Format("%s made, its styles detected", fontPath.Filename().CStr());
			}
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
			{
				ImGui::SetTooltip("A font next to this face, with this face and the ones of its family in its folder for their styles");
			}
		}
		else
		{
			ImGui::TextUnformatted(ICON_MDI_INFORMATION_OUTLINE " A style of:");
			for (const std::shared_ptr<Asset>& font : fonts)
			{
				ImGui::SameLine();
				if (ImGui::SmallButton(font->GetName().CStr()))
				{
					Editor::GetInstance()->OpenAsset(font);
				}
			}
		}
		ImGui::SameLine();
		if (ImGui::SmallButton(ICON_MDI_REFRESH))
		{
			fontFaceEditorTab->FindFonts();
			_createMessage.Clear();
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
		{
			ImGui::SetTooltip("Look for the fonts it is a style of again");
		}
		if (_createMessage.Empty() == false)
		{
			ImGui::TextDisabled("%s", _createMessage.CStr());
		}
		ImGui::EndChild();
		ImGui::PopStyleColor();
		ImGui::Spacing();
	}

	/// @brief Applied to the font as they are edited
	void FontFaceEditorPropertiesWindow::DrawSettings()
	{
		FontFaceEditorTab* fontFaceEditorTab = GetOwner<FontFaceEditorTab>();
		FontFaceSettings&  fontSettings = fontFaceEditorTab->GetFontFaceSettings();
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
				if (ImGui::Selectable(renderModeLabels[renderMode], std::to_underlying(fontSettings._renderMode) == renderMode) &&
				    std::to_underlying(fontSettings._renderMode) != renderMode)
				{
					fontSettings._renderMode = static_cast<FontFaceResource::RenderMode>(renderMode);
					changed = true;

					// Never set: the grid of the font, if it has one
					if (fontSettings._renderMode == FontFaceResource::RenderMode::PixelArt && fontSettings._pixelSize == 0)
					{
						DetectPixelSize();
					}
				}
			}
			ImGui::EndCombo();
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
		{
			ImGui::SetTooltip("Smooth: anti-aliased, rasterized at the size it is drawn at.\nPixel Art: on its grid of Pixel Size pixels an em, drawn at whole multiples of it, sharp.");
		}

		if (fontSettings._renderMode == FontFaceResource::RenderMode::PixelArt)
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
				changed |= DetectPixelSize();
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
			fontFaceEditorTab->ApplySettings();
			fontFaceEditorTab->MarkAsDirty();
		}
	}


	/// @brief The grid the outlines and advances of the glyphs are on, what was found shown under the pixel size
	/// @return
	bool FontFaceEditorPropertiesWindow::DetectPixelSize()
	{
		FontFaceEditorTab*                fontFaceEditorTab = GetOwner<FontFaceEditorTab>();
		std::shared_ptr<FontFaceResource> font = fontFaceEditorTab->GetFontFace();
		const uint32_t                pixelSize = font != nullptr ? font->GetFontFace().DetectPixelGrid() : 0;
		if (pixelSize == 0)
		{
			_detectMessage = "No pixel grid found: not a pixel art font?";
			return false;
		}

		fontFaceEditorTab->GetFontFaceSettings()._pixelSize = pixelSize;
		_detectMessage = String::Format("A grid of %u pixels an em", pixelSize);
		return true;
	}

	/// @brief Names, counts and metrics, in font units and in ems
	void FontFaceEditorPropertiesWindow::DrawInformation()
	{
		std::shared_ptr<FontFaceResource> font = GetOwner<FontFaceEditorTab>()->GetFontFace();
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
		drawValue("Weight", String::Format("%u", face.GetWeight()).CStr());
		drawValue("Italic", face.IsItalic() ? "Yes" : "No");
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
