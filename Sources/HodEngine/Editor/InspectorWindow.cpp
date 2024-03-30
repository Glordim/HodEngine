#include "HodEngine/Editor/InspectorWindow.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/ImGui/ImGuiManager.hpp>

#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"

#include "HodEngine/Game/Entity.hpp"
#include "HodEngine/Game/Components/NodeComponent.hpp"

#include "HodEngine/Game/ComponentFactory.hpp"
#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitDisplayName.hpp"
#include "HodEngine/Editor/Trait/ReflectionTraitComponentCustomEditor.hpp"
#include "HodEngine/Editor/Trait/ReflectionTraitImporterCustomEditor.hpp"
#include "HodEngine/Editor/ComponentCustomEditor/ComponentCustomEditor.hpp"
#include "HodEngine/Editor/ImporterCustomEditor/ImporterCustomEditor.hpp"

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
			ImGui::TextUnformatted(asset->GetName().c_str()); // same line add button to open (and later VersionControl)
			ImGui::Separator();
			bool changed = false;
			ReflectionDescriptor* reflectionDescriptor = asset->GetMeta()._importerSettings->GetReflectionDescriptorV();
			ReflectionTraitImporterCustomEditor* componentCustomEditorTrait = reflectionDescriptor->FindTrait<ReflectionTraitImporterCustomEditor>();
			if (componentCustomEditorTrait != nullptr)
			{
				changed = componentCustomEditorTrait->GetCustomEditor()->OnDrawInspector(asset);
			}
			else
			{
				changed = DrawDefaultInspector(asset->GetMeta()._importerSettings, reflectionDescriptor);
			}
			if (changed == true)
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
			Editor::GetInstance()->MarkCurrentSceneAsDirty();
		}
		ImGui::Separator();

		std::vector<std::weak_ptr<game::Component>> components = selection->GetComponents();
		for (const std::weak_ptr<game::Component>& component : components)
		{
			std::shared_ptr<game::Component> componentLock = component.lock();
			if (componentLock != nullptr)
			{
				bool opened = ImGui::CollapsingHeader(componentLock->GetMetaTypeName(), ImGuiTreeNodeFlags_DefaultOpen);
				if (ImGui::BeginPopupContextItem() == true)
				{
					if (ImGui::Button("Delete") == true)
					{
						selection->RemoveComponent(componentLock);
						Editor::GetInstance()->MarkCurrentSceneAsDirty();
					}
					ImGui::EndPopup();
				}

				if (opened == true)
				{
					bool changed = false;
					ReflectionDescriptor* reflectionDescriptor = componentLock->GetReflectionDescriptorV();
					ReflectionTraitComponentCustomEditor* componentCustomEditorTrait = reflectionDescriptor->FindTrait<ReflectionTraitComponentCustomEditor>();
					if (componentCustomEditorTrait != nullptr)
					{
						changed = componentCustomEditorTrait->GetCustomEditor()->OnDrawInspector(componentLock.get(), reflectionDescriptor);
					}
					else
					{
						changed = DrawDefaultInspector(componentLock.get(), reflectionDescriptor);
					}
					if (changed == true)
					{
						Editor::GetInstance()->MarkCurrentSceneAsDirty();
					}
				}
			}
		}

		ImGui::Separator();

		ImVec2 addComponentButtonSize = ImGui::CalcTextSize("  Add component  ") + ImVec2(35.0f, 15.0f);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetContentRegionAvail().x * 0.5f - addComponentButtonSize.x * 0.5f));
		if (ImGui::Button("  Add component  ", addComponentButtonSize) == true)
		{
			ImGui::OpenPopup("AddComponent");
		}
		if (ImGui::BeginPopup("AddComponent") == true)
		{
			for (const auto& componentDescriptorPair : game::ComponentFactory::GetInstance()->GetAllDescriptors())
			{
				const ReflectionDescriptor& componentDescriptor = *componentDescriptorPair.second;

				ReflectionTraitDisplayName* displayNameTrait = componentDescriptor.FindTrait<ReflectionTraitDisplayName>();
				if (ImGui::MenuItem(displayNameTrait->GetValue().c_str()) == true)
				{
					selection->AddComponent(componentDescriptor, Editor::GetInstance()->IsPlaying());
					Editor::GetInstance()->MarkCurrentSceneAsDirty();
				}
			}

			ImGui::EndPopup();
		}
	}

	/// @brief 
	/// @param object 
	bool InspectorWindow::DrawDefaultInspector(void* object, ReflectionDescriptor* reflectionDescriptor)
	{
		return PropertyDrawer::DrawDescriptor(object, reflectionDescriptor);
	}
}
