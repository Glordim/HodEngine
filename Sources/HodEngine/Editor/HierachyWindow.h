#pragma once

#include <HodEngine/ImGui/Window.h>

namespace hod::game
{
	class Entity;
}

namespace hod::editor
{
	/// @brief 
	class HierachyWindow : public imgui::Window
	{
		META_TYPE(HierachyWindow);
		WINDOW_DESCRIPTION()

	public:

					HierachyWindow() = default;
					~HierachyWindow() override = default;

	public:

		void		Draw() override;

	private:

		std::weak_ptr<game::Entity>	_selection;
	};
}
