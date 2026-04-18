#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/API/WindowLib/ApiWindowLib.hpp"

#include "HodEngine/Input/InputManager.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#include <HodEngine/Window/Desktop/DesktopWindow.hpp>
#include <HodEngine/Window/DisplayManager.hpp>

namespace hod::inline input
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
		DesktopWindow* window = static_cast<DesktopWindow*>(DisplayManager::GetInstance()->GetMainWindow());

		window::Event event;
		uint32_t      eventIndex = 0;
		uint32_t      eventMask = 1 << static_cast<uint32_t>(EventType::KeyPressed) | 1 << static_cast<uint32_t>(EventType::KeyReleased) |
		                     1 << static_cast<uint32_t>(EventType::MouseButtonDown) | 1 << static_cast<uint32_t>(EventType::MouseButtonUp) |
		                     1 << static_cast<uint32_t>(EventType::MouseMoved) | 1 << static_cast<uint32_t>(EventType::MouseScroll);
		while (window->PollEvent(eventIndex, event, eventMask))
		{
			switch (event.type)
			{
				case EventType::KeyPressed:
				{
					_keyboard.OnKeyPressed(event.data.key.scanCode);
				}
				break;

				case EventType::KeyReleased:
				{
					_keyboard.OnKeyReleased(event.data.key.scanCode);
				}
				break;

				case EventType::MouseButtonDown:
				{
					_mouse.OnButtonPressed(event.data.mouseButton.button);
				}
				break;

				case EventType::MouseButtonUp:
				{
					_mouse.OnButtonReleased(event.data.mouseButton.button);
				}
				break;

				case EventType::MouseMoved:
				{
					_mouse.OnButtonMoved(event.data.mouseMove.x, event.data.mouseMove.y);
				}
				break;

				case EventType::MouseScroll:
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
