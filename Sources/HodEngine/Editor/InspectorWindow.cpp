#include "HodEngine/Editor/InspectorWindow.h"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/ImGui/ImGuiManager.h>

#include "HodEngine/Editor/Editor.h"
#include "HodEngine/Editor/Asset.h"
#include "HodEngine/Editor/PropertyDrawer.h"

#include "HodEngine/Game/src/Actor.h"
#include "HodEngine/Game/Entity.h"
#include "HodEngine/Game/src/Components/NodeComponent.h"

namespace hod::editor
{
	DECLARE_WINDOW_DESCRIPTION(InspectorWindow, "Inspector", true)

	/// @brief 
	void InspectorWindow::Draw()
	{
		Editor* editor = Editor::GetInstance();
		std::shared_ptr<game::Entity> sceneSelection = editor->GetEntitySelection().lock();
		if (sceneSelection != nullptr)
		{
			DrawSceneSelection(sceneSelection.get());
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
			if (DrawDefaultInspector(*asset->GetMeta()._importerSettings) == true)
			{
				selection->_asset->SetDirty();
			}
		}
	}

	/// @brief 
	/// @param selection 
	void InspectorWindow::DrawSceneSelection(game::Entity* selection)
	{
		ImGui::Text("Actor");

		char buffer[256] = { '\0' };
		std::strcpy(buffer, selection->GetName().c_str());
		if (ImGui::InputText("##Name", buffer, sizeof(buffer) - 1) == true)
		{
			selection->SetName(buffer);
		}
		ImGui::Separator();

		std::vector<std::weak_ptr<game::Component>> components = selection->GetComponents();
		for (const std::weak_ptr<game::Component>& component : components)
		{
			std::shared_ptr<game::Component> componentLock = component.lock();
			if (componentLock != nullptr)
			{
				ImGui::TextUnformatted(componentLock->GetMetaTypeName());

				if (DrawDefaultInspector(*componentLock.get()) == true)
				{
					//selection->_asset->SetDirty(); // TODO
				}
			}
		}

		ImGui::Separator();

		if (ImGui::Button("Add component") == true)
		{
			selection->AddComponent<game::NodeComponent>();
		}
	}

	/// @brief 
	/// @param object 
	bool InspectorWindow::DrawDefaultInspector(Object& object)
	{
		core::ReflectionDescriptor* reflectionDescriptor = object.GetReflectionDescriptorV();

		bool changed = false;

		for (core::ReflectionProperty* property : reflectionDescriptor->GetProperties())
		{
			changed |= PropertyDrawer::DrawProperty(object, property);
		}

		return changed;
	}
}
