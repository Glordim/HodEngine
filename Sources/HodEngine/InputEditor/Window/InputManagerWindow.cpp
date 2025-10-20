#include "HodEngine/InputEditor/Pch.hpp"

#include "HodEngine/InputEditor/Window/DeviceWindow.hpp"
#include "HodEngine/InputEditor/Window/InputManagerWindow.hpp"

#include <HodEngine/Input/Devices/Device.hpp>
#include <HodEngine/Input/InputManager.hpp>

#include <HodEngine/Core/Reflection/EnumTrait.hpp>

#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/ImGuiManager.hpp>

#include "HodEngine/InputEditor/Helper.hpp"

using namespace hod::input;

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(InputManagerWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	void InputManagerWindow::DrawContent()
	{
		if (ImGui::CollapsingHeader("Devices"))
		{
			ImGui::Indent();
			for (Device* device : InputManager::GetInstance()->GetDevices())
			{
				String label = std::format("{} {}##{}", deviceTypeToIcon[(uint8_t)device->GetType()], device->GetName(), (void*)device).c_str();
				bool   selected = false;
				if (ImGui::Selectable(label.CStr(), &selected))
				{
					imgui::ImGuiManager::GetInstance()->OpenWindow<DeviceWindow>(device);
				}
			}
			ImGui::Unindent();
		}
	}
}
