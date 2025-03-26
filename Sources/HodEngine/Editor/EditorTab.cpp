#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/EditorTab.hpp"

#include "HodEngine/Editor/Asset.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

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

		//ICON_MDI_STAR_FOUR_POINTS_SMALL

		_dockSpaceId = reinterpret_cast<uintptr_t>(this);
	}

	/// @brief 
	/// @return 
	bool EditorTab::Draw()
	{
		bool open = true;
		if (ImGui::BeginTabItem(_title.c_str(), &open))
		{
			if (_initialLayoutCreated == false)
			{
				CreateDefaultLayout();
				_initialLayoutCreated = true;
			}
			ImGui::DockSpace(_dockSpaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
			open = DrawContent();
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
