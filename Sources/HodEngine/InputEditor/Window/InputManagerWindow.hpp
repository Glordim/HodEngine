#pragma once
#include <HodEngine/InputEditor/Export.hpp>

#include <HodEngine/ImGui/Window/Window.hpp>

#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>

namespace hod::editor
{
	class HOD_INPUT_EDITOR_API InputManagerWindow : public imgui::Window
	{
		REFLECTED_CLASS(InputManagerWindow, imgui::Window)

	public:

		void				DrawContent() override;
	};
}
