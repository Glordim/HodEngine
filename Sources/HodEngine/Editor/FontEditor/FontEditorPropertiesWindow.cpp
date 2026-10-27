#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/FontEditor/FontEditorPropertiesWindow.hpp"

#include "HodEngine/Editor/FontEditor/FontCooker.hpp"
#include "HodEngine/Editor/FontEditor/FontEditorTab.hpp"

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"

#include <HodEngine/Core/Reflection/EnumDescriptor.hpp>
#include <HodEngine/Core/Reflection/ReflectionHelper.hpp>
#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <HodEngine/GameSystems/Resource/ResourceManager.hpp>
#include <HodEngine/Renderer/Resource/FontResource.hpp>
#include <HodEngine/Text/FontFace.hpp>

#include <algorithm>
#include <cinttypes>
#include <limits>
#include <utility>

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
		if (ImGui::CollapsingHeader("Fallbacks", ImGuiTreeNodeFlags_DefaultOpen))
		{
			DrawFallbacks();
		}
		if (ImGui::CollapsingHeader("Styles", ImGuiTreeNodeFlags_DefaultOpen))
		{
			DrawStyles();
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
				if (ImGui::Selectable(renderModeLabels[renderMode], std::to_underlying(fontSettings._renderMode) == renderMode) &&
				    std::to_underlying(fontSettings._renderMode) != renderMode)
				{
					fontSettings._renderMode = static_cast<FontResource::RenderMode>(renderMode);
					changed = true;

					// Never set: the grid of the font, if it has one
					if (fontSettings._renderMode == FontResource::RenderMode::PixelArt && fontSettings._pixelSize == 0)
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
			fontEditorTab->ApplySettings();
			fontEditorTab->MarkAsDirty();
		}
	}

	/// @brief The fonts the characters this one lacks are drawn with, in order: moved, removed, added. Applied as they are edited.
	/// Under them, the whole chain a text is drawn with: their own fallbacks after each of them
	void FontEditorPropertiesWindow::DrawFallbacks()
	{
		FontEditorTab* fontEditorTab = GetOwner<FontEditorTab>();
		FontSettings&  fontSettings = fontEditorTab->GetFontSettings();
		bool           changed = false;

		ImGui::BeginDisabled();
		ImGui::TextWrapped("The characters the font lacks are drawn with the first of these that has them.");
		ImGui::EndDisabled();

		// The picker of a resource, as the inspector draws it
		EditorReflectedObject    reflectedObject(&fontSettings, &fontSettings.GetReflectionDescriptorV(), nullptr, this);
		EditorReflectedProperty* fallbacksProperty = reflectedObject.FindProperty("Fallbacks");

		uint32_t moveUp = std::numeric_limits<uint32_t>::max();
		uint32_t moveDown = std::numeric_limits<uint32_t>::max();
		uint32_t remove = std::numeric_limits<uint32_t>::max();
		if (fallbacksProperty != nullptr && fontSettings._fallbacks.Empty() == false &&
		    ImGui::BeginTable("##Fallbacks", 2, ImGuiTableFlags_PadOuterX | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerH))
		{
			ImGui::TableSetupColumn("Order", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Font", ImGuiTableColumnFlags_WidthStretch);

			// The parent of its elements (they reach the window through it), as the inspector makes it before drawing an array
			fallbacksProperty->GetEditorReflectedObject();

			const uint32_t fallbackCount = fontSettings._fallbacks.Size();
			for (uint32_t index = 0; index < fallbackCount; ++index)
			{
				ImGui::PushID((int)index);
				ImGui::TableNextRow();

				ImGui::TableNextColumn();
				ImGui::AlignTextToFramePadding();
				ImGui::Text("%u", index + 1);
				ImGui::SameLine();
				ImGui::BeginDisabled(index == 0);
				if (ImGui::SmallButton(ICON_MDI_ARROW_UP))
				{
					moveUp = index;
				}
				ImGui::EndDisabled();
				ImGui::SameLine(0.0f, 2.0f);
				ImGui::BeginDisabled(index + 1 == fallbackCount);
				if (ImGui::SmallButton(ICON_MDI_ARROW_DOWN))
				{
					moveDown = index;
				}
				ImGui::EndDisabled();
				ImGui::SameLine(0.0f, 2.0f);
				if (ImGui::SmallButton(ICON_MDI_CLOSE))
				{
					remove = index;
				}

				ImGui::TableNextColumn();
				EditorReflectedProperty elementProperty = fallbacksProperty->GenerateElementProperty(index);
				changed |= PropertyDrawer::DrawPropertyObject(elementProperty, true);

				const UID& uid = fontSettings._fallbacks[index].GetUid();
				std::shared_ptr<Asset> asset = fontEditorTab->GetAsset();
				if (asset != nullptr && uid == asset->GetUid())
				{
					ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), ICON_MDI_ALERT " The font itself: left out");
				}
				ImGui::PopID();
			}
			ImGui::EndTable();
		}

		if (moveUp < fontSettings._fallbacks.Size())
		{
			std::swap(fontSettings._fallbacks[moveUp - 1], fontSettings._fallbacks[moveUp]);
			changed = true;
		}
		if (moveDown < fontSettings._fallbacks.Size())
		{
			std::swap(fontSettings._fallbacks[moveDown], fontSettings._fallbacks[moveDown + 1]);
			changed = true;
		}
		if (remove < fontSettings._fallbacks.Size())
		{
			fontSettings._fallbacks.Erase(remove);
			changed = true;
		}

		if (ImGui::Button(ICON_MDI_PLUS " Add a fallback"))
		{
			fontSettings._fallbacks.EmplaceBack();
			changed = true;
		}

		if (changed)
		{
			fontEditorTab->ApplySettings();
			fontEditorTab->MarkAsDirty();
		}

		// What a text is drawn with: the fallbacks of the fallbacks included, each font once
		std::shared_ptr<FontResource> font = fontEditorTab->GetFont();
		if (font != nullptr && font->GetFallbacks().Empty() == false)
		{
			_fontChain.Update(font);

			String chain;
			for (const std::shared_ptr<FontResource>& chainFont : _fontChain.GetFonts())
			{
				std::shared_ptr<Asset> asset = AssetDatabase::GetInstance()->Find(chainFont->GetUid());
				if (chain.Empty() == false)
				{
					chain += "  " ICON_MDI_ARROW_RIGHT "  ";
				}
				chain += asset != nullptr ? asset->GetName() : chainFont->GetUid().ToString();
			}

			ImGui::Spacing();
			ImGui::BeginDisabled();
			ImGui::TextWrapped("Drawn with: %s", chain.CStr());
			ImGui::EndDisabled();
		}
	}

	/// @brief A combo of the weights, "Bold (700)"
	/// @param id
	/// @param weight
	/// @return true when another one is picked
	static bool DrawWeightCombo(const char* id, FontResource::Weight& weight)
	{
		const EnumDescriptor& enumDescriptor = ReflectedEnum<FontResource::Weight>::GetEnumDescriptor();

		bool         changed = false;
		const String preview = String::Format("%s (%u)", enumDescriptor.ToString(weight).data(), (uint32_t)std::to_underlying(weight));
		if (ImGui::BeginCombo(id, preview.CStr()))
		{
			for (uint16_t value = 100; value <= 900; value += 100)
			{
				const FontResource::Weight item = static_cast<FontResource::Weight>(value);
				const String               label = String::Format("%s (%u)", enumDescriptor.ToString(item).data(), (uint32_t)value);
				if (ImGui::Selectable(label.CStr(), item == weight) && item != weight)
				{
					weight = item;
					changed = true;
				}
			}
			ImGui::EndCombo();
		}
		return changed;
	}

	/// @brief The fonts of the family of this one, the font drawn for each style: a text referencing this font and asking for a style is drawn with
	/// the closest. The same font for several, this one among them, as wished. Applied as they are edited
	void FontEditorPropertiesWindow::DrawStyles()
	{
		FontEditorTab* fontEditorTab = GetOwner<FontEditorTab>();
		FontSettings&  fontSettings = fontEditorTab->GetFontSettings();
		bool           changed = false;

		ImGui::BeginDisabled();
		ImGui::TextWrapped("A text asking for a style is drawn with the closest of these (the italic first, then the weight), then with this font for what it lacks. "
		                   "None: this font for all of them. Its fallbacks are drawn in the style asked for too, from their own styles.");
		ImGui::EndDisabled();

		uint32_t remove = std::numeric_limits<uint32_t>::max();
		if (fontSettings._styles.Empty() == false &&
		    ImGui::BeginTable("##Styles", 4, ImGuiTableFlags_PadOuterX | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerH))
		{
			ImGui::TableSetupColumn("Weight", ImGuiTableColumnFlags_WidthStretch, 1.0f);
			ImGui::TableSetupColumn("Italic", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Font", ImGuiTableColumnFlags_WidthStretch, 2.0f);
			ImGui::TableSetupColumn("##Remove", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableHeadersRow();

			for (uint32_t index = 0; index < fontSettings._styles.Size(); ++index)
			{
				FontSettings::Style& style = fontSettings._styles[index];

				ImGui::PushID((int)index);
				ImGui::TableNextRow();

				ImGui::TableNextColumn();
				ImGui::SetNextItemWidth(-1);
				changed |= DrawWeightCombo("##Weight", style._weight);

				ImGui::TableNextColumn();
				changed |= ImGui::Checkbox("##Italic", &style._italic);

				// The style reflected on its own: an element of the array has no parent object to reach the window through
				ImGui::TableNextColumn();
				EditorReflectedObject    styleObject(&style, &style.GetReflectionDescriptorV(), nullptr, this);
				EditorReflectedProperty* fontProperty = styleObject.FindProperty("Font");
				if (fontProperty != nullptr)
				{
					changed |= PropertyDrawer::DrawPropertyObject(*fontProperty, true);
				}

				// What the file says, when it is not the style it is drawn for: not wrong (a Bold drawn for Black...), to know
				std::shared_ptr<FontResource> font = style._font.Lock();
				if (font != nullptr && font->GetFontFace().IsLoaded())
				{
					const FontSettings::Style fileStyle = FontSettings::DetectStyle(font->GetFontFace());
					if (fileStyle._weight != style._weight || fileStyle._italic != style._italic)
					{
						const String weightName(ReflectedEnum<FontResource::Weight>::GetEnumDescriptor().ToString(fileStyle._weight));
						ImGui::TextDisabled("The file says %s%s", weightName.CStr(), fileStyle._italic ? " Italic" : "");
					}
				}

				ImGui::TableNextColumn();
				if (ImGui::SmallButton(ICON_MDI_CLOSE))
				{
					remove = index;
				}
				ImGui::PopID();
			}
			ImGui::EndTable();
		}

		if (remove < fontSettings._styles.Size())
		{
			fontSettings._styles.Erase(remove);
			changed = true;
		}

		if (ImGui::Button(ICON_MDI_PLUS " Add a style"))
		{
			fontSettings._styles.EmplaceBack();
			changed = true;
		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_MDI_MAGNIFY " Detect##Styles"))
		{
			changed |= DetectStyles();
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
		{
			ImGui::SetTooltip("Adds this font and the fonts of its folder of its family (and width), each for the style its file says it is.\n"
			                  "The ones already there are left as they are.");
		}
		if (_detectStylesMessage.Empty() == false)
		{
			ImGui::BeginDisabled();
			ImGui::TextWrapped("%s", _detectStylesMessage.CStr());
			ImGui::EndDisabled();
		}

		if (changed)
		{
			fontEditorTab->ApplySettings();
			fontEditorTab->MarkAsDirty();
		}
	}

	/// @brief This font and the fonts of the folder of the asset (not below it) whose face has its family and its width: "Noto Sans Bold" for
	/// "Noto Sans", not "Noto Sans Condensed Bold". Added for the style their file says (see FontSettings::DetectStyle), sorted, but for the ones
	/// already in, and the ones of a style already taken: said in the message
	/// @return
	bool FontEditorPropertiesWindow::DetectStyles()
	{
		FontEditorTab*                fontEditorTab = GetOwner<FontEditorTab>();
		FontSettings&                 fontSettings = fontEditorTab->GetFontSettings();
		std::shared_ptr<FontResource> font = fontEditorTab->GetFont();
		std::shared_ptr<Asset>        asset = fontEditorTab->GetAsset();
		if (font == nullptr || font->GetFontFace().IsLoaded() == false || asset == nullptr)
		{
			_detectStylesMessage = "The font is not loaded";
			return false;
		}

		const String family = font->GetFontFace().GetFamilyName();
		const float  width = font->GetFontFace().GetWidth();
		if (family.Empty())
		{
			_detectStylesMessage = "The font file has no family name";
			return false;
		}

		AssetDatabase::FileSystemMapping* node = AssetDatabase::GetInstance()->FindFileSystemMappingFromPath(asset->GetPath());
		if (node == nullptr || node->_parentFolder == nullptr)
		{
			_detectStylesMessage = "The folder of the font is not found";
			return false;
		}

		Vector<FontSettings::Style> found;
		String                      skipped;
		auto                        add = [&fontSettings, &found, &skipped](const Asset& candidateAsset, const FontFace& face)
		{
			for (const FontSettings::Style& style : fontSettings._styles)
			{
				if (style._font.GetUid() == candidateAsset.GetUid())
				{
					return; // already in
				}
			}

			FontSettings::Style candidate = FontSettings::DetectStyle(face);
			bool                taken = false;
			for (const FontSettings::Style& style : fontSettings._styles)
			{
				taken |= style._font.GetUid().IsValid() && style._weight == candidate._weight && style._italic == candidate._italic;
			}
			for (const FontSettings::Style& style : found)
			{
				taken |= style._weight == candidate._weight && style._italic == candidate._italic;
			}
			if (taken)
			{
				skipped += skipped.Empty() ? candidateAsset.GetName() : String(", ") + candidateAsset.GetName();
				return;
			}

			candidate._font.SetUid(candidateAsset.GetUid());
			found.PushBack(candidate);
		};

		add(*asset, font->GetFontFace()); // first: the one kept when another one says the same style
		for (const AssetDatabase::FileSystemMapping* sibling : node->_parentFolder->_childrenAsset)
		{
			const std::shared_ptr<Asset>& siblingAsset = sibling->_asset;
			if (siblingAsset == nullptr || siblingAsset->GetType() != asset->GetType() || siblingAsset->GetUid() == asset->GetUid())
			{
				continue;
			}

			// Loaded to read its face: only the ones of the folder, when asked
			std::shared_ptr<FontResource> siblingFont = ResourceManager::GetInstance()->GetResource<FontResource>(siblingAsset->GetUid());
			if (siblingFont != nullptr && siblingFont->GetFontFace().IsLoaded() && siblingFont->GetFontFace().GetFamilyName() == family &&
			    siblingFont->GetFontFace().GetWidth() == width)
			{
				add(*siblingAsset, siblingFont->GetFontFace());
			}
		}

		std::sort(found.begin(), found.end(),
		          [](const FontSettings::Style& left, const FontSettings::Style& right)
		          {
			          return left._italic != right._italic ? right._italic : std::to_underlying(left._weight) < std::to_underlying(right._weight);
		          });

		// After the ones there
		for (const FontSettings::Style& style : found)
		{
			fontSettings._styles.PushBack(style);
		}

		_detectStylesMessage = found.Empty() ? String::Format("Nothing new of the family \"%s\" in its folder", family.CStr()) : String::Format("%u added", found.Size());
		if (skipped.Empty() == false)
		{
			_detectStylesMessage += String::Format(". Left out, their style is taken: %s", skipped.CStr());
		}
		return found.Empty() == false;
	}

	/// @brief The grid the outlines and advances of the glyphs are on, what was found shown under the pixel size
	/// @return
	bool FontEditorPropertiesWindow::DetectPixelSize()
	{
		FontEditorTab*                fontEditorTab = GetOwner<FontEditorTab>();
		std::shared_ptr<FontResource> font = fontEditorTab->GetFont();
		const uint32_t                pixelSize = font != nullptr ? font->GetFontFace().DetectPixelGrid() : 0;
		if (pixelSize == 0)
		{
			_detectMessage = "No pixel grid found: not a pixel art font?";
			return false;
		}

		fontEditorTab->GetFontSettings()._pixelSize = pixelSize;
		_detectMessage = String::Format("A grid of %u pixels an em", pixelSize);
		return true;
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
