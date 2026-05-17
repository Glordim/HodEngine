#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Input/API/Android/AndroidInput.hpp"

#include "HodEngine/Input/InputId.hpp"
#include <HodEngine/Window/Android/AndroidDisplayManager.hpp>

#include <game-activity/GameActivity.h>
#include <game-activity/native_app_glue/android_native_app_glue.h>

namespace hod::inline input
{
	/// @brief
	AndroidInput::AndroidInput()
	: Api("AndroidInput")
	{
	}

	/// @brief
	AndroidInput::~AndroidInput()
	{
	}

	/// @brief
	/// @return
	bool AndroidInput::Initialize()
	{
		AddDevice(&_mouse);

		SetInitialized(true);
		return true;
	}

	/// @brief
	void AndroidInput::UpdateDeviceValues()
	{
		android_app* androidApp = AndroidDisplayManager::GetInstance()->GetAndroidApp();
		android_input_buffer* inputBuffer = android_app_swap_input_buffers(androidApp);
		if (inputBuffer != nullptr)
		{
			for (int i = 0; i < inputBuffer->motionEventsCount; ++i)
			{
				GameActivityMotionEvent& event = inputBuffer->motionEvents[i];
				const int32_t action = event.action & AMOTION_EVENT_ACTION_MASK;

				/*
				const float x = GameActivityPointerAxes_getX(&event.pointers[0]);
				const float y = GameActivityPointerAxes_getY(&event.pointers[0]);
				*/

				switch (action)
				{
					case AMOTION_EVENT_ACTION_DOWN:
						_mouse.OnButtonPressed(MouseButton::Left);
						break;
					case AMOTION_EVENT_ACTION_UP:
						_mouse.OnButtonReleased(MouseButton::Left);
						break;
					default:
						break;
				}
			}

			/*
			for (int i = 0; i < inputBuffer->keyEventsCount; ++i)
			{
				GameActivityKeyEvent& event = inputBuffer->keyEvents[i];
			}
			*/

			android_app_clear_motion_events(inputBuffer);
			android_app_clear_key_events(inputBuffer);
		}
		_mouse.UpdateState();
	}
}
