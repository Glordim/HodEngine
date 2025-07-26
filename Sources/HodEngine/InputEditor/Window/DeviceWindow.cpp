#include "HodEngine/InputEditor/Pch.hpp"
#include "HodEngine/InputEditor/Window/DeviceWindow.hpp"

#include <HodEngine/Input/InputManager.hpp>
#include <HodEngine/Input/Device.hpp>

#include <HodEngine/Core/Reflection/EnumTrait.hpp>

#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/ImGuiManager.hpp>

using namespace hod::input;

namespace hod::editor
{
	DECLARE_WINDOW_DESCRIPTION(DeviceWindow, "Device", false);
	DESCRIBE_REFLECTED_CLASS(DeviceWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	DeviceWindow::DeviceWindow(input::Device* device)
	: _device(device)
	{
		SetTitle(device->GetName());
	}

	void DeviceWindow::DrawContent()
	{
		
	}
}
