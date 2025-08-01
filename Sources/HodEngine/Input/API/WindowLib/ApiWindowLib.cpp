#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/API/WindowLib/ApiWindowLib.hpp"

#include "HodEngine/Input/InputManager.hpp"
#include "HodEngine/Input/API/WindowLib/DeviceKeyboardWindowLib.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#include <HodEngine/Window/DisplayManager.hpp>
#include <HodEngine/Window/Desktop/DesktopWindow.hpp>

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
		
	}

	void ApiWindowLib::OnKeyPressed(int keyCode)
	{
		_keyboard.OnKeyPressed(keyCode);
	}

	void ApiWindowLib::OnKeyReleased(int keyCode)
	{
		_keyboard.OnKeyReleased(keyCode);
	}

	void ApiWindowLib::OnMouseButtonPressed(int button)
	{
		// todo
		(void)button;
	}

	void ApiWindowLib::OnMouseButtonReleased(int button)
	{
		// todo
		(void)button;
	}

	void ApiWindowLib::OnMouseMoved(float x, float y)
	{
		// todo
		(void)x;
		(void)y;
	}
}
