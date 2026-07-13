#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/TextureEditor/TextureEditorPropertiesWindow.hpp"

#include "HodEngine/Editor/EditorTab.hpp"
#include "HodEngine/Editor/TextureEditor/TextureCooker.hpp"
#include "HodEngine/Editor/TextureEditor/TextureEditorTab.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/ImGui/ImGuiManager.hpp>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/Helper.hpp>

#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(TextureEditorPropertiesWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief 
	/// @param editorTab 
	TextureEditorPropertiesWindow::TextureEditorPropertiesWindow(EditorTab* editorTab)
	: EditorTabWindow(editorTab)
	{
		SetTitle("Properties");
	}

	/// @brief
	void TextureEditorPropertiesWindow::DrawContent()
	{
		bool changed = false;

		TextureSettings& textureSettings = GetOwner<TextureEditorTab>()->GetTextureSettings();

		float valuePos = ImGui::GetContentRegionAvail().x * 0.4f;

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Generate mipmap");
		ImGui::SameLine(valuePos);
		ImGui::SetNextItemWidth(-1);
		changed |= ImGui::Checkbox("##Generate mipmap", &textureSettings._generateMipmap);

		static const char* filterModeLabels[static_cast<std::underlying_type_t<FilterMode>>(FilterMode::Count)] = { "Nearest", "Linear" };

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Filter Mode");
		ImGui::SameLine(valuePos);
		ImGui::SetNextItemWidth(-1);
		if (ImGui::BeginCombo("##FilterMode", filterModeLabels[static_cast<std::underlying_type_t<FilterMode>>(textureSettings._filterMode)]))
		{
			for (uint32_t filterMode = 0; filterMode < static_cast<std::underlying_type_t<FilterMode>>(FilterMode::Count); ++filterMode)
			{
				if (ImGui::MenuItem(filterModeLabels[filterMode]))
				{
					textureSettings._filterMode = static_cast<FilterMode>(filterMode);
					changed = true;
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndCombo();
		}

		static const char* wrapModeLabels[static_cast<std::underlying_type_t<WrapMode>>(WrapMode::Count)] = { "Clamp", "Repeat" };

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Wrap Mode");
		ImGui::SameLine(valuePos);
		ImGui::SetNextItemWidth(-1);
		if (ImGui::BeginCombo("##WrapMode", wrapModeLabels[static_cast<std::underlying_type_t<WrapMode>>(textureSettings._wrapMode)]))
		{
			for (uint32_t wrapMode = 0; wrapMode < static_cast<std::underlying_type_t<WrapMode>>(WrapMode::Count); ++wrapMode)
			{
				if (ImGui::MenuItem(wrapModeLabels[wrapMode]))
				{
					textureSettings._wrapMode = static_cast<WrapMode>(wrapMode);
					changed = true;
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndCombo();
		}

		if (changed)
		{
			GetOwner()->MarkAsDirty();
		}
	}
}
