#pragma once

#include <HodEngine/ImGui/MainBar.h>

namespace hod::editor
{
	/// @brief 
	class MainBar : public imgui::MainBar
	{
	public:

					MainBar() = default;
					~MainBar() override = default;

	public:

		void		Draw() override;
	};
}
