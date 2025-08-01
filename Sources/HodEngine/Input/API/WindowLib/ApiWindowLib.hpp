#pragma once
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Input/Api.hpp"
#include "HodEngine/Input/InputId.hpp"
#include "HodEngine/Input/API/WindowLib/DeviceKeyboardWindowLib.hpp"

#include <HodEngine/Window/Desktop/IDesktopWindowInputListener.hpp>

#include "HodEngine/Core/Event.hpp"

#include <mutex>

namespace hod::input
{
	/// @brief 
	class HOD_INPUT_API ApiWindowLib : public Api, window::IDesktopWindowInputListener
	{
	public:

											ApiWindowLib();
											ApiWindowLib(const ApiWindowLib&) = delete;
											ApiWindowLib(ApiWindowLib&&) = delete;
											~ApiWindowLib() override;

		ApiWindowLib&						operator=(const ApiWindowLib&) = delete;
		ApiWindowLib&						operator=(ApiWindowLib&&) = delete;

	public:

		bool								Initialize() override;

		void								OnKeyPressed(int keyCode) override;
		void								OnKeyReleased(int keyCode) override;
		void								OnMouseButtonPressed(int button) override;
		void								OnMouseButtonReleased(int button) override;
		void								OnMouseMoved(float x, float y) override;

	protected:

		void								UpdateDeviceValues() override;

	private:

		DeviceKeyboardWindowLib				_keyboard;
		//DeviceMouseWindowLib				_mouse;
	};
}
