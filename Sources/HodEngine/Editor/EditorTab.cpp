#include "HodEngine/Editor/Pch.hpp"
#include "EditorTab.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

namespace hod::editor
{
	/// @brief 
	/// @param title 
	EditorTab::EditorTab(std::string_view title)
	{
		_title = title;
		_dockSpaceId = ImGui::GetID(_title.c_str());
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
}
