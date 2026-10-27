#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/FontEditor/FontEditorPropertiesWindow.hpp"

#include "HodEngine/Editor/FontEditor/FontCooker.hpp"
#include "HodEngine/Editor/FontEditor/FontEditorTab.hpp"
#include "HodEngine/Editor/FontEditor/FontHelper.hpp"

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"

#include <HodEngine/Core/Reflection/EnumDescriptor.hpp>
#include <HodEngine/Core/Reflection/ReflectionHelper.hpp>
#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <HodEngine/Renderer/Resource/FontFaceResource.hpp>
#include <HodEngine/Renderer/Resource/FontResource.hpp>
#include <HodEngine/Text/FontFace.hpp>

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
		if (ImGui::CollapsingHeader("Styles", ImGuiTreeNodeFlags_DefaultOpen))
		{
			DrawStyles();
		}
		if (ImGui::CollapsingHeader("Fallbacks", ImGuiTreeNodeFlags_DefaultOpen))
		{
			DrawFallbacks();
		}
		DrawChain();
	}

	/// @brief A combo of the weights, "Bold (700)"
	/// @param id
	/// @param weight
	/// @return true when another one is picked
	static bool DrawWeightCombo(const char* id, FontResource::Weight& weight)
	{
		const EnumDescriptor& enumDescriptor = ReflectedEnum<FontResource::Weight>::GetEnumDescriptor();

		bool         changed = false;
		const String preview = String::Format("%s (%u)", String(enumDescriptor.ToString(weight)).CStr(), (uint32_t)std::to_underlying(weight));
		if (ImGui::BeginCombo(id, preview.CStr()))
		{
			for (uint16_t value = 100; value <= 900; value += 100)
			{
				const FontResource::Weight item = static_cast<FontResource::Weight>(value);
				const String               label = String::Format("%s (%u)", String(enumDescriptor.ToString(item)).CStr(), (uint32_t)value);
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

	/// @brief The face drawn for each style: a text asking for a style is drawn with the closest. The same face for several, as wished.
	/// Applied as they are edited
	void FontEditorPropertiesWindow::DrawStyles()
	{
		FontEditorTab* fontEditorTab = GetOwner<FontEditorTab>();
		FontSettings&  fontSettings = fontEditorTab->GetFontSettings();
		bool           changed = false;

		ImGui::BeginDisabled();
		ImGui::TextWrapped("A text asking for a style is drawn with the face of the closest of these (the italic first, then the weight), "
		                   "then with the Regular one for what it lacks.");
		ImGui::EndDisabled();

		uint32_t remove = std::numeric_limits<uint32_t>::max();
		if (fontSettings._styles.Empty() == false &&
		    ImGui::BeginTable("##Styles", 4, ImGuiTableFlags_PadOuterX | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerH))
		{
			ImGui::TableSetupColumn("Weight", ImGuiTableColumnFlags_WidthStretch, 1.0f);
			ImGui::TableSetupColumn("Italic", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Face", ImGuiTableColumnFlags_WidthStretch, 2.0f);
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
				EditorReflectedProperty* faceProperty = styleObject.FindProperty("Face");
				if (faceProperty != nullptr)
				{
					changed |= PropertyDrawer::DrawPropertyObject(*faceProperty, true);
				}

				// What the file says, when it is not the style it is drawn for: not wrong (a Bold drawn for Black...), to know
				std::shared_ptr<FontFaceResource> face = style._face.Lock();
				if (face != nullptr && face->GetFontFace().IsLoaded())
				{
					const FontSettings::Style fileStyle = FontHelper::DetectStyle(face->GetFontFace());
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
		ImGui::BeginDisabled(fontSettings._styles.Empty() || fontSettings._styles[0]._face.GetUid().IsValid() == false);
		if (ImGui::Button(ICON_MDI_MAGNIFY " Detect"))
		{
			std::shared_ptr<Asset> faceAsset = AssetDatabase::GetInstance()->Find(fontSettings._styles[0]._face.GetUid());
			changed |= FontHelper::DetectStyles(fontSettings, faceAsset, _detectMessage) != 0;
		}
		ImGui::EndDisabled();
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip | ImGuiHoveredFlags_AllowWhenDisabled))
		{
			ImGui::SetTooltip("Adds the faces of the folder of the first one of its family (and width), each for the style its file says it is.\n"
			                  "The ones already there are left as they are.");
		}
		if (_detectMessage.Empty() == false)
		{
			ImGui::BeginDisabled();
			ImGui::TextWrapped("%s", _detectMessage.CStr());
			ImGui::EndDisabled();
		}

		if (changed)
		{
			fontEditorTab->ApplySettings();
			fontEditorTab->MarkAsDirty();
		}
	}

	/// @brief The fonts the characters its faces lack are drawn with, in order: moved, removed, added. Applied as they are edited
	void FontEditorPropertiesWindow::DrawFallbacks()
	{
		FontEditorTab* fontEditorTab = GetOwner<FontEditorTab>();
		FontSettings&  fontSettings = fontEditorTab->GetFontSettings();
		bool           changed = false;

		ImGui::BeginDisabled();
		ImGui::TextWrapped("The characters its faces lack are drawn with the first of these that has them, in the style asked for (from their own styles).");
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

				const UID&             uid = fontSettings._fallbacks[index].GetUid();
				std::shared_ptr<Asset> asset = fontEditorTab->GetAsset();
				if (asset != nullptr && uid == asset->GetUid())
				{
					ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), ICON_MDI_ALERT " The font itself: left out");
				}
				else if (uid.IsValid())
				{
					for (uint32_t previous = 0; previous < index; ++previous)
					{
						if (fontSettings._fallbacks[previous].GetUid() == uid)
						{
							ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), ICON_MDI_ALERT " Already fallback %u: left out", previous + 1);
							break;
						}
					}
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
	}

	/// @brief The faces the preview is drawn with, in its style: the ones of the fallbacks included, each once
	void FontEditorPropertiesWindow::DrawChain()
	{
		GetOwner<FontEditorTab>()->UpdateFontChain(_fontChain);

		String chain;
		for (const std::shared_ptr<FontFaceResource>& face : _fontChain.GetFaceResources())
		{
			std::shared_ptr<Asset> asset = AssetDatabase::GetInstance()->Find(face->GetUid());
			if (chain.Empty() == false)
			{
				chain += "  " ICON_MDI_ARROW_RIGHT "  ";
			}
			chain += asset != nullptr ? asset->GetName() : face->GetUid().ToString();
		}

		ImGui::Spacing();
		ImGui::BeginDisabled();
		ImGui::TextWrapped("The preview is drawn with: %s", chain.Empty() ? "nothing (no face set)" : chain.CStr());
		ImGui::EndDisabled();
	}
}
