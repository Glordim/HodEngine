#pragma once
#include <HodEngine/InputEditor/Export.hpp>

#include <HodEngine/ImGui/Window/Window.hpp>

#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>

namespace hod::inline input
{
	class Device;
}

namespace hod::inline editor
{
	class HOD_INPUT_EDITOR_API DeviceWindow : public Window
	{
		REFLECTED_CLASS(DeviceWindow, Window)

	public:

		DeviceWindow() = default; // TODO REMOVE
		DeviceWindow(input::Device* device);

		void				DrawContent() override;

	private:

		input::Device* _device;
	};
}
