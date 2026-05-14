#pragma once
#include "HodEngine/Window/Export.hpp"

#include "HodEngine/Window/DisplayManager.hpp"

struct ANativeWindow;
struct android_app;

namespace hod::inline window
{
	class Window;

	class HOD_WINDOW_API AndroidDisplayManager : public DisplayManager
	{
		_SingletonOverride(AndroidDisplayManager)

	public:

		bool    Initialize() override { return false; };
		bool    Initialize(android_app* androidApp);
		void    Update() override {};
		bool    Run() override;
		void    Terminate() override {}

		Window* CreateMainWindow(ANativeWindow* nativeWindow);

	private:
		static void CommandHandleStatic(android_app* androidApp, int32_t command);
		void        CommandHandle(int32_t command);

	private:
		android_app* _androidApp;
	};
}
