#include "HodEngine/Editor/ImporterCustomEditor/TextureImporterCustomEditor.hpp"
#include "HodEngine/Editor/Importer/TextureImporter.hpp"
#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Editor.hpp"

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

		float valuePos = ImGui::GetContentRegionAvail().x * 0.4f;

		if (ImGui::CollapsingHeader("Texture", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted("Generate mipmap");
			ImGui::SameLine(valuePos);
			ImGui::SetNextItemWidth(-1);
			changed |= ImGui::Checkbox("##Generate mipmap", &textureImporterSettings->_generateMipmap);

			static const char* filterModeLabels[static_cast<std::underlying_type_t<renderer::FilterMode>>(renderer::FilterMode::Count)] = { "Nearest", "Linear" };

			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted("Filter Mode");
			ImGui::SameLine(valuePos);
			ImGui::SetNextItemWidth(-1);
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
			ImGui::SameLine(valuePos);
			ImGui::SetNextItemWidth(-1);
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
			float height = width / (static_cast<float>(texture->GetTexture()->GetWidth()) / static_cast<float>(texture->GetTexture()->GetHeight()));
			ImVec2 cursor = ImGui::GetCursorPos();
			ImGui::Image(Editor::GetInstance()->GetCheckerTexture(), ImVec2(width, height), ImVec2(0, 0), ImVec2(width / (4 * 8), height / (4 * 8)));
			ImGui::SetCursorPos(cursor);
			ImGui::Image(texture->GetTexture(), ImVec2(width, height));
			ImGui::Spacing();

			ImGui::BeginDisabled(asset->IsDirty() == false);
			if (ImGui::Button("Apply"))
			{
				asset->Save();
				AssetDatabase::GetInstance()->Import(asset->GetPath());
			}
			ImGui::EndDisabled();
		}

		if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::Button("Edit"))
			{

			}
		}

		return changed;
	}
}
