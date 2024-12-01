#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/ImporterCustomEditor/MaterialInstanceImporterCustomEditor.hpp"
#include "HodEngine/Editor/Importer/SerializedDataImporter.hpp"
#include "HodEngine/Editor/Importer/SerializedDataAsset.hpp"
#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"

#include "HodEngine/ImGui/DearImGui/imgui.h"

#include "HodEngine/Game/SerializedDataResource.hpp"
#include "HodEngine/Game/SerializedData.hpp"

#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"
#include "HodEngine/Core/Resource/ResourceManager.hpp"
#include "HodEngine/Renderer/Enums.hpp"
#include "HodEngine/Renderer/RHI/ShaderSetDescriptor.hpp"

namespace hod::editor
{
	/// @brief 
	/// @param asset 
	void MaterialInstanceImporterCustomEditor::OnInit(std::shared_ptr<Asset> asset)
	{
		ImporterCustomEditor::OnInit(asset);

		Document document;
		DocumentReaderJson reader;
		reader.Read(document, asset->GetPath());

		Serializer::Deserialize(_materialInstanceAsset, document.GetRootNode());
	}

	/// @brief 
	/// @param member 
	/// @return 
	bool DrawUboMember(const renderer::ShaderSetDescriptor::BlockUbo::Member& member)
	{
		bool changed = false;

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(member._name.c_str());
		ImGui::Indent();
		{
			for (const auto& childPair : member._childsMap)
			{
				changed |= DrawUboMember(childPair.second);
			}
		}
		ImGui::Unindent();

		return changed;
	}

	/// @brief 
	bool MaterialInstanceImporterCustomEditor::OnDrawInspector()
	{
		bool changed = false;

		if (ImGui::CollapsingHeader("Data", ImGuiTreeNodeFlags_DefaultOpen))
		{
			EditorReflectedObject reflectedObject(&_materialInstanceAsset, _materialInstanceAsset.GetReflectionDescriptorV());
			changed |= PropertyDrawer::DrawDescriptor(reflectedObject);
		}

		if (ImGui::CollapsingHeader("Params", ImGuiTreeNodeFlags_DefaultOpen))
		{
			std::shared_ptr<renderer::MaterialResource> materialResource = _materialInstanceAsset._material.Lock();
			if (materialResource != nullptr)
			{
				renderer::Material* material = materialResource->GetMaterial();
				if (material != nullptr)
				{
					const std::unordered_map<uint32_t, renderer::ShaderSetDescriptor*> setDescriptors = material->GetSetDescriptors();
					for (const auto& pair : setDescriptors)
					{
						for (const renderer::ShaderSetDescriptor::BlockUbo& ubo : pair.second->GetUboBlocks())
						{
							ImGui::AlignTextToFramePadding();
							ImGui::TextUnformatted(ubo._name.c_str());
							ImGui::Indent();
							{
								for (const auto& childPair : ubo._rootMember._childsMap)
								{
									changed |= DrawUboMember(childPair.second);
								}
							}
							ImGui::Unindent();
						}

						for (const renderer::ShaderSetDescriptor::BlockTexture& texture : pair.second->GetTextureBlocks())
						{
							if (texture._type == renderer::ShaderSetDescriptor::BlockTexture::Type::Texture)
							{
								//WeakResourceCustomEditor
							}
						}
					}
				}
			}
		}

		ImGui::BeginDisabled(_asset->IsDirty() == false);
		if (ImGui::Button("Apply"))
		{
			_asset->Save(&_materialInstanceAsset, _materialInstanceAsset.GetReflectionDescriptorV());
			AssetDatabase::GetInstance()->Import(_asset->GetPath());
		}
		ImGui::EndDisabled();

		return changed;
	}
}
