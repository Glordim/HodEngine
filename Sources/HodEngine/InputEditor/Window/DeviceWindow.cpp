#include "HodEngine/InputEditor/Pch.hpp"
#include "HodEngine/InputEditor/Window/DeviceWindow.hpp"

#include <HodEngine/Input/InputManager.hpp>
#include <HodEngine/Input/Devices/Device.hpp>
#include <HodEngine/Input/Input.hpp>

#include <HodEngine/Core/Reflection/EnumTrait.hpp>

#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/ImGuiManager.hpp>

#include "HodEngine/InputEditor/Helper.hpp"

using namespace hod::input;

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(DeviceWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	DeviceWindow::DeviceWindow(input::Device* device)
	: _device(device)
	{
		SetTitle(std::format("{} {}", deviceTypeToIcon[(uint8_t)device->GetType()], device->GetName()));
	}

	void DeviceWindow::DrawContent()
	{
		if (ImGui::CollapsingHeader("Raw State"))
		{
			const State* state = _device->GetCurrentState();
			const uint32_t stateSize = _device->GetStateSize();

			uint32_t byteIndex = 0;

			uint32_t lineCount = (stateSize / 8);
			if (stateSize % 8 > 0)
			{
				++lineCount;
			}

			for (uint32_t line = 0; line < lineCount; ++line)
			{
				ImGui::Text("#%u", line * 8);
				ImGui::SameLine(0.0f, 35.0f);

				for (uint32_t byteInThisLine = 0; byteInThisLine < 8 && byteIndex < stateSize; ++byteInThisLine)
				{
					uint8_t byte = *reinterpret_cast<const uint8_t*>(reinterpret_cast<uintptr_t>(state) + byteIndex);

					for (uint32_t bitIndex = 0; bitIndex < 8; ++bitIndex)
					{
						if ((byte & (1 << (7 - bitIndex))) != 0)
							ImGui::Text("1");
						else
							ImGui::Text("0");

						ImGui::SameLine(0.0f, 0.0f);
					}

					ImGui::SameLine(0.0f, 10.0f);

					++byteIndex;
				}

				ImGui::NewLine();
			}
		}
		if (ImGui::BeginTable("InputTable", 7, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_RowBg))
		{
			ImGui::TableSetupColumn("Name");
			ImGui::TableSetupColumn("Display Name");
			ImGui::TableSetupColumn("Type");
			ImGui::TableSetupColumn("Byte Offset");
			ImGui::TableSetupColumn("Bit Offset");
			ImGui::TableSetupColumn("Size (bit)");
			ImGui::TableSetupColumn("Value");
			ImGui::TableHeadersRow();
			
			for (Input* input : _device->GetInputs())
			{
				ImGui::TableNextRow();

				ImGui::TableNextColumn();
				ImGui::TextUnformatted(input->GetDisplayName().c_str());

				ImGui::TableNextColumn();
				ImGui::TextUnformatted(input->GetDisplayName().c_str());

				ImGui::TableNextColumn();
				ImGui::TextUnformatted(input->GetReflectionDescriptorV().GetTypeName().c_str());

				ImGui::TableNextColumn();
				ImGui::Text("%u", input->GetStateView().GetByteOffset());

				ImGui::TableNextColumn();
				ImGui::Text("%u", input->GetStateView().GetBitOffset());

				ImGui::TableNextColumn();
				ImGui::Text("%u", input->GetStateView().GetSizeInBit());

				ImGui::TableNextColumn();
				ImGui::TextUnformatted(input->ToString().c_str());
			}

			ImGui::EndTable();
		}
	}
}
