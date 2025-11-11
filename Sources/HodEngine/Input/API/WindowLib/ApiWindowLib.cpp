#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/API/WindowLib/ApiWindowLib.hpp"

#include "HodEngine/Input/InputManager.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#include <HodEngine/Window/Desktop/DesktopWindow.hpp>
#include <HodEngine/Window/DisplayManager.hpp>

namespace hod::input
{
	/// @brief
	ApiWindowLib::ApiWindowLib()
	: Api("WindowLib")
	{
	}

	/// @brief
	/// @return
	bool ApiWindowLib::Initialize()
	{
		window::DesktopWindow* window = static_cast<window::DesktopWindow*>(window::DisplayManager::GetInstance()->GetMainWindow());
		window->RegisterInputListener(this);

		AddDevice(&_mouse);
		AddDevice(&_keyboard);

		return true;
	}

	/// @brief
	ApiWindowLib::~ApiWindowLib()
	{
		window::DesktopWindow* window = static_cast<window::DesktopWindow*>(window::DisplayManager::GetInstance()->GetMainWindow());
		window->UnregisterInputListener(this);
	}

	/// @brief
	void ApiWindowLib::UpdateDeviceValues()
	{
		_mouse.UpdateState();
		_keyboard.UpdateState();
	}

	void ApiWindowLib::OnKeyPressed(window::ScanCode scanCode)
	{
		_keyboard.OnKeyPressed(scanCode);
	}

	void ApiWindowLib::OnKeyReleased(window::ScanCode scanCode)
	{
		_keyboard.OnKeyReleased(scanCode);
	}

	void ApiWindowLib::OnMouseButtonPressed(int button)
	{
		_mouse.OnButtonPressed(button);
	}

	void ApiWindowLib::OnMouseButtonReleased(int button)
	{
		_mouse.OnButtonReleased(button);
	}

	void ApiWindowLib::OnMouseMoved(float x, float y)
	{
		// todo
		(void)x;
		(void)y;
	}
}
