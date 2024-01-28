#include "WeakResourceCustomEditor.hpp"

#include "HodEngine/Core/Math/Vector2.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp"

#include "HodEngine/ImGui/DearImGui/imgui.h"

#include "HodEngine/Game/WeakResource.hpp"

#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Asset.hpp"

namespace hod::editor
{
	/// @brief 
	/// @param instance 
	/// @return 
	bool WeakResourceCustomEditor::Draw(void* instance, ReflectionPropertyObject* property)
	{
		static std::vector<AssetDatabase::FileSystemMapping*> assetList;

		bool changed = false;

		game::WeakResourceBase* value = static_cast<game::WeakResourceBase*>(property->GetValue(instance));

		ImGui::PushID(value);
		if (ImGui::BeginTable("Var", 2) == true)
		{
			ImGui::TableNextRow();

			ImGui::TableNextColumn();
			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted(property->GetDisplayName().c_str());
			
			ImGui::TableNextColumn();			
			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted(value->GetUid().ToString().c_str());
			ImGui::SameLine();
			if (ImGui::Button("Find") == true)
			{
				AssetDatabase* assetDatabase = AssetDatabase::GetInstance();
				assetList.clear();
				assetDatabase->ListAsset(assetList, assetDatabase->GetAssetRootNode(), value->GetResourceDescriptor());

				ImGui::OpenPopup("WeakResourceFindPopup");
			}
			if (ImGui::BeginPopup("WeakResourceFindPopup"))
			{
				ImGui::AlignTextToFramePadding();
				ImGui::TextUnformatted("Name:");
				ImGui::SameLine();

				static char inputTextBuffer[2048] = "";
				ImGui::InputText("##Name", inputTextBuffer, sizeof(inputTextBuffer));
				ImGui::Separator();

				float itemHeight = 32; // todo max 32 - FontSize ?

				for (AssetDatabase::FileSystemMapping* assetNode : assetList)
				{
					ImGui::PushID(assetNode);
					ImGui::Image(assetNode->_asset->GetThumbnail(), ImVec2(itemHeight, itemHeight));
					ImGui::SameLine();
					ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0, 0.5f));
					if (ImGui::Selectable(assetNode->_asset->GetName().c_str(), false, 0, ImVec2(0, itemHeight)))
					{
						value->SetUid(assetNode->_asset->GetUid());
					}
					ImGui::PopStyleVar();
					ImGui::PopID();
				}

				ImGui::EndPopup();
			}

			ImGui::EndTable();
		}

		ImGui::PopID();
		return changed;
	}
}
