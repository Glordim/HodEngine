#pragma once
#include "HodEngine/Editor/Export.hpp"
#include "HodEngine/Editor/FpsCounter.hpp"

#include <HodEngine/ImGui/MainBar.hpp>

namespace hod::editor
{
	/// @brief 
	class HOD_EDITOR_API MainBar : public imgui::MainBar
	{
	public:

					MainBar() = default;
					~MainBar() override = default;

	public:

		void		Draw() override;

	private:

		FpsCounter	_fpsCounter;
	};
}
