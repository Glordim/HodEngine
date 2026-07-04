#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"
#include "HodEngine/Editor/AssetContainer.hpp"
#include "HodEngine/Editor/TextureEditor/TextureEditorPropertiesWindow.hpp"

#include "HodEngine/Editor/EditorTab.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/ImGui/ImGuiManager.hpp>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/Helper.hpp>

#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Asset.hpp"

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
		std::shared_ptr<Asset> asset = GetOwner()->GetAsset();
		AssetContainer assetContainer;
		if (assetContainer.Load(asset->GetPath()) == false)
		{
			return;
		}

		const AssetContainer::DataBlockInfo* settingsDataBlock = assetContainer.FindDataBlock("Settings");
		if (settingsDataBlock != nullptr)
		{
			Document document;
			DocumentReaderJson documentReader;
			documentReader.Read(document, *settingsDataBlock->_stream);
			Serializer::Deserialize(_textureSettings, document.GetRootNode());
		}
	}

	/// @brief 
	void TextureEditorPropertiesWindow::DrawContent()
	{
		bool changed = false;

		float valuePos = ImGui::GetContentRegionAvail().x * 0.4f;

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Generate mipmap");
		ImGui::SameLine(valuePos);
		ImGui::SetNextItemWidth(-1);
		changed |= ImGui::Checkbox("##Generate mipmap", &_textureSettings._generateMipmap);

		static const char* filterModeLabels[static_cast<std::underlying_type_t<FilterMode>>(FilterMode::Count)] = { "Nearest", "Linear" };

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Filter Mode");
		ImGui::SameLine(valuePos);
		ImGui::SetNextItemWidth(-1);
		if (ImGui::BeginCombo("##FilterMode", filterModeLabels[static_cast<std::underlying_type_t<FilterMode>>(_textureSettings._filterMode)]))
		{
			for (uint32_t filterMode = 0; filterMode < static_cast<std::underlying_type_t<FilterMode>>(FilterMode::Count); ++filterMode)
			{
				if (ImGui::MenuItem(filterModeLabels[filterMode]))
				{
					_textureSettings._filterMode = static_cast<FilterMode>(filterMode);
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
		if (ImGui::BeginCombo("##WrapMode", wrapModeLabels[static_cast<std::underlying_type_t<WrapMode>>(_textureSettings._wrapMode)]))
		{
			for (uint32_t wrapMode = 0; wrapMode < static_cast<std::underlying_type_t<WrapMode>>(WrapMode::Count); ++wrapMode)
			{
				if (ImGui::MenuItem(wrapModeLabels[wrapMode]))
				{
					_textureSettings._wrapMode = static_cast<WrapMode>(wrapMode);
					changed = true;
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndCombo();
		}
		std::shared_ptr<Asset> asset = GetOwner()->GetAsset();
		ImGui::BeginDisabled(asset->IsDirty() == false);
		if (ImGui::Button("Apply"))
		{
			asset->Save();
			AssetDatabase::GetInstance()->Import(asset->GetPath());
		}
		ImGui::EndDisabled();

		if (changed)
		{
			GetOwner()->MarkAssetAsDirty();
		}
	}
}
