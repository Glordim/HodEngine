#pragma once
#include <HodEngine/InputEditor/Export.hpp>

#include <HodEngine/ImGui/Window/Window.hpp>

#include <HodEngine/Input/FeedbackVibration.hpp>

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
		DeviceWindow(Device* device);
		~DeviceWindow() override;

		void				DrawContent() override;

	private:

		void				DrawFeedbacks();
		void				StopVibration();

	private:

		Device* _device;

		FeedbackVibration	_vibration;					// Tested from the window (DrawFeedbacks)
		bool				_vibrating = false;			// Started from the window, stopped when it closes
		bool				_vibrationSupported = true;	// What the last ApplyFeedback answered
	};
}
