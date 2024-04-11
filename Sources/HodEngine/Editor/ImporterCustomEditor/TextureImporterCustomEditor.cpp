#include "HodEngine/Editor/ImporterCustomEditor/TextureImporterCustomEditor.hpp"
#include "HodEngine/Editor/Importer/TextureImporter.hpp"
#include "HodEngine/Editor/Asset.hpp"

#include "HodEngine/ImGui/DearImGui/imgui.h"

#include "HodEngine/Renderer/Resource/TextureResource.hpp"

#include "HodEngine/Core/ResourceManager.hpp"
#include "HodEngine/Renderer/Enums.hpp"

namespace hod::editor
{
	/// @brief 
	bool TextureImporterCustomEditor::OnDrawInspector(std::shared_ptr<Asset> asset)
	{
		bool changed = false;

		std::shared_ptr<renderer::TextureResource> texture = ResourceManager::GetInstance()->GetResource<renderer::TextureResource>(asset->GetMeta()._uid);

		TextureImporterSettings* textureImporterSettings = static_cast<TextureImporterSettings*>(asset->GetMeta()._importerSettings);

		ImGui::TextUnformatted("Texture");
		ImGui::Spacing();
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Generate mipmap");
		ImGui::SameLine();
		changed |= ImGui::Checkbox("##Generate mipmap", &textureImporterSettings->_generateMipmap);

		static const char* filterModeLabels[static_cast<std::underlying_type_t<renderer::FilterMode>>(renderer::FilterMode::Count)] = { "Nearest", "Linear" };

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Filter Mode");
		ImGui::SameLine();
		if (ImGui::BeginCombo("##FilterMode", filterModeLabels[static_cast<std::underlying_type_t<renderer::FilterMode>>(textureImporterSettings->_filterMode)]))
		{
			for (uint32_t filterMode = 0; filterMode < static_cast<std::underlying_type_t<renderer::FilterMode>>(renderer::FilterMode::Count); ++filterMode)
			{
				if (ImGui::MenuItem(filterModeLabels[filterMode]))
				{
					textureImporterSettings->_filterMode = static_cast<renderer::FilterMode>(filterMode);
					changed = true;
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndCombo();
		}

		static const char* wrapModeLabels[static_cast<std::underlying_type_t<renderer::WrapMode>>(renderer::WrapMode::Count)] = { "Clamp", "Repeat" };

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Wrap Mode");
		ImGui::SameLine();
		if (ImGui::BeginCombo("##WrapMode", wrapModeLabels[static_cast<std::underlying_type_t<renderer::WrapMode>>(textureImporterSettings->_wrapMode)]))
		{
			for (uint32_t wrapMode = 0; wrapMode < static_cast<std::underlying_type_t<renderer::WrapMode>>(renderer::WrapMode::Count); ++wrapMode)
			{
				if (ImGui::MenuItem(wrapModeLabels[wrapMode]))
				{
					textureImporterSettings->_wrapMode = static_cast<renderer::WrapMode>(wrapMode);
					changed = true;
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndCombo();
		}

		float width = ImGui::GetContentRegionAvail().x;
		float height = width / (texture->GetTexture()->GetWidth() / texture->GetTexture()->GetHeight());
		ImGui::Image(texture->GetTexture(), ImVec2(width, height));
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::TextUnformatted("Sprite");
		if (ImGui::Button("Edit"))
		{

		}
		ImGui::Spacing();

		return changed;
	}
}
