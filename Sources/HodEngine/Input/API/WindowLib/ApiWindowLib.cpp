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
		AddDevice(&_mouse);
		AddDevice(&_keyboard);

		return true;
	}

	/// @brief
	ApiWindowLib::~ApiWindowLib() {}

	/// @brief
	void ApiWindowLib::UpdateDeviceValues()
	{
		window::DesktopWindow* window = static_cast<window::DesktopWindow*>(window::DisplayManager::GetInstance()->GetMainWindow());

		window::Event event;
		uint32_t      eventIndex = 0;
		uint32_t      eventMask = 1 << static_cast<uint32_t>(window::EventType::KeyPressed) | 1 << static_cast<uint32_t>(window::EventType::KeyReleased) |
		                     1 << static_cast<uint32_t>(window::EventType::MouseButtonDown) | 1 << static_cast<uint32_t>(window::EventType::MouseButtonUp) |
		                     1 << static_cast<uint32_t>(window::EventType::MouseMoved) | 1 << static_cast<uint32_t>(window::EventType::MouseScroll);
		while (window->PollEvent(eventIndex, event, eventMask))
		{
			switch (event.type)
			{
				case window::EventType::KeyPressed:
				{
					_keyboard.OnKeyPressed(event.data.key.scanCode);
				}
				break;

				case window::EventType::KeyReleased:
				{
					_keyboard.OnKeyReleased(event.data.key.scanCode);
				}
				break;

				case window::EventType::MouseButtonDown:
				{
					_mouse.OnButtonPressed(event.data.mouseButton.button);
				}
				break;

				case window::EventType::MouseButtonUp:
				{
					_mouse.OnButtonReleased(event.data.mouseButton.button);
				}
				break;

				case window::EventType::MouseMoved:
				{
					_mouse.OnButtonMoved(event.data.mouseMove.x, event.data.mouseMove.y);
				}
				break;

				case window::EventType::MouseScroll:
				{
					_mouse.OnButtonScroll(event.data.mouseScroll.value);
				}
				break;

				default: break;
			}
		}
		_mouse.UpdateState();
		_keyboard.UpdateState();
	}
}
