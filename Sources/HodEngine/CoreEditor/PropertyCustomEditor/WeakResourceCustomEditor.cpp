#include "HodEngine/CoreEditor/Pch.hpp"
#include "HodEngine/CoreEditor/PropertyCustomEditor/WeakResourceCustomEditor.hpp"

#include "HodEngine/Core/Math/Vector2.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp"

#include "HodEngine/ImGui/DearImGui/imgui.h"
#include "HodEngine/ImGui/Helper.hpp"
#include "HodEngine/ImGui/Widgets.hpp"

#include "HodEngine/Core/Resource/WeakResource.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"

#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Asset.hpp"

#include "HodEngine/Renderer/RHI/Texture.hpp"

#include "HodEngine/ImGui/ImGuiManager.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"

#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#undef GetObject

namespace hod::editor
{
	/// @brief 
	/// @param instance 
	/// @return 
	bool WeakResourceCustomEditor::Draw(EditorReflectedProperty& editorReflectedProperty)
	{
		bool changed = false;
		changed |= PropertyDrawer::BeginProperty(editorReflectedProperty);
		ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);

		WeakResourceBase* value = editorReflectedProperty.GetObject<WeakResourceBase>();
		changed |= WeakResourceCustomEditor::Draw(*value);
		if (changed == true)
		{
			editorReflectedProperty.SetObject(*value); // Set to itself for call SetFunction // todo affect a new object, wrong if SetFunction check if the value is the same
		}
		return changed;
	}

	/// @brief 
	/// @param weakResource 
	/// @return 
	bool WeakResourceCustomEditor::Draw(WeakResourceBase& weakResource)
	{
		static Vector<AssetDatabase::FileSystemMapping*> assetList;

		bool changed = false;
		ImGui::PushID(&weakResource);

		AssetDatabase* assetDatabase = AssetDatabase::GetInstance();
		std::shared_ptr<Asset> asset = assetDatabase->Find(weakResource.GetUid());

		float height = 50;

		ImVec2 prevCursorPos = ImGui::GetCursorPos();

		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(ImGuiCol_FrameBg));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetColorU32(ImGuiCol_FrameBgActive));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetColorU32(ImGuiCol_FrameBgHovered));
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
		bool clicked = false;
		if (asset != nullptr)
		{
			//clicked = ImageTextButton(asset->GetThumbnail(), ImVec2((float)asset->GetThumbnail()->GetWidth(), (float)asset->GetThumbnail()->GetHeight()), ImVec2(height, height), asset->GetName().c_str(), ImVec2(-1, height + ImGui::GetStyle().FramePadding.y * 2));
			if (asset->GetThumbnail() != nullptr)
			{
				PreserveAspectImage(asset->GetThumbnail(), ImVec2((float)asset->GetThumbnail()->GetWidth(), (float)asset->GetThumbnail()->GetHeight()), ImVec2(height, height));
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
				{
					if (ImGui::BeginTooltip())
					{
						ImVec2 size = ImVec2(256.0f, ((float)asset->GetThumbnail()->GetHeight() / (float)asset->GetThumbnail()->GetWidth()) * 256.0f);
						ImGui::Image(asset->GetThumbnail(), size);
						ImGui::EndTooltip();
					}
				}
			}
			else
			{
				PreserveAspectImage(nullptr, ImVec2(0.0f, 0.0f), ImVec2(height, height));
			}

			ImGui::SameLine();

			prevCursorPos = ImGui::GetCursorPos();

			ImGui::SetNextItemWidth(-1);
			if (ImGui::BeginCombo("", asset->GetName().c_str(), ImGuiComboFlags_HeightLarge))
			{
				if (clicked == false)
				{
					clicked = true;

					assetList.clear();
					assetDatabase->ListAsset(assetList, assetDatabase->GetAssetRootNode(), weakResource.GetResourceDescriptor());
				}
			}
		}
		else
		{
			//clicked = ImageTextButton(nullptr, ImVec2(0.0f, 0.0f), ImVec2(0.0f, 0.0f), "None", ImVec2(-1, height + ImGui::GetStyle().FramePadding.y * 2));
			PreserveAspectImage(nullptr, ImVec2(0.0f, 0.0f), ImVec2(height, height));
			ImGui::SameLine();

			prevCursorPos = ImGui::GetCursorPos();

			ImGui::SetNextItemWidth(-1);
			if (ImGui::BeginCombo("", "None", ImGuiComboFlags_HeightLarge))
			{
				if (clicked == false)
				{
					clicked = true;

					assetList.clear();
					assetDatabase->ListAsset(assetList, assetDatabase->GetAssetRootNode(), weakResource.GetResourceDescriptor());
				}
			}
		}
		ImGui::PopStyleVar();
		ImGui::PopStyleColor(3);
		if (clicked)
		{
			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted("Name:");
			ImGui::SameLine();
			
			static char inputTextBuffer[2048] = "";
			if (ImGui::IsWindowAppearing())
			{
				inputTextBuffer[0] = '\0';
				ImGui::SetKeyboardFocusHere();
			}
			ImGui::InputText("##Name", inputTextBuffer, sizeof(inputTextBuffer));
			ImGui::Separator();

			float itemHeight = 48; // todo max 32 - FontSize ?
			float contentHeight = itemHeight - ImGui::GetStyle().FramePadding.y * 2;

			ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
			bool clicked = ImageTextButton(nullptr, ImVec2(0, 0), ImVec2(0, 0), "None", ImVec2(-1, itemHeight));
			ImGui::PopStyleVar();
			if (clicked)
			{
				weakResource.SetUid(UID::INVALID_UID);
				changed = true;

				ImGui::CloseCurrentPopup();
			}

			for (AssetDatabase::FileSystemMapping* assetNode : assetList)
			{
				if (inputTextBuffer[0] == '\0' || assetNode->_asset->GetName().find(inputTextBuffer) != std::string::npos)
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
					ImVec2 imageSize = ImVec2(0, 0);
					if (assetNode->_asset->GetThumbnail())
					{
						imageSize = ImVec2((float)assetNode->_asset->GetThumbnail()->GetWidth(), (float)assetNode->_asset->GetThumbnail()->GetHeight());
					}
					bool clicked = ImageTextButton(assetNode->_asset->GetThumbnail(), imageSize, ImVec2(contentHeight, contentHeight), assetNode->_asset->GetName().c_str(), ImVec2(-1, itemHeight));
					ImGui::PopStyleVar();
					if (clicked)
					{
						weakResource.SetUid(assetNode->_asset->GetUid());
						changed = true;

						ImGui::CloseCurrentPopup();
					}
					ImGui::PopID();
				}
			}

			ImGui::EndCombo();
			clicked = false;
		}

		ImVec2 cursorToRestore = ImGui::GetCursorPos();
		prevCursorPos.y += CalculateButtonSize("Toto").y + 4;
		ImGui::SetCursorPos(prevCursorPos);
		if (asset != nullptr && ImGui::Button(ICON_MDI_MAGNIFY))
		{
			Editor::GetInstance()->PingAsset(asset);
		}
		ImGui::SetCursorPos(cursorToRestore);

		/*
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			assetList.clear();
			assetDatabase->ListAsset(assetList, assetDatabase->GetAssetRootNode(), value->GetResourceDescriptor());

			ImGui::OpenPopup("WeakResourceFindPopup");
		}
		else if (clicked == true && asset != nullptr)
		{
			Editor::GetInstance()->PingAsset(asset);
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip) && asset != nullptr && asset->GetThumbnail() != nullptr)
		{
			if (ImGui::BeginTooltip())
			{
				ImVec2 size = ImVec2(128.0f, ((float)asset->GetThumbnail()->GetHeight() / (float)asset->GetThumbnail()->GetWidth()) * 128.0f);
				ImGui::Image(asset->GetThumbnail(), size);
				ImGui::EndTooltip();
			}
		}
		*/
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
			if (ImGui::IsWindowAppearing())
			{
				inputTextBuffer[0] = '\0';
				ImGui::SetKeyboardFocusHere();
			}
			ImGui::InputText("##Name", inputTextBuffer, sizeof(inputTextBuffer));
			ImGui::Separator();

			float itemHeight = 48; // todo max 32 - FontSize ?
			float contentHeight = itemHeight - ImGui::GetStyle().FramePadding.y * 2;

			ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
			bool clicked = ImageTextButton(nullptr, ImVec2(0, 0), ImVec2(0, 0), "None", ImVec2(-1, itemHeight));
			ImGui::PopStyleVar();
			if (clicked)
			{
				weakResource.SetUid(UID::INVALID_UID);
				changed = true;
			}

			for (AssetDatabase::FileSystemMapping* assetNode : assetList)
			{
				if (inputTextBuffer[0] == '\0' || assetNode->_asset->GetName().find(inputTextBuffer) != std::string::npos)
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
					ImVec2 imageSize = ImVec2(0, 0);
					if (assetNode->_asset->GetThumbnail())
					{
						imageSize = ImVec2((float)assetNode->_asset->GetThumbnail()->GetWidth(), (float)assetNode->_asset->GetThumbnail()->GetHeight());
					}
					bool clicked = ImageTextButton(assetNode->_asset->GetThumbnail(), imageSize, ImVec2(contentHeight, contentHeight), assetNode->_asset->GetName().c_str(), ImVec2(-1, itemHeight));
					ImGui::PopStyleVar();
					if (clicked)
					{
						weakResource.SetUid(assetNode->_asset->GetUid());
						changed = true;
					}
					else if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						ImGui::CloseCurrentPopup();
					}
					ImGui::PopID();
				}
			}

			ImGui::EndPopup();
		}

		ImGui::Dummy(ImVec2(0,0)); // todo ?

		ImGui::PopID();
		return changed;
	}
}
