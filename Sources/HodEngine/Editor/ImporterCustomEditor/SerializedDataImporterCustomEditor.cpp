#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/ImporterCustomEditor/SerializedDataImporterCustomEditor.hpp"
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
#include "HodEngine/Core/ResourceManager.hpp"
#include "HodEngine/Renderer/Enums.hpp"

namespace hod::editor
{
	/// @brief 
	/// @param asset 
	void SerializedDataImporterCustomEditor::OnInit(std::shared_ptr<Asset> asset)
	{
		ImporterCustomEditor::OnInit(asset);

		Document document;
		DocumentReaderJson reader;
		reader.Read(document, asset->GetPath());

		Serializer::Deserialize(_serializedDataAsset, document.GetRootNode());
	}

	/// @brief 
	bool SerializedDataImporterCustomEditor::OnDrawInspector()
	{
		bool changed = false;

		if (ImGui::CollapsingHeader("Data", ImGuiTreeNodeFlags_DefaultOpen))
		{
			EditorReflectedObject reflectedObject(_serializedDataAsset.GetData(), _serializedDataAsset.GetData()->GetReflectionDescriptorV());
			changed |= PropertyDrawer::DrawDescriptor(reflectedObject);
		}

		ImGui::BeginDisabled(_asset->IsDirty() == false);
		if (ImGui::Button("Apply"))
		{
			_asset->Save(&_serializedDataAsset, _serializedDataAsset.GetReflectionDescriptorV());
			AssetDatabase::GetInstance()->Import(_asset->GetPath());
		}
		ImGui::EndDisabled();

		return changed;
	}
}
