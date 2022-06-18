#pragma once

#include <HodEngine/ImGui/src/MainBar.h>

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
