#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/AudioEditor/AudioEditorPropertiesWindow.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/ImGui/ImGuiManager.hpp>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/Helper.hpp>

#include <HodEngine/Core/Serialization/Serializer.hpp>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(AudioEditorPropertiesWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief 
	/// @param editorTab 
	AudioEditorPropertiesWindow::AudioEditorPropertiesWindow(EditorTab* editorTab)
	: EditorTabWindow(editorTab)
	{

	}

	/// @brief 
	void AudioEditorPropertiesWindow::DrawContent()
	{
		/*
		bool changed = false;
		std::shared_ptr<Asset> asset = GetOwner()->GetAsset();
		SerializedDataAsset& serializedDataAsset = GetOwner<SerializedDataEditorTab>()->GetSerializedDataAsset();

		if (ImGui::CollapsingHeader("Data", ImGuiTreeNodeFlags_DefaultOpen))
		{
			EditorReflectedObject reflectedObject(serializedDataAsset.GetData(), &serializedDataAsset.GetData()->GetReflectionDescriptorV(), nullptr, this);
			changed |= PropertyDrawer::DrawDescriptor(reflectedObject);
		}

		ImGui::BeginDisabled(asset->IsDirty() == false);
		if (ImGui::Button("Apply"))
		{
			asset->Save(&serializedDataAsset, &serializedDataAsset.GetReflectionDescriptorV());
			AssetDatabase::GetInstance()->Import(asset->GetPath());
		}
		ImGui::EndDisabled();

		if (changed)
		{
			GetOwner()->MarkAssetAsDirty();
		}
		*/
	}
}
