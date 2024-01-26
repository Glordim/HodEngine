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
		changed |= ImGui::Checkbox("Generate mipmap", &textureImporterSettings->_generateMipmap);
		ImGui::Image(texture->GetTexture(), ImVec2(256, 256));
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
