#include "HodEngine/InputEditor/Pch.hpp"

#include "HodEngine/InputEditor/Window/InputManagerWindow.hpp"
#include "HodEngine/InputEditor/Window/DeviceWindow.hpp"

#include <HodEngine/Input/InputManager.hpp>
#include <HodEngine/Input/Device.hpp>

#include <HodEngine/Core/Reflection/EnumTrait.hpp>

#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/ImGuiManager.hpp>

using namespace hod::input;

namespace hod::editor
{
	DECLARE_WINDOW_DESCRIPTION(InputManagerWindow, "Input Manager", true);
	DESCRIBE_REFLECTED_CLASS(InputManagerWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	const char* deviceTypeToIcon[EnumTrait::GetCount<Device::Type>()] = {
		ICON_MDI_CONTROLLER,
		ICON_MDI_KEYBOARD,
		ICON_MDI_MOUSE,
		"",
	};

	void InputManagerWindow::DrawContent()
	{
		if (ImGui::CollapsingHeader("Devices"))
		{
			ImGui::Indent();
			for (Device* device : InputManager::GetInstance()->GetDevices())
			{
				String label = std::format("{} {}##{}", deviceTypeToIcon[(uint8_t)device->GetType()], device->GetName(), (void*)device);
				bool selected = false;
				if (ImGui::Selectable(label.c_str(), &selected))
				{
					imgui::ImGuiManager::GetInstance()->OpenWindow<DeviceWindow>(device);
				}
			}
			ImGui::Unindent();
		}
	}
}
