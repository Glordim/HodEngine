#pragma once
#include <HodEngine/InputEditor/Export.hpp>

#include <HodEngine/ImGui/Window/Window.hpp>

#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>

namespace hod::editor
{
	class HOD_INPUT_EDITOR_API InputManagerWindow : public imgui::Window
	{
		WINDOW_DESCRIPTION()
		REFLECTED_CLASS_NO_PARENT(InputManagerWindow)

	public:

		void				DrawContent() override;
	};
}
