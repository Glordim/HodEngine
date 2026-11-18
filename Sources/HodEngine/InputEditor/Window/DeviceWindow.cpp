#include "HodEngine/InputEditor/Pch.hpp"
#include "HodEngine/InputEditor/Window/DeviceWindow.hpp"

#include <HodEngine/Input/Devices/Device.hpp>
#include <HodEngine/Input/Input.hpp>
#include <HodEngine/Input/InputManager.hpp>

#include <HodEngine/Core/Reflection/EnumTrait.hpp>

#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/ImGuiManager.hpp>

#include "HodEngine/InputEditor/Helper.hpp"

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(DeviceWindow) {}

	DeviceWindow::DeviceWindow(Device* device)
	: _device(device)
	{
		SetTitle(fmt::format("{} {}", deviceTypeToIcon[(uint8_t)device->GetType()], device->GetName()));
	}

	DeviceWindow::~DeviceWindow()
	{
		StopVibration();
	}

	/// @brief Stops the vibration started from the window, if any
	void DeviceWindow::StopVibration()
	{
		if (_vibrating)
		{
			FeedbackVibration stop;
			stop._left = 0.0f;
			stop._right = 0.0f;
			stop._leftTrigger = 0.0f;
			stop._rightTrigger = 0.0f;
			_device->ApplyFeedback(stop);
			_vibrating = false;
		}
	}

	/// @brief The feedbacks of a gamepad: its motors' speeds, applied while started (changed live), stopped with the window
	void DeviceWindow::DrawFeedbacks()
	{
		if (ImGui::CollapsingHeader("Feedbacks") == false)
		{
			return;
		}

		ImGui::BeginDisabled(_device->IsConnected() == false);

		bool changed = false;
		changed |= ImGui::SliderFloat("Left (low frequency)", &_vibration._left, 0.0f, 1.0f);
		changed |= ImGui::SliderFloat("Right (high frequency)", &_vibration._right, 0.0f, 1.0f);
		changed |= ImGui::SliderFloat("Left Trigger", &_vibration._leftTrigger, 0.0f, 1.0f);
		changed |= ImGui::SliderFloat("Right Trigger", &_vibration._rightTrigger, 0.0f, 1.0f);

		if (_vibrating == false)
		{
			if (ImGui::Button(ICON_MDI_VIBRATE " Start"))
			{
				_vibrationSupported = _device->ApplyFeedback(_vibration);
				_vibrating = _vibrationSupported;
			}
		}
		else
		{
			if (changed)
			{
				_device->ApplyFeedback(_vibration);
			}
			if (ImGui::Button(ICON_MDI_STOP " Stop"))
			{
				StopVibration();
			}
		}

		ImGui::EndDisabled();

		if (_device->IsConnected() == false)
		{
			_vibrating = false; // Nothing to stop anymore
			ImGui::TextDisabled("Disconnected");
		}
		else if (_vibrationSupported == false)
		{
			ImGui::TextDisabled("Vibration not supported by this device");
		}
	}

	void DeviceWindow::DrawContent()
	{
		if (_device->GetType() == Device::Type::PAD)
		{
			DrawFeedbacks();
		}

		if (ImGui::CollapsingHeader("Raw State"))
		{
			const State*   state = _device->GetCurrentState();
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
						{
							ImGui::Text("1");
						}
						else
						{
							ImGui::Text("0");
						}

						ImGui::SameLine(0.0f, 0.0f);
					}

					ImGui::SameLine(0.0f, 10.0f);

					++byteIndex;
				}

				ImGui::NewLine();
			}
		}
		if (ImGui::BeginTable("InputTable", 7, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_RowBg | ImGuiTableFlags_PadOuterX))
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
				ImGui::TextUnformatted(input->GetDisplayName().CStr());

				ImGui::TableNextColumn();
				ImGui::TextUnformatted(input->GetDisplayName().CStr());

				ImGui::TableNextColumn();
				ImGui::TextUnformatted(input->GetReflectionDescriptorV().GetTypeName().CStr());

				ImGui::TableNextColumn();
				ImGui::Text("%u", input->GetStateView().GetByteOffset());

				ImGui::TableNextColumn();
				ImGui::Text("%u", input->GetStateView().GetBitOffset());

				ImGui::TableNextColumn();
				ImGui::Text("%u", input->GetStateView().GetSizeInBit());

				ImGui::TableNextColumn();
				ImGui::TextUnformatted(input->ToString().CStr());
			}

			ImGui::EndTable();
		}
	}
}
