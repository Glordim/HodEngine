#pragma once
#include <HodEngine/InputEditor/Export.hpp>

#include <HodEngine/ImGui/Window/Window.hpp>

#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>

namespace hod::input
{
	class Device;
}

namespace hod::editor
{
	class HOD_INPUT_EDITOR_API DeviceWindow : public imgui::Window
	{
		WINDOW_DESCRIPTION()
		REFLECTED_CLASS_NO_PARENT(DeviceWindow)

	public:

		DeviceWindow() = default; // TODO REMOVE
		DeviceWindow(input::Device* device);

		void				DrawContent() override;

	private:

		input::Device* _device;
	};
}
