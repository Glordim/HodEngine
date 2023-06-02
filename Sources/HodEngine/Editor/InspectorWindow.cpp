#include "HodEngine/Editor/InspectorWindow.h"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/ImGui/ImGuiManager.h>

#include "HodEngine/Editor/Editor.h"
#include "HodEngine/Editor/Asset.h"
#include "HodEngine/Editor/PropertyDrawer.h"

namespace hod::editor
{
	DECLARE_WINDOW_DESCRIPTION(InspectorWindow, "Inspector", true)

	/// @brief 
	void InspectorWindow::Draw()
	{
		Editor* editor = Editor::GetInstance();
		void* sceneSelection = editor->GetSceneSelection();
		if (sceneSelection != nullptr)
		{
			DrawSceneSelection(sceneSelection);
		}
		else
		{
			const AssetDatabase::FileSystemMapping* assetSelection = editor->GetAssetSelection();
			if (assetSelection != nullptr)
			{
				DrawAssetSelection(assetSelection);
			}
		}
	}

	/// @brief 
	/// @param selection 
	void InspectorWindow::DrawAssetSelection(const AssetDatabase::FileSystemMapping* selection)
	{
		ImGui::Text("Asset");

		if (selection->_type == AssetDatabase::FileSystemMapping::Type::AssetType)
		{
			std::shared_ptr<Asset> asset = selection->_asset;
			ImGui::Text(asset->GetName().c_str()); // same line add button to open (and later VersionControl)
			ImGui::Separator();
			DrawDefaultInspector(*asset->GetMeta()._importerSettings);
			//asset-> // TODO display ImporterSettings
		}
	}

	/// @brief 
	/// @param selection 
	void InspectorWindow::DrawSceneSelection(void* selection)
	{
		ImGui::Text("Scene");
	}

	/// @brief 
	/// @param object 
	void InspectorWindow::DrawDefaultInspector(Object& object)
	{
		core::ReflectionDescriptor* reflectionDescriptor = object.GetReflectionDescriptorV();

		for (core::ReflectionProperty* property : reflectionDescriptor->GetProperties())
		{
			PropertyDrawer::DrawProperty(property);
		}
	}
}
