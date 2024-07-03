#pragma once
#include <HodEngine/HodEngine.hpp>

#include <HodEngine/ImGui/MainBar.hpp>

namespace hod::editor
{
	/// @brief 
	class HOD_API MainBar : public imgui::MainBar
	{
	public:

					MainBar() = default;
					~MainBar() override = default;

	public:

		void		Draw() override;
	};
}
