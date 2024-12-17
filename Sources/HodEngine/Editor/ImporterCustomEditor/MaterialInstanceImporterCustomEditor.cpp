#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/ImporterCustomEditor/MaterialInstanceImporterCustomEditor.hpp"
#include "HodEngine/Editor/Importer/SerializedDataImporter.hpp"
#include "HodEngine/Editor/Importer/SerializedDataAsset.hpp"
#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"

#include "HodEngine/ImGui/DearImGui/imgui.h"

#include "HodEngine/Game/SerializedDataResource.hpp"
#include "HodEngine/Game/SerializedData.hpp"

#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"
#include "HodEngine/Core/Resource/ResourceManager.hpp"
#include "HodEngine/Renderer/Enums.hpp"
#include "HodEngine/Renderer/RHI/ShaderSetDescriptor.hpp"
#include "HodEngine/Renderer/Resource/TextureResource.hpp"

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

		std::memset(_paramsBuffer, 0, sizeof(_paramsBuffer));

		std::shared_ptr<renderer::MaterialResource> materialResource = _materialInstanceAsset._material.Lock();
		if (materialResource != nullptr)
		{
			renderer::Material* material = materialResource->GetMaterial();
			if (material != nullptr)
			{
				uint32_t offset = 0;
				_reflectionDescriptor = ReflectionDescriptor();

				const std::unordered_map<uint32_t, renderer::ShaderSetDescriptor*> setDescriptors = material->GetSetDescriptors();

				for (const auto& pair : setDescriptors)
				{
					for (const renderer::ShaderSetDescriptor::BlockUbo& ubo : pair.second->GetUboBlocks())
					{
						uint32_t uboOffset = 0;
						ReflectionDescriptor* uboReflectionDescriptor = new ReflectionDescriptor();
						for (const auto& childPair : ubo._rootMember._childsMap)
						{
							if (childPair.second._memberType == renderer::ShaderSetDescriptor::BlockUbo::MemberType::Float)
							{
								uboReflectionDescriptor->AddProperty<ReflectionPropertyVariable>(ReflectionPropertyVariable::Type::Float32, uboOffset, childPair.second._name.c_str(), nullptr, nullptr);
								*reinterpret_cast<float*>(_paramsBuffer + offset + uboOffset) = 0.0f;
								uboOffset += 1 * sizeof(float);
							}
							else if (childPair.second._memberType == renderer::ShaderSetDescriptor::BlockUbo::MemberType::Float2)
							{
								uboReflectionDescriptor->AddProperty<ReflectionPropertyObject>(uboOffset, childPair.second._name.c_str(), Vector2::GetReflectionDescriptor(), nullptr, nullptr);
								*reinterpret_cast<Vector2*>(_paramsBuffer + offset + uboOffset) = Vector2();
								uboOffset += 2 * sizeof(float);
							}
						}
						_reflectionDescriptor.AddProperty<ReflectionPropertyObject>(offset, ubo._name.c_str(), uboReflectionDescriptor, nullptr, nullptr);
						offset += uboOffset;
					}

					for (const renderer::ShaderSetDescriptor::BlockTexture& texture : pair.second->GetTextureBlocks())
					{
						if (texture._type == renderer::ShaderSetDescriptor::BlockTexture::Texture)
						{
							_reflectionDescriptor.AddProperty<ReflectionPropertyObject>(offset, texture._name.c_str(), WeakResource<renderer::TextureResource>::GetReflectionDescriptor(), nullptr, nullptr);
							*reinterpret_cast<WeakResource<renderer::TextureResource>*>(_paramsBuffer + offset) = WeakResource<renderer::TextureResource>();
							offset += sizeof(WeakResource<renderer::TextureResource>);
						}
					}
				}

				// todo check _materialInstanceAsset._params and _paramsBuffer size ?
				Serializer::Deserialize(&_reflectionDescriptor, static_cast<void*>(_paramsBuffer), _materialInstanceAsset._params.GetRootNode());
			}
		}
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
					EditorReflectedObject reflectedObject(static_cast<void*>(_paramsBuffer), &_reflectionDescriptor);
					changed |= PropertyDrawer::DrawDescriptor(reflectedObject);
				}
			}
		}

		ImGui::BeginDisabled(_asset->IsDirty() == false);
		if (ImGui::Button("Apply"))
		{
			Serializer::Serialize(&_reflectionDescriptor, static_cast<void*>(_paramsBuffer), _materialInstanceAsset._params.GetRootNode());
			_asset->Save(&_materialInstanceAsset, _materialInstanceAsset.GetReflectionDescriptorV());
			AssetDatabase::GetInstance()->Import(_asset->GetPath());
		}
		ImGui::EndDisabled();

		return changed;
	}
}
