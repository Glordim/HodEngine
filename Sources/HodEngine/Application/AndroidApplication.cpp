#include "HodEngine/Application/Pch.hpp"
#include "HodEngine/Application/AndroidApplication.hpp"

#include <game-activity/GameActivity.h>
#include <game-activity/native_app_glue/android_native_app_glue.h>

namespace hod::inline application
{
	bool AndroidApplication::Run(android_app* androidApp)
	{
		_androidApp = androidApp;
		return RunInternal();
	}

	void AndroidApplication::ConfigureFileSystem(FileSystemConfig& fileSystemConfig)
	{
		fileSystemConfig._assetManager = _androidApp->activity->assetManager;
	}
}

/*
while (_androidApp->window == nullptr)
		{
			int                  timeout = 0; // non-blocking
			int                  events;
			android_poll_source* source;
			int                  result = ALooper_pollOnce(timeout, nullptr, &events, reinterpret_cast<void**>(&source));
			switch (result)
			{
				case ALOOPER_POLL_TIMEOUT: [[fallthrough]];
				case ALOOPER_POLL_WAKE:
					// No events occurred before the timeout or explicit wake. Stop checking for events.
					break;
				case ALOOPER_EVENT_ERROR: OUTPUT_ERROR("ALooper_pollOnce returned an error"); break;
				case ALOOPER_POLL_CALLBACK: break;
				default:
					if (source)
					{
						source->process(_androidApp, source);
					}
			}
		}

		_window = androidDisplayManager->CreateMainWindow(_androidApp->window);
*/

/*
/// @brief
	/// @param androidApp
	/// @param command
	void AndroidApplication::CommandHandleStatic(android_app* androidApp, int32_t command)
	{
		static_cast<AndroidApplication*>(androidApp->userData)->CommandHandle(command);
	}

	/// @brief
	/// @param command
	void AndroidApplication::CommandHandle(int32_t command)
	{
		switch (command)
		{
			case APP_CMD_INIT_WINDOW:
				// A new window is created, associate a renderer with it. You may replace this with a
				// "game" class if that suits your needs. Remember to change all instances of userData
				// if you change the class here as a reinterpret_cast is dangerous this in the
				// android_main function and the APP_CMD_TERM_WINDOW handler case.
				break;
			case APP_CMD_TERM_WINDOW:
				// The window is being destroyed. Use this to clean up your userData to avoid leaking
				// resources.
				// TODO
				break;
			default: break;
		}
	}
*/