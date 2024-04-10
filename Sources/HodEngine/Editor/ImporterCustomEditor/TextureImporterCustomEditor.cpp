#include "HodEngine/Editor/ImporterCustomEditor/TextureImporterCustomEditor.hpp"
#include "HodEngine/Editor/Importer/TextureImporter.hpp"
#include "HodEngine/Editor/Asset.hpp"

#include "HodEngine/ImGui/DearImGui/imgui.h"

#include "HodEngine/Renderer/Resource/TextureResource.hpp"

#include "HodEngine/Core/ResourceManager.hpp"

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

		static const char* filteringLabel[renderer::TextureResource::Filtering::Count] = { "Nearest", "Linear" };

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Filtering");
		ImGui::SameLine();
		if (ImGui::BeginCombo("##Filtering", filteringLabel[textureImporterSettings->_filtering]))
		{
			for (uint32_t filtering = renderer::TextureResource::Nearest; filtering < renderer::TextureResource::Count; ++filtering)
			{
				if (ImGui::MenuItem(filteringLabel[filtering]))
				{
					textureImporterSettings->_filtering = (renderer::TextureResource::Filtering)filtering;
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
