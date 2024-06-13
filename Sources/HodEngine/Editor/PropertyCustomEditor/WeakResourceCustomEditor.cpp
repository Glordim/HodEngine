#include "WeakResourceCustomEditor.hpp"

#include "HodEngine/Core/Math/Vector2.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp"

#include "HodEngine/ImGui/DearImGui/imgui.h"
#include "HodEngine/ImGui/Helper.hpp"
#include "HodEngine/ImGui/Widgets.hpp"

#include "HodEngine/Game/WeakResource.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"

#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Asset.hpp"

#include "HodEngine/Renderer/RHI/Texture.hpp"

#include "HodEngine/ImGui/ImGuiManager.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/AssetBrowserWindow.hpp"

namespace hod::editor
{
	/// @brief 
	/// @param instance 
	/// @return 
	bool WeakResourceCustomEditor::Draw(EditorReflectedObject& reflectedObject)
	{
		if (reflectedObject.IsOverride() == true)
		{
			float height = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2;
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(ImGui::GetWindowPos().x, ImGui::GetCursorScreenPos().y), ImVec2(ImGui::GetWindowPos().x + 2.0f, ImGui::GetCursorScreenPos().y + height), IM_COL32(0, 170, 255, 255));
		}

		static std::vector<AssetDatabase::FileSystemMapping*> assetList;

		bool changed = false;

		game::WeakResourceBase* value = static_cast<game::WeakResourceBase*>(reflectedObject.GetInstance());

		ImGui::PushID(value);

		float valuePos = ImGui::GetContentRegionAvail().x * 0.4f;

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(reflectedObject.GetSourceProperty()->GetReflectionProperty()->GetDisplayName().c_str());

		ImGui::SameLine(valuePos);

		AssetDatabase* assetDatabase = AssetDatabase::GetInstance();
		std::shared_ptr<Asset> asset = assetDatabase->Find(value->GetUid());

		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(ImGuiCol_FrameBg));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetColorU32(ImGuiCol_FrameBgActive));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetColorU32(ImGuiCol_FrameBgHovered));
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
		bool clicked = false;
		if (asset != nullptr)
		{
			clicked = ImageTextButton(asset->GetThumbnail(), ImVec2(ImGui::GetTextLineHeight(), ImGui::GetTextLineHeight()), asset->GetName().c_str(), ImVec2(-1, 0));
		}
		else
		{
			clicked = ImageTextButton(nullptr, ImVec2(0.0f, 0.0f), "None", ImVec2(-1, 0));
		}
		ImGui::PopStyleVar();
		ImGui::PopStyleColor(3);
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			assetList.clear();
			assetDatabase->ListAsset(assetList, assetDatabase->GetAssetRootNode(), value->GetResourceDescriptor());

			ImGui::OpenPopup("WeakResourceFindPopup");
		}
		else if (clicked == true && asset != nullptr)
		{
			AssetBrowserWindow* assetBrowserWindow = imgui::ImGuiManager::GetInstance()->FindWindow<AssetBrowserWindow>();
			if (assetBrowserWindow != nullptr)
			{
				assetBrowserWindow->PingAsset(*asset);
			}
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip) && asset != nullptr)
		{
			if (ImGui::BeginTooltip())
			{
				ImGui::Image(asset->GetThumbnail(), ImVec2(asset->GetThumbnail()->GetWidth() * 0.5f, asset->GetThumbnail()->GetHeight() * 0.5f));
				ImGui::EndTooltip();
			}
		}
		/*
		ImGui::Image(asset->GetThumbnail(), ImVec2(ImGui::GetTextLineHeight(), ImGui::GetTextLineHeight()));
		ImGui::SameLine();

		ImGui::TextUnformatted(asset->GetName().c_str());
		*/
		/*
		ImGui::SameLine(ImGui::GetContentRegionMax().x - CalculateButtonSize("Find").x);
		if (ImGui::Button("Find") == true)
		{
			assetList.clear();
			assetDatabase->ListAsset(assetList, assetDatabase->GetAssetRootNode(), value->GetResourceDescriptor());

			ImGui::OpenPopup("WeakResourceFindPopup");
		}
		*/
		if (ImGui::BeginPopup("WeakResourceFindPopup"))
		{
			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted("Name:");
			ImGui::SameLine();

			static char inputTextBuffer[2048] = "";
			ImGui::InputText("##Name", inputTextBuffer, sizeof(inputTextBuffer));
			ImGui::Separator();

			float itemHeight = 48; // todo max 32 - FontSize ?

			ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
			bool clicked = ImageTextButton(nullptr, ImVec2(0, itemHeight), "None", ImVec2(-1, 0));
			ImGui::PopStyleVar();
			if (clicked)
			{
				ReflectionPropertyObject* reflectionProperty = static_cast<ReflectionPropertyObject*>(reflectedObject.GetSourceProperty()->GetReflectionProperty());
				
				value->SetUid(UID::INVALID_UID);
				reflectionProperty->SetValue(reflectedObject.GetSourceProperty()->GetParent()->GetInstance(), value); // Set to itself for call SetFunction
			}

			for (AssetDatabase::FileSystemMapping* assetNode : assetList)
			{
				ImGui::PushID(assetNode);
				/*
				ImGui::Image(assetNode->_asset->GetThumbnail(), ImVec2(itemHeight, itemHeight));
				ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0, 0.5f));
				bool clicked = ImGui::Selectable(assetNode->_asset->GetName().c_str(), false, 0, ImVec2(0, itemHeight));
				ImGui::PopStyleVar();
				*/
				ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
				bool clicked = ImageTextButton(assetNode->_asset->GetThumbnail(), ImVec2(itemHeight, itemHeight), assetNode->_asset->GetName().c_str(), ImVec2(-1, 0));
				ImGui::PopStyleVar();
				if (clicked)
				{
					ReflectionPropertyObject* reflectionProperty = static_cast<ReflectionPropertyObject*>(reflectedObject.GetSourceProperty()->GetReflectionProperty());

					value->SetUid(assetNode->_asset->GetUid());
					reflectionProperty->SetValue(reflectedObject.GetSourceProperty()->GetParent()->GetInstance(), value); // Set to itself for call SetFunction
				}
				else if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					ImGui::CloseCurrentPopup();
				}
				ImGui::PopID();
			}

			ImGui::EndPopup();
		}

		ImGui::PopID();
		return changed;
	}
}
