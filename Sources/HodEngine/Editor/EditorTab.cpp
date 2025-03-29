#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/EditorTab.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"

#include "HodEngine/Editor/Asset.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

namespace hod::editor
{
	/// @brief 
	/// @param asset 
	EditorTab::EditorTab(std::shared_ptr<Asset> asset, const char* icon)
	{
		_asset = asset;
		if (_asset == nullptr)
		{
			_title = std::format("{}   Untitled", icon);
		}
		else
		{
			_title = std::format("{}   {}", icon, _asset->GetName());
		}

		_dockSpaceId = reinterpret_cast<uintptr_t>(this);
	}

	/// @brief 
	EditorTab::~EditorTab()
	{
		for (EditorTabWindow* window : _windows)
		{
			delete window;
		}
	}

	/// @brief 
	/// @return 
	bool EditorTab::Draw()
	{
		bool open = true;
		ImGui::SetNextItemWidth(210);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));
		bool draw = ImGui::BeginTabItem(_title.c_str(), &open, AssetIsDirty() ? ImGuiTabItemFlags_UnsavedDocument : ImGuiTabItemFlags_None);
		ImGui::PopStyleVar();
		if (draw)
		{
			float y = ImGui::GetCursorScreenPos().y - ImGui::GetStyle().ItemSpacing.y;
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(0.0f, y), ImVec2(ImGui::GetIO().DisplaySize.x, y + 32), ImGui::GetColorU32(ImGuiCol_TabSelected));

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
			ImGui::PushStyleVarX(ImGuiStyleVar_FramePadding, 12.0f);

			ImVec2 cursorPos = ImGui::GetCursorScreenPos();
			cursorPos.x = 0.0f;
			cursorPos.y += 32.0f;

			ImGui::SetCursorScreenPos(ImVec2(0.0f, y));
			if (ImGui::Button(ICON_MDI_CONTENT_SAVE, ImVec2(0.0f, 32.0f)))
			{

			}
			ImGui::SameLine();
			if (ImGui::Button(ICON_MDI_MAGNIFY, ImVec2(0.0f, 32.0f)))
			{

			}
			ImGui::SameLine();
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, 2.0f);
			DrawMenuBar();

			ImGui::PopStyleVar(3);
			ImGui::PopStyleColor();
			ImGui::SetCursorScreenPos(cursorPos);

			if (_initialLayoutCreated == false)
			{
				CreateDefaultLayout();
				_initialLayoutCreated = true;
			}
			ImGui::DockSpace(_dockSpaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
			auto it = _windows.begin();
			auto itEnd = _windows.end();
			while (it != itEnd)
			{
				(*it)->Draw();
				if ((*it)->IsClosed())
				{
					delete (*it);
					_windows.erase(it);
					itEnd = _windows.end();
				}
				else
				{
					++it;
				}
			}

			if (DrawContent() == false)
			{
				open = false;
			}
			ImGui::EndTabItem();
		}
		return open;
	}

	/// @brief 
	/// @return 
	std::shared_ptr<Asset> EditorTab::GetAsset() const
	{
		return _asset;
	}

	/// @brief 
	/// @return 
	bool EditorTab::AssetIsDirty() const
	{
		if (_asset)
		{
			return _asset->IsDirty();
		}
		else
		{
			return true;
		}
	}

	/// @brief 
	void EditorTab::MarkAssetAsDirty()
	{
		if (_asset)
		{
			return _asset->SetDirty();
		}
	}
}
