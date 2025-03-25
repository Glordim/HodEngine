#pragma once

#include <string>
#include <string_view>

#include <HodEngine/ImGui/DearImGui/imgui.h>

namespace hod::editor
{
	class EditorTab
	{
	public:

				EditorTab(std::string_view title);

		bool	Draw();

	protected:

		virtual void	CreateDefaultLayout() = 0;
		virtual bool	DrawContent() = 0;

	protected:
		std::string		_title;
		ImGuiID 		_dockSpaceId;

	private:
		bool			_initialLayoutCreated = false;
	};
}
