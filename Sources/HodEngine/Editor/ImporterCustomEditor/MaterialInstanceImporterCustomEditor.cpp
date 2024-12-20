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
				// todo check _materialInstanceAsset._params and _paramsBuffer size ?
				Serializer::Deserialize(&material->GetReflectionDescriptorForParameters(), static_cast<void*>(_paramsBuffer), _materialInstanceAsset._params.GetRootNode());
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
					EditorReflectedObject reflectedObject(static_cast<void*>(_paramsBuffer), &material->GetReflectionDescriptorForParameters());
					changed |= PropertyDrawer::DrawDescriptor(reflectedObject);
				}
			}
		}

		ImGui::BeginDisabled(_asset->IsDirty() == false);
		if (ImGui::Button("Apply"))
		{
			std::shared_ptr<renderer::MaterialResource> materialResource = _materialInstanceAsset._material.Lock();
			if (materialResource != nullptr)
			{
				renderer::Material* material = materialResource->GetMaterial();
				if (material != nullptr)
				{
					_materialInstanceAsset._params.GetRootNode().Clear();
					Serializer::Serialize(&material->GetReflectionDescriptorForParameters(), static_cast<void*>(_paramsBuffer), _materialInstanceAsset._params.GetRootNode());
					_asset->Save(&_materialInstanceAsset, _materialInstanceAsset.GetReflectionDescriptorV());
					AssetDatabase::GetInstance()->Import(_asset->GetPath());
				}
			}
		}
		ImGui::EndDisabled();

		return changed;
	}
}
